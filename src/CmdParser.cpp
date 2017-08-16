#include "CmdParser.hpp"

using namespace std;
using namespace OpenCG3;
using namespace OpenCG3::CmdParser;

/// constants
const string
CmdParser::OP_NAME[5] = {"", "create", "delete", "assign", "attach"};

const string
CmdParser::OBJ_NAME[3] = {"", "window", "point"};



const map<string, int>
CmdParser::OP_ID = {{OP_NAME[0], 0} ,{OP_NAME[1], 1}, {OP_NAME[2], 2}, {OP_NAME[3], 3}};

const map<string, int>
CmdParser::OBJ_ID = {{OBJ_NAME[0], 0} ,{OBJ_NAME[1], 1}, {OBJ_NAME[2], 2}, {OBJ_NAME[3], 3}};



/// class Command
CmdParser::Command::Command()
	:opcode(0),object(0),param(NULL) {}

CmdParser::Command::Command(CmdParser::Command const &in)
	:opcode(in.opcode),
	object(in.object),
	param(in.param) {}

OpenCG3::CmdParser::Command::~Command()
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
CmdParser::safe_queue_maker(deque<StringParser::ArgTree*> *raw_arg, deque<CmdParser::Command *> &Queue, mutex &Lock)
{
	// because an input may not include a complete assign command
	static deque<StringParser::ArgTree *> cached_cmd;
	// for assign command processing
	static bool unfinished_assign_op = false;
	static size_t unfinished_assign_count = 0;
	{
		deque<StringParser::ArgTree *>::iterator it = raw_arg->begin();
		/* expanding assign opname
		 * command format:
		 * assign $opname:S $op:S $class:S $count:N
		 */
		while (it != raw_arg->end())
		{
			if (ARG_ROOT(*it)[0].str_val == OP_NAME[OP_ID_ASSIGN])
			{
				unfinished_assign_op = true;
				try {
					unfinished_assign_count = ARG_ROOT(*it)[4].num_val.i64;
				}
				catch (exception e) {
					DBG_DMP_INVALID_CMD(OP_NAME[OP_ID_ASSIGN], "%s", );
					fputs(e.what(), stderr);
					DEBUG_TRACEBACK;
				}
			}
			if (!unfinished_assign_count)
				unfinished_assign_op = false;
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
