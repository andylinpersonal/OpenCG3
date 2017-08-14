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
		
		/// class ExtensibleString
		class ExtensibleString {
		public:
			const static size_t pos_eof;
			const static size_t pos_begin;
			const static char   eof;

			ExtensibleString(size_t sz = 0);
			ExtensibleString(string const &src);
			ExtensibleString(ExtensibleString const &src);
			ExtensibleString(ExtensibleString const &src, size_t begin, size_t end);

			string to_string() const
			{
				return string(this->val.begin(), this->val.end());
			}

			inline void
				append(string const &str, size_t pos = pos_eof)
			{
				this->val.insert(
					(this->val.begin() + (pos < this->val.size() ? pos : this->val.size())),
					str.begin(), str.end()
				);
			}

			inline void
				append(ExtensibleString const &str, size_t pos = pos_eof)
			{
				this->val.insert(
					(this->val.begin() + (pos < this->val.size() ? pos : this->val.size())),
					str.val.begin(), str.val.end()
				);
			}

			inline void
				append(char const &ch, size_t pos = pos_eof)
			{
				this->val.insert((this->val.begin() + (pos < this->val.size() ? pos : this->val.size())), ch);
			}

			inline string
				substring(size_t idx_begin = 0, size_t idx_end = string::npos) const
			{
				return string(
					(this->val.begin() + (idx_begin <= this->val.size() ? idx_begin : this->val.size())),
					(this->val.begin() + (idx_end <= this->val.size() ? idx_end : this->val.size()))
				);
			}
			/// operators
			friend istream &operator >> (istream &, ExtensibleString &);
			friend ostream &operator<<(ostream &, ExtensibleString const &);

			inline bool
				operator+=(ExtensibleString const &in)
			{
				this->val.insert(this->val.end(), in.val.begin(), in.val.end());
				return true;
			}

			ExtensibleString
				operator+(ExtensibleString const &in) const
			{
				ExtensibleString out = ExtensibleString(*this);
				out += in;
				return out;
			}

			ExtensibleString &
				operator=(ExtensibleString const &to_assign) {
				this->val = deque<char>(to_assign.val);
				return *this;
			}

			inline bool
				operator==(ExtensibleString const &target) const
			{
				return this->val == target.val;
			}

			inline bool
				operator==(string const &target) const
			{
				return *this == ExtensibleString(target);
			}

			inline char &
				operator[](size_t idx) { return this->val.at(idx); }

			inline char
				operator[](size_t idx) const { return this->val.at(idx); }


			/// utility
			inline deque<char>::iterator
				begin(void)
			{
				return this->val.begin();
			}

			inline deque<char>::iterator
				end(void)
			{
				return this->val.end();
			}

			inline void     clear(void) { this->val.clear(); }
			inline size_t   size(void) const { return this->val.size(); }
			inline bool     empty(void) const { return this->val.empty(); }
			// erase range: [off, off + count)
			inline void     erase(size_t off, size_t count = 1)
			{
				this->val.erase(this->val.begin() + off, this->val.begin() + off + count);
			}

			inline size_t   find_last_of(char ch, size_t start_offset = 0) const
			{
				size_t sz = this->val.size();
				for (size_t i = sz - 1; (i > start_offset) && (i < pos_eof); --i)
					if (this->val[i] == ch) return i;

				return pos_eof;
			}
			inline size_t   find_first_of(char ch, size_t start_offset = 0) const
			{
				size_t sz = this->val.size();
				for (size_t i = start_offset; i < sz; ++i)
				{
					if (this->val[i] == ch) return i;
				}
				return pos_eof;
			}
			
			friend ExtensibleString operator "" _xs(const char *in, size_t sz);

		private:
			deque<char> val;
		};

		istream &operator >> (istream &, ExtensibleString &);

		inline ostream &
			operator<<(ostream &os, ExtensibleString const &in)
		{
			for (char c : in.val) os << c;
			return os;
		}

		ExtensibleString operator "" _xs(const char *in, size_t sz);

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