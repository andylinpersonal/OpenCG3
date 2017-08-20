#include "CmdParser.hpp"

using namespace std;
using namespace OpenCG3;
using namespace OpenCG3::CmdParser;

/// constants
const char*
CmdParser::OP_NAME[5] = {"", "create", "delete", "assign", "attach"};

const char*
CmdParser::OBJ_NAME[6] = {"", "window", "point", "line", "camera", "attrib"};



const unordered_map<string, int>
CmdParser::OP_ID = {{OP_NAME[0], 0} ,{OP_NAME[1], 1}, {OP_NAME[2], 2}, {OP_NAME[3], 3}};

const unordered_map<string, int>
CmdParser::OBJ_ID = {{OBJ_NAME[0], 0} ,{OBJ_NAME[1], 1}, {OBJ_NAME[2], 2}, {OBJ_NAME[3], 3}};



/// class Command
CmdParser::Command::Command()
	:opcode(0),object(0),param(NULL) {}

CmdParser::Command::Command(CmdParser::Command const &in)
	:opcode(in.opcode),
	object(in.object),
	param(in.param) {}

CmdParser::Command::~Command()
{
	if (this->param) delete this->param;
}

void
CmdParser::Command::swap(Command &target)
{
	auto tmp = target.param;
	target.param = this->param;
	this->param = tmp;
}

void
CmdParser::safe_queue_maker(deque<StringParser::ArgTree*> *const raw_arg, deque<CmdParser::Command *> &Queue, mutex &Lock)
{
	// because an input may not include a complete assign command
	static StringParser::ArgTree cached_assign;
	// for assign command processing
	static bool unfinished_assign_op = false;
	static size_t unfinished_assign_count = 0;
	Command *tmp = new Command();
	{
		deque<StringParser::ArgTree *>::iterator it = raw_arg->begin();
		/* expanding assign opname
		 * command format:
		 * assign $opname:S $op:S $class:S $count:N
		 */
		while (it != raw_arg->end())
		{
			if ((*(*it))[0]->str_val == OP_NAME[OP_ID_ASSIGN])
			{
				unfinished_assign_op = true;
				try {
					unfinished_assign_count = ARG_ROOT_ELEMT(*it, 4).get_uint();
					// copy $op and $class into cache
					cached_assign.root->child.push_back(new StringParser::ArgTree::Node(*((*(*it))[2])));
					cached_assign.root->child.push_back(new StringParser::ArgTree::Node(*((*(*it))[3])));
				}
				catch (exception e) {
					DBG_DMP_INVALID_CMD(OP_NAME[OP_ID_ASSIGN], "%s\n", ARG_ROOT_PTR(*it)->to_string());
					fputs(e.what(), stderr);
					DEBUG_TRACEBACK;
				}
			}
			if (!unfinished_assign_count)
			{
				unfinished_assign_op = false;
				cached_assign.clear();
			}
			else if (unfinished_assign_op)
			{
				tmp->param = new StringParser::ArgTree(*(*it));
				for (deque<StringParser::ArgTree::Node *>::reverse_iterator it_cached_op = cached_assign.root->child.rbegin();
					it_cached_op != cached_assign.root->child.rend();
					++it)
				{
					tmp->param->root->child.push_front(new StringParser::ArgTree::Node(*(*it_cached_op)));
				}
				AUTOLOCK(mutex, Lock)
					Queue.push_front(tmp);
				AUTOUNLOCK;
                --unfinished_assign_count;
			}
			else
			{
				
			}
			++it;
		}
	}
	// if there is any unfinished assign op, return for
	// next command input
	if (unfinished_assign_op) return;

	// Expand compound commands
	deque<Command *> cache;
	AUTOLOCK(mutex, Lock)

	AUTOUNLOCK;


}
