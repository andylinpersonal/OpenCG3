#pragma once
#ifndef OPENCG3_STRING_PARSER_HEADER
#define OPENCG3_STRING_PARSER_HEADER

#include <unordered_map>
#include <deque>
#include <list>
#include <string>

#include <iostream>
#include <sstream>

#include <iterator>

#include <cstdint>
using namespace std;

#include "StringParser_ArgTree.hpp"

namespace OpenCG3 {

	namespace StringParser {

#define QUOTE        '\''
#define SEMICOLON    ';'
		/// Bracket relationship...
		extern const unordered_map<char, char> BRACKETS;
		/// Internal use
		enum string_parser_status
		{
			Start = 0, Text = 0x1, Quoted_Text = 0x2, In_Ctnr = 0x4, Undefined = 0x8
		};

		/// split string into logical line, and send them into arg_lexer
		deque<ArgTree *> *line_parser(string &line);
		/// for recursively generate tree...
		//  determine type of each argument; write them into new node,
		//  or call arg_lexer recursively if arg have container type...
		//  => pointer to subtree (action failed => NULL pointer)
		ArgTree::Node *arg_parser(string const& token);
		//  split logical line into tokens (arg) => action successful?
		bool           arg_lexer(string const& logical_line, ArgTree::Node *parent = NULL);
		/// remove trailing / fronting space characters.
        inline bool cmd_trim_terminal_space(string &);

		/// parse pattern of tree content
		extern const string PTN_INVALID;
		string cmd_pattern_parser(ArgTree *const cmd);
		//  for parsing the pattern string recursively ...
		string cmd_pattern_parser_rec(ArgTree::Node *const arg);

		enum num_parser_status
		{
			start, sign, digit, dot, digit_decimal, blank, closed
		};
		/// Pattern: [0-9a-zA-Z+\-]
		bool isValidRawStr(string const&);
		/// Pattern: [0-9]+
		bool isReal(string const&);
		/// Pattern: [+\-]?([0-9]*[.])?[0-9]+
		bool isNaturalNum(string const&);
		/// Auxiliary small function
		inline bool is_space(char const);
		inline bool is_sign(char const);
		inline bool is_dot(char const);
		/// is a valid non-singly-quoted string
		inline bool is_raw_string(char const);
	}
}
#endif // !OPENCG3_STRING_PARSER_HEADER
