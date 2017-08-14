#include "../../src/StringParser.hpp"

using namespace std;
using namespace OpenCG3::StringParser;
int main(void)
{
    EditableString udl = "Usr Dfn Literal"_estr;
	EditableString test = EditableString();
	string str_test = "Hello";
	string str_ad0 = "Appended";
	size_t pos_a = udl.find_first_of('D');
	EditableString ass = str_test;
	size_t pos_b_e = ass.find_last_of('l');
	ass += udl;
	size_t pos = udl.find_last_of('s', 7);
	pos = udl.find_last_of('r', 7);
	ass = ass + udl + str_test;
	test = EditableString(str_test);
	cout << test.to_string() << endl;
	test.append(str_ad0);
	cout << test.to_string() << endl;
	test = EditableString("New Str");
	cout << test.substring(4, 7) << endl;
	return EXIT_SUCCESS;
}