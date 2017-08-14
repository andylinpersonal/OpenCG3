#include "StringParser.hpp"

using namespace OpenCG3;
using namespace std;

const unordered_map<char, char>
StringParser::BRACKETS = {
	{ '(', ')' },
	{ '[', ']' },
	{ '{', '}' },
	{ '<', '>' }
};

const string
StringParser::PTN_INVALID = "Invalid";
const string
StringParser::PTN_EMPTY_TREE = "<empty>";


deque<StringParser::ArgTree*>*
StringParser::line_parser(ExtensibleString &line)
{
	cmd_trim_terminal_space(line);
	deque<StringParser::ArgTree*> *treeLst = new deque<StringParser::ArgTree*>();
	list<ExtensibleString> lineLst;
	// check for unquoted semicolon ...
	bool is_quoted = false;
	// for stacks
	stack<char> bracketStack;

	size_t str_len = line.size(), i = 0;
	// this section of substring...
	size_t curr_start = 0;
	while (true)
	{
		// last character in the input line...
		if (i == str_len)
		{
			// if last character is unquoted semicolon, discard it.
			// incomplete quoted string or container will be treated later in lexer.
			// if (line[(i - 1)] != SEMICOLON && !is_quoted && bracketStack.empty())
			//if (line[(i - 1)] != SEMICOLON)
			lineLst.push_back(ExtensibleString(line, curr_start, i - curr_start));
			break;
		}
		// unquoted semicolon is EOL symbol
		else if ((line[i] == SEMICOLON) && !is_quoted && bracketStack.empty())
		{
			lineLst.push_back(ExtensibleString(line, curr_start, i - curr_start));
			++i;
			curr_start = i;
			continue;
		}
		else if (line[i] == QUOTE)
			is_quoted = !is_quoted;
		else if (BRACKETS.count(line[i]) && !is_quoted)
			bracketStack.push(line[i]);
		else if (!bracketStack.empty() && !is_quoted)
		{
			if(line[i] == BRACKETS.at(bracketStack.top()))
				bracketStack.pop();
		}
		++i;
	}
	// if no logical line exist, return NULL...
	if (lineLst.empty())
	{
		delete treeLst;
		return NULL;
	}
	// parse each logical line
	for (ExtensibleString cmd : lineLst)
	{
		treeLst->push_back(new ArgTree());
		arg_lexer(cmd, treeLst->back()->root);
	}
	// parse pattern of each line
	for (ArgTree *cmd : *treeLst)
	{
		cmd->set_pattern(cmd_pattern_parser(cmd));
	}
	return treeLst;
}

StringParser::ArgTree::Node *
StringParser::arg_parser(ExtensibleString const &token)
{
	ExtensibleString arg_edit = ExtensibleString(token);
	cmd_trim_terminal_space(arg_edit);
	if (arg_edit.empty())
		return NULL;
	ArgTree::Node *node = new ArgTree::Node();
	size_t paired_bracket_loc;
	switch (arg_edit[0])
	{
	case '\'':
		// trim back and front quote
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of('\'');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		node->str_val = token.to_string();
		node->type = ArgTree::Type::Str;
		break;
	case '<':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of('>');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->str_val = STR_NULL;
		node->type = ArgTree::Type::Ctnr_Vector;
		break;
	case '{':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of('}');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->str_val = STR_NULL;
		node->type = ArgTree::Type::Ctnr_Set;
		break;
	case '(':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of(')');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->str_val = STR_NULL;
		node->type = ArgTree::Type::Ctnr_Tuple;
		break;
	case '[':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of(']');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->str_val = STR_NULL;
		node->type = ArgTree::Type::Ctnr_Univ;
		break;
	default:
		node->str_val = token.to_string();
		stringstream str;
		if (isNaturalNum(token))
		{
			str << token;
			node->type = ArgTree::Type::Natural;
			str >> node->num_val.n64;
		}
		else if (isReal(token))
		{
			str << token;
			node->type = ArgTree::Type::Real;
			str >> node->num_val.fp64;
		}
		else if (isValidRawStr(token))
			node->type = ArgTree::Type::Str;
		else
			node->type = ArgTree::Type::Invalid;
		break;
	}
	return node;
}

