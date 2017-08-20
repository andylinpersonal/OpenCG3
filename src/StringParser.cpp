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
		// get line number info
		string phy = string(cmd.begin(), cmd.begin() + cmd.find_first_of(SPACE) + 1);
		cmd.erase(0, cmd.find_first_of(SPACE) + 1);
		string logical = string(cmd.begin(), cmd.begin() + cmd.find_first_of(SPACE) + 1);
		cmd.erase(0, cmd.find_first_of(SPACE) + 1);

		stringstream bfr;
		bfr << phy;
		bfr >> treeLst->back()->physical_line_no();
		bfr.clear();
		bfr << logical;
		bfr >> treeLst->back()->logical_line_no();

		cmd_trim_terminal_space(cmd);
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
		node->_Str_val = token.to_string();
		node->_Node_Type = ArgTree::NodeType::Str;
		break;
	case '<':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of('>');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->_Str_val = STR_NULL;
		node->_Node_Type = ArgTree::NodeType::Ctnr_Vector;
		break;
	case '{':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of('}');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->_Str_val = STR_NULL;
		node->_Node_Type = ArgTree::NodeType::Ctnr_Set;
		break;
	case '(':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of(')');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->_Str_val = STR_NULL;
		node->_Node_Type = ArgTree::NodeType::Ctnr_Tuple;
		break;
	case '[':
		arg_edit.erase(0, 1);
		paired_bracket_loc = arg_edit.find_last_of(']');
		if (paired_bracket_loc < arg_edit.size()) arg_edit.erase(paired_bracket_loc, 1);
		arg_lexer(arg_edit, node);
		node->_Str_val = STR_NULL;
		node->_Node_Type = ArgTree::NodeType::Ctnr_Univ;
		break;
	default:
		node->_Str_val = token.to_string();
		stringstream str;
		if (isNaturalNum(token))
		{
			str << token;
			node->_Node_Type = ArgTree::NodeType::Natural;
			str >> node->_Num_val.i64;
		}
		else if (isReal(token))
		{
			str << token;
			node->_Node_Type = ArgTree::NodeType::Real;
			str >> node->_Num_val.fp64;
		}
		else if (isValidRawStr(token))
			node->_Node_Type = ArgTree::NodeType::Str;
		else
			node->_Node_Type = ArgTree::NodeType::Invalid_Node;
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
					parent->_Node_Type = ArgTree::NodeType::Invalid_Node;
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

void
StringParser::cmd_get_op_obj_type(ArgTree & in)
{
	string const &op = in[0]->get_str_val();
	string const &obj = in[1]->get_str_val();
	
	// get op
	if (op == "create")
		in.set_opcode_type(ArgTree::Op_t::Create);
	else if (op == "attach")
		in.set_opcode_type(ArgTree::Op_t::Attach);
	else if (op == "select")
		in.set_opcode_type(ArgTree::Op_t::Select);
	else if (op == "remove")
		in.set_opcode_type(ArgTree::Op_t::Remove);
	else if (op == "detach")
		in.set_opcode_type(ArgTree::Op_t::Detach);
	else if (op == "delete")
		in.set_opcode_type(ArgTree::Op_t::Delete);
	else
		in.set_opcode_type(ArgTree::Op_t::Undefined_Obj);
	// get obj
	if (obj == "attrib")
		in.set_object_type(ArgTree::Obj_t::Attrib);
	else if (obj == "point")
		in.set_object_type(ArgTree::Obj_t::Point);
	else if (obj == "line")
		in.set_object_type(ArgTree::Obj_t::Line);
	else if (obj == "camera")
		in.set_object_type(ArgTree::Obj_t::Camera);
	else if (obj == "window")
		in.set_object_type(ArgTree::Obj_t::Window);
	else
		in.set_object_type(ArgTree::Obj_t::Undefined);
	
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
		if (arg->_Node_Type == ArgTree::NodeType::Ctnr_Univ)
		{
			out.append(1, '[');
			for (string item : bfr) out.append(item);
			out.append(1, ']');
			return out;
		}
		// root node is also universal container : can have different type of child
		else if(arg->_Node_Type == ArgTree::NodeType::Ctnr_Root)
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
				auto end_type = [&](ArgTree::NodeType t) -> string {
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
					out.append(end_type(arg->_Node_Type));
					return out;
				}
				else if (exist_S)
				{
					out = "S:";
					// std::to_string(T number) -> convert number to 
					// corresponding string representation.
					out.append(to_string(sz_child));
					out.append(end_type(arg->_Node_Type));
					return out;
				}
				else if (exist_R)
				{
					out = "R:";
					out.append(to_string(sz_child));
					out.append(end_type(arg->_Node_Type));
					return out;
				}
				else if (exist_N)
				{
					out = "N:";
					out.append(to_string(sz_child));
					out.append(end_type(arg->_Node_Type));
					return out;
				}
			}
		}
	}
	// is leaf node...
	else
	{
		switch (arg->_Node_Type)
		{
		case ArgTree::Ctnr_Root:
			return PTN_EMPTY_TREE;
		case ArgTree::NodeType::Ctnr_Univ:
			return "[]";
		case ArgTree::NodeType::Ctnr_Set:
			return "}";
		case ArgTree::NodeType::Ctnr_Tuple:
			return ")";
		case ArgTree::NodeType::Ctnr_Vector:
			return ">";
		case ArgTree::NodeType::Natural:
			return "N";
		case ArgTree::NodeType::Real:
			return "R";
		case ArgTree::NodeType::Str:
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
