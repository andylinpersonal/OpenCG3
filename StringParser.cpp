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

deque<StringParser::ArgTree*>* 
StringParser::line_parser(string &line)
{
	cmd_trim_terminal_space(line);
	deque<StringParser::ArgTree*> *treeLst = new deque<StringParser::ArgTree*>();
	list<string> lineLst;
	// check for unquoted semicolon ...
	bool is_quoted = false;
	// for stacks
	stack<char> bracketStack;

	size_t str_len = line.size(), i = 0;
	// this section of substring...
	size_t curr_start = 0;
	string strbfr;
	while (true)
	{
		// last character in the input line...
		if (i == str_len)
		{
			// if last character is unquoted semicolon, discard it.
			if (!((line[i] == SEMICOLON) && !is_quoted && bracketStack.empty()))
				++i;
			lineLst.push_back(string(line, curr_start, i - curr_start));
			break;
		}
		// unquoted semicolon is EOL symbol
		else if ((line[i] == SEMICOLON) && !is_quoted && bracketStack.empty())
		{
			lineLst.push_back(string(line, curr_start, i - curr_start));
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
	
	// remove empty logical line
	{
		list<string>::iterator it = lineLst.begin();
		while (it != lineLst.end())
		{
			cmd_trim_terminal_space(*it);
			if (*it == STR_NULL)
			{
				it = lineLst.erase(it);
				continue;
			}
			if (it != lineLst.end())
				++it;
			else
				break;
		}
	}
	// if no logical line exist, return NULL...
	if (lineLst.empty())
	{
		delete treeLst;
		return NULL;
	}
	// parse each logical line
	for (string cmd : lineLst)
	{
		treeLst->push_back(new ArgTree());
		arg_lexer(cmd, treeLst->back()->root);
	}
	return treeLst;
}

StringParser::ArgTree::Node *
StringParser::arg_parser(string const &token)
{
	string arg_edit = string(token);
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
		node->str_val = string(arg_edit);
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
		node->str_val = string(token);
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
StringParser::arg_lexer(string const &cmd, ArgTree::Node *parent)
{
	bool is_quoted = false, is_space = false;
	stack<char> bracketStack;
	// cache current argument
	size_t curr_arg_start = 0;

	auto flush_arg = [&](size_t &i) {
		auto out = arg_parser(string(cmd, curr_arg_start, i - curr_arg_start));
		if (out) parent->child.push_back(out);
		curr_arg_start = i;
	};
	
	{
		size_t i = 0, sz = cmd.size();
		while (true)
		{
			// if this char is end of command, flush it & quit...
			if (i == sz)
			{
				flush_arg(i);
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
					curr_arg_start = i;
				}
				// '[quoted]'(arg0)|(arg1)
				else if (cmd[i] == QUOTE)
				{
					is_quoted = true;
					++i;
				}
				else if (BRACKETS.count(cmd[i]) && !is_quoted)
				{
					// Just get into enclosed section, flush the argument;
					// xxx(arg0)|(arg1)<[enclosed]...
					//if (bracketStack.size() == 1)
					{
						//FLUSH_ARG();
					}
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
StringParser::cmd_trim_terminal_space(string &cmd)
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
	string out = STR_NULL;
	
	return out;
}

string OpenCG3::StringParser::cmd_pattern_parser_rec(ArgTree::Node *const arg)
{
	string out = STR_NULL;

	return out;
}

bool
StringParser::isValidRawStr(string const &bfr)
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
StringParser::isReal(string const &bfr)
{
	stringstream tmpbfr;
	tmpbfr >> std::noskipws;
	tmpbfr << bfr;
	// current character cache
	uint8_t ch;
	// contain digit
	bool contain_digit = false;
	num_parser_status prev_char_type = start;
	do
	{
		if (bfr == "") break;
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
StringParser::isNaturalNum(string const &bfr)
{
	stringstream tmpbfr;
	tmpbfr >> std::noskipws;
	tmpbfr << bfr;
	// previous character type
	num_parser_status prev_char_type = start;
	// current character cache
	uint8_t ch;
	do
	{
		if (bfr == "") break;
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