bool
StringParser::arg_lexer(ExtensibleString const &cmd, ArgTree::Node *parent)
{
	bool is_quoted = false, is_space = false;
	stack<char> bracketStack;
	// cache current argument
	size_t curr_arg_start = 0;

	auto flush_arg = [&](size_t &i) {
		auto out = arg_parser(ExtensibleString(cmd, curr_arg_start, i - curr_arg_start));
		if (out) parent->child.push_back(out);
		curr_arg_start = i;
	};
	
	{
		size_t i = 0, sz = cmd.size();
		while (true)
		{
			// if this char is end of command, flush it & quit...
			if (i >= sz)
			{
				if (!is_quoted && bracketStack.empty())
					flush_arg(i);
				// handle incomplete input
				else
				{
					cerr << "error:    unbalanced quotation marks or brackets" << endl;
					parent->type = ArgTree::Type::Invalid;
					for (size_t j = 0; j < parent->child.size(); ++j)
					{
						if(parent->child[j]) delete parent->child[j];
					}
					parent->child.clear();
					return false;
				}
				break;
			}
			if (is_space)
			{
				if (StringParser::is_space(cmd[i])) {
					curr_arg_start = ++i;
					continue;
				}
				else
				{
					curr_arg_start = i;
					is_space = false;
				}
			}
			if (is_quoted)
			{
				// xxx(arg0)|(arg1)'[quoted]'
				if (cmd[i] == QUOTE)
				{
					is_quoted = false;
					++i;
					if(bracketStack.empty())
						flush_arg(i);
					continue;
				}
			}
			else if (!is_quoted)
			{
				// (arg0)|[space]|(arg1)
				if (StringParser::is_space(cmd[i]) && bracketStack.empty())
				{
					flush_arg(i);
					is_space = true;
				}
				// '[quoted]'(arg0)|(arg1)
				else if (cmd[i] == QUOTE)
				{
					is_quoted = true;
				}
				else if (BRACKETS.count(cmd[i]) && !is_quoted)
				{
					if(bracketStack.empty())
						flush_arg(i);
					bracketStack.push(cmd[i]);
				}
				else if (!bracketStack.empty())
				{
					// if ((123)(|(error: seperating the argument...)|111))
					if (cmd[i] == BRACKETS.at(bracketStack.top()) && !is_quoted)
					{
						// [enclosed]>(arg0)|(arg1)
						bracketStack.pop();
						++i;
						if (bracketStack.empty())
							flush_arg(i);
						continue;
					}
				}
			}
			++i;
		}
	}
	return true;
}

bool
StringParser::cmd_trim_terminal_space(ExtensibleString &cmd)
{
	
	if(!cmd.size()) return false;
	for (size_t i = 0; i < cmd.size();)
	{
		if (is_space(cmd[0]))
		{
			cmd.erase(0, 1);
			continue;
		}
		else
			break;
	}

	if (!cmd.size()) return false;
	for (;;)
	{
		if (is_space(cmd[cmd.size() - 1]))
		{
			cmd.erase(cmd.size() - 1, 1);
			continue;
		}
		else
			break;
	}
    return true;
}

string
StringParser::cmd_pattern_parser(ArgTree *const cmd)
{
	return cmd_pattern_parser_rec(cmd->root);
}

