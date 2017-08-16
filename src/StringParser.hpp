#pragma once
#ifndef OPENCG3_STRING_PARSER_HEADER
#define OPENCG3_STRING_PARSER_HEADER

#include <unordered_map>
#include <deque>
#include <list>
#include <string>

#include <iostream>
#include <sstream>

#include <algorithm>
#include <iterator>

#include <cstdint>
#include <cstdio>
using namespace std;

#include "StringParser_ArgTree.hpp"

namespace OpenCG3 {

	namespace StringParser {

		/// Command parser functions
#define QUOTE        '\''
#define SEMICOLON    ';'
#define SPACE        ' '
		/// Bracket relationship...
		extern const unordered_map<char, char> BRACKETS;
		/// Internal use
		enum STR_PSR_STAT
		{
			Start = 0, Text = 0x1, Quoted_Text = 0x2, In_Ctnr = 0x4, Undefined = 0x8
		};
		typedef deque<char> flex_str_t;
		/// split string into logical line, and send them into arg_lexer
		deque<ArgTree *> *line_parser(ExtensibleString &line);
		/// for recursively generate tree...
		//  determine type of each argument; write them into new node,
		//  or call arg_lexer recursively if arg have container type...
		//  => pointer to subtree (action failed => NULL pointer)
		ArgTree::Node *   arg_parser(ExtensibleString const& token);
		//  split logical line into tokens (arg) => action successful?
		bool              arg_lexer(ExtensibleString const& logical_line, ArgTree::Node *parent = NULL);
		/// remove trailing / fronting space characters.
		inline bool       cmd_trim_terminal_space(ExtensibleString &);

		/// parse pattern of tree content
		extern const string PTN_INVALID;
		extern const string PTN_EMPTY_TREE;
		string cmd_pattern_parser(ArgTree *const cmd);
		//  for parsing the pattern string recursively ...
		string cmd_pattern_parser_rec(ArgTree::Node *const arg);

		enum NUM_PSR_STAT
		{
			start, sign, digit, dot, digit_decimal, blank, closed
		};
		/// Pattern: [0-9a-zA-Z+\-]
		bool isValidRawStr(ExtensibleString const&);
		/// Pattern: [0-9]+
		bool isReal(ExtensibleString const&);
		/// Pattern: [+\-]?([0-9]*[.])?[0-9]+
		bool isNaturalNum(ExtensibleString const&);
		/// Auxiliary small function
		inline bool is_space(char const);
		inline bool is_sign(char const);
		inline bool is_dot(char const);
		/// is a valid non-singly-quoted string
		inline bool is_raw_string(char const);
	}
}
#endif // !OPENCG3_STRING_PARSER_HEADER