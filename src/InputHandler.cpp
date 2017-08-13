#include "InputHandler.hpp"

using namespace std;
using namespace OpenCG3;

/// Global variable
deque<string>
Input::CmdQueue;
/// TODO: save each command as deque of Command object
deque<OpenCG3::CmdParser::Command>
Input::CommandQueue;

/// TODO: escape sequence "\\\n" "\\\\" should be test carefully
void
Input::StdinHandler(deque<string> &Queue)
{
    string lineBfr;
    int    ch;
    int    assign_cnt = 0;
    CmdParser::Command queueItem;
	bool is_escape = false, is_invalid_line = false;
	
    while(1)
    {
        // loader
		while ((ch = getchar()) != '\n') {
			// discard whole line when invalid escape character occurrs until reach
			// another newline character.
			if (is_invalid_line)
				continue;
			// processing escape character
			if (ch == '\\')
			{
				is_escape = true;
				continue;
			}
			if (!is_escape)
				lineBfr.append(1, ch);
			else
			{
				switch (ch)
				{
				// "\\\\" -> append a '\\' character on string.
				case '\\':
					lineBfr.append(1, ch);
					break;
				default:
					cerr << "Error:\n Invalid escape character: [" << ch << "]" << endl;
					is_invalid_line = true;
					break;
				}
				// jump out when invalid escape character occurrs.
				if (is_invalid_line)
					break;
			}
		}
		// clear line buffer when invalid escape character occurrs until reach
		// another newline character.
		if (is_invalid_line)
		{
			lineBfr.clear();
			is_invalid_line = false;
			continue;
		}
		// "\\\n" disable newline
		if (is_escape)
		{
			is_escape = false;
			continue;
		}

        queueItem = CmdParser::Parser(lineBfr);
        // push latest command to the head of deque
        if(queueItem.isValid)
        {
            Queue.push_front(string(lineBfr));
            //CommandQueue.push_front(Command(queueItem));
        }
        else
            cerr << "Error:\n Invalid command:\n  " << lineBfr << endl;

        lineBfr.clear();
    }
    return;
}

