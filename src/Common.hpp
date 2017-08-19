/**
*   define some common symbols
*/

#pragma once
#ifndef OPENCG3_COMMON_HEADER
#define OPENCG3_COMMON_HEADER

#include <mutex>
#define AUTOLOCK(MutexType, to_lock)                                          \
{   lock_guard<MutexType> lock(to_lock);

#define AUTOUNLOCK                                                            \
}

#define DEBUG_TRACEBACK                                                       \
	fprintf(stderr, "  at file %s, line %s,\n    in function %s\n",           \
		__FILE__, __LINE__, __FUNCTION__)
#define DEBUG_TRACEBACK_PRINT(msg)                                            \
	fprintf(stderr, "  at file %s, line %s,\n    in function %s\n"            \
		"    message: %s\n",__FILE__, __LINE__, __FUNCTION__, msg)

#include <string>
#include <deque>
#include <iterator>
using namespace std;

namespace OpenCG3 {

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

		inline string to_string() const
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

		inline ExtensibleString &
			operator+=(ExtensibleString const &in)
		{
			this->val.insert(this->val.end(), in.val.begin(), in.val.end());
			return *this;
		}

		inline ExtensibleString
			operator+(ExtensibleString const &in) const
		{
			ExtensibleString out = ExtensibleString(*this);
			out += in;
			return out;
		}

		inline ExtensibleString &
			operator=(ExtensibleString const &to_assign)
		{
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
}
#endif // !OPENCG3_COMMON_HEADER