string
StringParser::cmd_pattern_parser_rec(ArgTree::Node *const arg)
{
	string out = STR_NULL, tmp = STR_NULL;
	deque<string> bfr = deque<string>();
	size_t sz_child = arg->child.size();
	if (sz_child)
	{
		for (size_t i = 0; i < sz_child; ++i)
		{
			tmp = cmd_pattern_parser_rec(arg->at(i));
			// if any item in this argument is INVALID, this command will also be 
			// INVALID.
			if (tmp == PTN_INVALID) return PTN_INVALID;
			bfr.push_back(string(tmp));
		}
		// [] is universal container : can have different type of child
		if (arg->type == ArgTree::Type::Ctnr_Univ)
		{
			out.append(1, '[');
			for (string item : bfr) out.append(item);
			out.append(1, ']');
			return out;
		}
		// root node is also universal container : can have different type of child
		else if(arg->type == ArgTree::Type::Ctnr_Root)
		{
			for (string item : bfr) out.append(item);
			return out;
		}
		else
		{
			bool exist_N = false, exist_R = false, exist_S = false, exist_Other = false;
			// promotion N -> R -> S
			for (size_t i = 0; i < sz_child; ++i)
			{
				if (bfr[i] == "N")
					exist_N = true;
				else if (bfr[i] == "R")
					exist_R = true;
				else if (bfr[i] == "S")
					exist_S = true;
				else
					exist_Other = true;
			}
			{
				auto end_type = [&](ArgTree::Type t) -> string {
					switch (t)
					{
					case OpenCG3::StringParser::ArgTree::Ctnr_Set:
						return "}";
					case OpenCG3::StringParser::ArgTree::Ctnr_Tuple:
						return ")";
					case OpenCG3::StringParser::ArgTree::Ctnr_Vector:
						return ">";
					default:
						// never go here...
#ifdef _MSC_VER
						__debugbreak();
#else  // for non windows
						asm ( "int3" );
#endif // _MSC_VER
						return PTN_INVALID;
					}
				};
				if (exist_Other)
				{
					size_t cnt = sz_child - 1;
					for (size_t i = 0; i < cnt; ++i)
					{
						if (bfr[i] != bfr[i + 1])
							return PTN_INVALID;
					}
					out.append(bfr[0]);
					out.append(1, ':');
					out.append(to_string(sz_child));
					out.append(end_type(arg->type));
					return out;
				}
				else if (exist_S)
				{
					out = "S:";
					// std::to_string(T number) -> convert number to 
					// corresponding string representation.
					out.append(to_string(sz_child));
					out.append(end_type(arg->type));
					return out;
				}
				else if (exist_R)
				{
					out = "R:";
					out.append(to_string(sz_child));
					out.append(end_type(arg->type));
					return out;
				}
				else if (exist_N)
				{
					out = "N:";
					out.append(to_string(sz_child));
					out.append(end_type(arg->type));
					return out;
				}
			}
		}
	}
	// is leaf node...
	else
	{
		switch (arg->type)
		{
		case ArgTree::Ctnr_Root:
			return PTN_EMPTY_TREE;
		case ArgTree::Type::Ctnr_Univ:
			return "[]";
		case ArgTree::Type::Ctnr_Set:
			return "}";
		case ArgTree::Type::Ctnr_Tuple:
			return ")";
		case ArgTree::Type::Ctnr_Vector:
			return ">";
		case ArgTree::Type::Natural:
			return "N";
		case ArgTree::Type::Real:
			return "R";
		case ArgTree::Type::Str:
			return "S";
		default:
			return PTN_INVALID;
		}
	}
	// Never goes here
	cerr << "Error:\n Unexpected status:\n  Reach the end of function " <<
		"\"StringParser::cmd_pattern_parser_rec(ArgTree::Node *const arg)" <<
		" -> string\"" << endl;

#ifdef _MSC_VER
	__debugbreak();
#else  // for non windows
	asm("int3");
#endif // _MSC_VER

	return PTN_INVALID;
}

bool
StringParser::isValidRawStr(ExtensibleString const &bfr)
{
	stringstream tmpbfr;
	tmpbfr >> std::noskipws;
	tmpbfr << bfr;
	// current character cache
	uint8_t ch;
	while (!tmpbfr.eof())
	{
		tmpbfr >> ch;
		if (!is_raw_string(ch))
			return false;
	}
	return true;
}

