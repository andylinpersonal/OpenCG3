#include "../../src/StringParser.hpp"
#include "../../src/StringParser_ArgTree.hpp"


using namespace OpenCG3::StringParser;
using namespace std;

int main(void)
{
	FlexibleString lineBfr;
	char ch;
	bool is_escape_char = false, is_invalid_line = false;
	stack<char> bracketStack;
	size_t physical_line = 0, logical_line = 0;
	while ((ch = getchar()) != EOF)
	{
		if (ch == '\n')
		{
			++physical_line;
			// "\\\n" disable newline
			if (is_invalid_line)
			{
				is_invalid_line = false;
				continue;
			}
			if (is_escape_char)
			{
				is_escape_char = false;
				continue;
			}
			size_t id = 0;
			deque<ArgTree *> *out = line_parser(lineBfr);
			for (size_t i = 0; i < out->size() ; ++i)
			{
				cout << physical_line << ":" << (physical_line + i) << " " << (*out)[i]->get_pattern() << endl;
				delete out->at(i);
			}
			logical_line += out->size() - 1;
			delete out;
			lineBfr.clear();
		}
		else
		{
			if (ch == '\\') {
				is_escape_char = true;
				continue;
			}
			if (is_escape_char)
			{
				switch (ch)
				{
				// "\\\\" -> append a '\\' character on string.
				case '\\':
					lineBfr.append(1, ch);
					break;
				// discard line with invalid escape character.
				default:
					cerr << "warning: unsupported escape sequence: [\\" << ch << "] ignored. please be careful, Mr. Pig." << endl;
					break;
				}
				is_escape_char = false;
			}
			else
				lineBfr.append(ch);
		}
	}

#ifdef _MSC_VER
	__debugbreak();
#else
 #ifdef DEBUG
	asm("int3");
 #endif // DEBUG

#endif // _MSC_VER
	return EXIT_SUCCESS;
}
