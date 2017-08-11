#include "../StringParser.hpp"
#include "../StringParser_ArgTree.hpp"

#include <regex>

using namespace OpenCG3::StringParser;
using namespace std;

int main(void)
{
	const regex pat_str = regex("'(.*?)'|[.0-9A-Za-z+\-]+");
	string lineBfr;
	vector<string> pattern;
	char ch;
	bool is_escape = false, is_invalid_line = false;
	while ((ch = getchar()) != EOF)
	{
		if (ch == '\n')
		{
			// "\\\n" disable newline
			if (is_invalid_line)
			{
				is_invalid_line = false;
				continue;
			}
			if (is_escape)
			{
				is_escape = false;
				continue;
			}
			/*
			cout << "input:[" << lineBfr << "]\n pattern:" << endl;
			cout << boolalpha;
			cout << " Is natural: " << isNaturalNum(lineBfr) << endl;
			cout << boolalpha;
			cout << " Is real: " << isReal(lineBfr) << endl;
			cout << boolalpha;
			cout << " Is valid string: " << isValidRawStr(lineBfr) << endl;
			
			
			pattern.clear();
			pattern = StrParser(bfr);
			for (size_t i = 0; i < pattern.size(); ++i)
				cout << " arg" << i << ":[" << pattern[i] << "]" << endl;
			*/
			sregex_iterator begin = sregex_iterator(lineBfr.begin(), lineBfr.end(), pat_str);
			sregex_iterator w_end = sregex_iterator();
			size_t id = 0;
			deque<ArgTree *> *out = line_parser(lineBfr);
			cout << "Export pattern:" << endl;
			for (size_t i = 0; i < out->size() ; ++i)
			{
				cout << " Pattern " << i << " :" << cmd_pattern_parser(out->at(i)) << endl;
				delete out->at(i);
			}
			cout << endl;
			delete out;
			/*
			for (sregex_iterator i = begin; i != w_end; ++i, ++id) {
				smatch matched_id = *i;
				cout << " valid term " << id << ": " << matched_id.str() << endl;
			}
			*/
			lineBfr.clear();
		}
		else
		{
			if (is_invalid_line)
				continue;

			if (ch == '\\') {
				is_escape = true;
				continue;
			}
			if (is_escape)
			{
				switch (ch)
				{
				// "\\\\" -> append a '\\' character on string.
				case '\\':
					lineBfr.append(1, (char)ch);
					break;
				// discard line with invalid escape character.
				default:
					cerr << "Error:\n Invalid escape character: [\\" << ch << "]" << endl;
					lineBfr.clear();
					is_invalid_line = true;
					break;
				}
				is_escape = false;
			}
			else
				lineBfr.append(1, (char)ch);
		}
	}

	return EXIT_SUCCESS;
}