bool
StringParser::isReal(ExtensibleString const &bfr)
{
	stringstream tmpbfr;
	tmpbfr >> std::noskipws;
	tmpbfr << bfr;
	// current character cache
	uint8_t ch;
	// contain digit
	bool contain_digit = false;
	NUM_PSR_STAT prev_char_type = start;
	do
	{
		if (bfr == XSTR_NULL) break;
		tmpbfr >> ch;
		switch (prev_char_type)
		{
		case OpenCG3::StringParser::start:
			if (is_sign(ch))
				prev_char_type = sign;
			else if (isdigit(ch))
				prev_char_type = digit;
			else if (is_dot(ch))
				prev_char_type = dot;
			else return false;
			break;
		case OpenCG3::StringParser::sign:
			if (isdigit(ch))
				prev_char_type = digit;
			else if (is_dot(ch))
				prev_char_type = dot;
			else return false;
			break;
		case OpenCG3::StringParser::digit:
			contain_digit = true;
			if (is_dot(ch))
				prev_char_type = dot;
			else if (!isdigit(ch))
				return false;
			break;
		case OpenCG3::StringParser::dot:
			if (isdigit(ch))
				prev_char_type = digit_decimal;
			else return false;
			break;
		case OpenCG3::StringParser::digit_decimal:
			contain_digit = true;
			if (!isdigit(ch))
				return false;
			break;
		default:
			return false;
		}

	} while (!tmpbfr.eof());
	// e.g. "   "(all blank), " +." , "." , "+" (no digit)
	if ((prev_char_type == sign) || (prev_char_type == dot) || !contain_digit)
		return false;
	return true;
}

bool
StringParser::isNaturalNum(ExtensibleString const &bfr)
{
	stringstream tmpbfr;
	tmpbfr >> std::noskipws;
	tmpbfr << bfr;
	// previous character type
	NUM_PSR_STAT prev_char_type = start;
	// current character cache
	uint8_t ch;
	do
	{
		if (bfr == XSTR_NULL) break;
		tmpbfr >> ch;
		// after parsing complete, any appearance of non-blank character
		// is invalid.
		switch (prev_char_type)
		{
		case StringParser::start:
			if (isdigit(ch))
				prev_char_type = digit;
			else return false;
			break;
		case StringParser::digit:
			if (!isdigit(ch))
				return false;
			else
				prev_char_type = digit;
			break;
		default:
			return false;
		}
	} while (!tmpbfr.eof());
	if (prev_char_type == start) return false;
	return true;
}

bool
StringParser::is_space(char const ch)
{
	return (ch == ' ') || (ch == '\t') ? true : false;
}

bool
StringParser::is_sign(char const ch)
{
	return (ch == '+') || (ch == '-') ? true : false;
}

bool
StringParser::is_dot(char const ch)
{
	return ch == '.' ? true : false;
}

bool
StringParser::is_raw_string(char const ch)
{
	if (!(isalnum(ch) ||
		is_dot(ch) ||
		is_sign(ch)))
		return false;
	return true;
}

/// class OpenCG3::StringParser::ExtensibleString
const size_t
StringParser::ExtensibleString::pos_eof = ULLONG_MAX;
const size_t
StringParser::ExtensibleString::pos_begin = 0;
const char
StringParser::ExtensibleString::eof = EOF;

// ctors
StringParser::ExtensibleString
StringParser::operator""_xs(const char * in, size_t sz)
{
	ExtensibleString out = ExtensibleString(sz);
	for (size_t i = 0; i < sz; ++i)
	{
		out.val[i] = in[i];
	}
	return out;
}

StringParser::ExtensibleString::ExtensibleString(size_t sz)
	: val(deque<char>(sz)){ }

StringParser::ExtensibleString::ExtensibleString(string const &in)
{
	this->val.clear();
	this->val = deque<char>(in.begin(), in.end());
}

StringParser::ExtensibleString::ExtensibleString(ExtensibleString const &to_copy)
{
	this->val = deque<char>(to_copy.val);
}

StringParser::ExtensibleString::ExtensibleString(ExtensibleString const & src, size_t begin, size_t end)
	:val(end >= begin ? (end - begin) : 0)
{
	this->val.assign(
		src.val.begin() + begin,
		src.val.begin() + begin + end
	);
}

istream &
StringParser::operator >> (istream &is, ExtensibleString &target)
{
	is >> skipws;
	char tmp;
	target.val.clear();
	while (!is.eof())
	{
		is >> tmp;
		target.val.push_back(tmp);
	}
	return is;
}