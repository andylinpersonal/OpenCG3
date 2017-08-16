#include "Common.hpp"

using namespace OpenCG3;
/// class OpenCG3::StringParser::ExtensibleString
const size_t
ExtensibleString::pos_eof = ULLONG_MAX;
const size_t
ExtensibleString::pos_begin = 0;
const char
ExtensibleString::eof = EOF;

// ctors
ExtensibleString
OpenCG3::operator""_xs(const char * in, size_t sz)
{
	ExtensibleString out = ExtensibleString(sz);
	for (size_t i = 0; i < sz; ++i)
	{
		out.val[i] = in[i];
	}
	return out;
}

ExtensibleString::ExtensibleString(size_t sz)
	: val(deque<char>(sz)) { }

ExtensibleString::ExtensibleString(string const &in)
{
	this->val.clear();
	this->val = deque<char>(in.begin(), in.end());
}

ExtensibleString::ExtensibleString(ExtensibleString const &to_copy)
{
	this->val = deque<char>(to_copy.val);
}

ExtensibleString::ExtensibleString(ExtensibleString const & src, size_t begin, size_t end)
	:val(end >= begin ? (end - begin) : 0)
{
	this->val.assign(
		src.val.begin() + begin,
		src.val.begin() + begin + end
	);
}

istream &
OpenCG3::operator >> (istream &is, ExtensibleString &target)
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