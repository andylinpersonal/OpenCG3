#include "../../src/StringParser.hpp"
#include "../../src/StringParser_ArgTree.hpp"


using namespace OpenCG3::StringParser;
using namespace std;

int main(void)
{
	string lineBfr;
	char ch;
	bool is_escape = false, is_invalid_line = false;
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
			if (is_escape)
			{
				is_escape = false;
				continue;
			}
			size_t id = 0;
			deque<ArgTree *> *out = line_parser(lineBfr);
			for (size_t i = 0; i < out->size() ; ++i)
			{
				cout << physical_line << ":" << (physical_line + i) << " " << (*out)[i]->get_pattern() << endl;
				delete out->at(i);
			}
			delete out;
			lineBfr.clear();
		}
		else
		{
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
					lineBfr.append(1, ch);
					break;
				// discard line with invalid escape character.
				default:
					cerr << "warning: unsupported escape sequence: [\\" << ch << "] ignored. please be careful, Mr. Pig." << endl;
					break;
				}
				is_escape = false;
			}
			else
				lineBfr.append(1, ch);
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
