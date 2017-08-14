#include "../../src/StringParser.hpp"
#include "../../src/StringParser_ArgTree.hpp"


using namespace OpenCG3::StringParser;
using namespace std;

int main(void)
{
	ExtensibleString lineBfr;
	char ch;
	// flags for escape sequence parsing
	bool is_escape_char = false, is_invalid_line = false;

	// for valid semicolon resolution
	stack<char> bracketStack;
	bool is_quoted = false;

	size_t physical_line = 1, logical_line = 1;

	// add line number tag for first input line...
	lineBfr.append(to_string(physical_line) + " " + to_string(logical_line) + " ");

	while ((ch = getchar()) != EOF)
	{
		if (ch == '\n')
		{
			++physical_line;
			// "\\\n" 使此次換行失效
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
			++logical_line;
			size_t id = 0;
			deque<ArgTree *> *out = line_parser(lineBfr);
			for (size_t i = 0; i < out->size() ; ++i)
			{
				cout << (*out)[i]->get_physical_line_no() << ":" << 
					(*out)[i]->get_logical_line_no() << " " << 
					(*out)[i]->get_pattern() << endl;
				delete out->at(i);
			}
			delete out;
			lineBfr.clear();
			lineBfr.append(to_string(physical_line) + " " + to_string(logical_line) + " ");
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
			{
				lineBfr.append(ch);
				// processing line number
				if (is_quoted && ch == QUOTE)
				{
					is_quoted = false;
				}
				else
				{
					if (ch == QUOTE)
					{
						is_quoted = true;
					}
					else if (bracketStack.size())
					{
						if (ch == BRACKETS.at(bracketStack.top()))
						{
							bracketStack.pop();
						}
					}
					else if (BRACKETS.count(ch))
					{
						bracketStack.push(ch);
					}
					else if (bracketStack.empty())
					{
						if (ch == SEMICOLON)
						{
							++logical_line;
							lineBfr.append(to_string(physical_line) + " " + to_string(logical_line) + " ");
						}
					}
					
				}
			}
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
