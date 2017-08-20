#include "InputHandler.hpp"

using namespace std;
using namespace OpenCG3;
using namespace OpenCG3::StringParser;

/// Global variable
// mutex for CommandQueue
mutex
Input::mutex_CommandQueue;

// deque for transferring parsed command to main thread.
// push front and pop back
deque<StringParser::ArgTree *>
Input::CommandQueue;

volatile bool
Input::is_alive = true;

void
Input::stdin_handle_worker(deque<StringParser::ArgTree *> &queueLst)
{

	ExtensibleString lineBfr;
	char ch;
	// flags for escape sequence parsing
	bool is_escape_char = false, is_invalid_line = false;

	// for valid semicolon resolution
	stack<char> bracketStack;
	bool is_quoted = false;

	// add line number tag for first input line...
	size_t physical_line = 1, logical_line = 1;
	auto add_line_no = [&]() {
		lineBfr.append(to_string(physical_line) + " " + to_string(logical_line) + " ");
	};
	add_line_no();

	while (is_alive)
	{
		ch = getchar();
		if (ch == '\n')
		{
			++physical_line;
			// if input is "\\\n" , disable this newline (concatenate
			// previous input with following input)
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

			// parsing input ...
			deque<ArgTree *> *raw_cmd = line_parser(lineBfr);
			//  clear empty or invalid commands ...

			for (size_t i = 0; i < raw_cmd->size(); ++i)
			{
#ifdef _DEBUG
				if ((*out)[i]->get_pattern() == PTN_INVALID)
#endif // _DEBUG
				{
					cout << (*raw_cmd)[i]->get_physical_line_no() << ":" <<
						(*raw_cmd)[i]->get_logical_line_no() << " " <<
						(*raw_cmd)[i]->get_pattern() << endl;

				}

				if ((raw_cmd->at(i)->get_pattern() == PTN_EMPTY_TREE) ||
					(raw_cmd->at(i)->get_pattern() == PTN_INVALID))
				{
#ifdef _DEBUG
					cout << " debug:\n  command at line " <<
						out->at(i)->get_physical_line_no() << " is ignored." << endl;
#endif // _DEBUG
					delete raw_cmd->at(i);
					raw_cmd->at(i) = NULL;
				}

				CmdParser::safe_queue_maker(raw_cmd, queueLst, mutex_CommandQueue);

			}

			delete raw_cmd;
			lineBfr.clear();
			add_line_no();
		}
		else
		{
			if ((ch == '\\') && (is_escape_char == false)) {
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
					is_escape_char = false;
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
					is_quoted = false;

				else
				{
					if (ch == QUOTE)
						is_quoted = true;

					else if (bracketStack.size())
					{
						if (ch == BRACKETS.at(bracketStack.top()))
							bracketStack.pop();
					}
					else if (BRACKETS.count(ch))
						bracketStack.push(ch);

					else if (bracketStack.empty())
					{
						if (ch == SEMICOLON)
						{
							++logical_line;
							add_line_no();
						}
					}

				}
			}
		}
	}
	// clear before quit the application
	AUTOLOCK(mutex, mutex_CommandQueue)
		for (CmdParser::Command *cmd : CommandQueue)
		{
			if(cmd) delete cmd;
		}
	AUTOUNLOCK;
    return;
}

