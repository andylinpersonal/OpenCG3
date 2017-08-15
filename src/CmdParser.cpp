#include "CmdParser.hpp"

using namespace std;
using namespace OpenCG3;
using namespace OpenCG3::CmdParser;

/// constants
const string
CmdParser::OP_NAME[4] = {"", "create", "delete", "assign"};

const string
CmdParser::OBJ_NAME[4] = {"", "window", "point", "instr"};



const map<string, int>
CmdParser::OP_ID = {{OP_NAME[0], 0} ,{OP_NAME[1], 1}, {OP_NAME[2], 2}, {OP_NAME[3], 3}};

const map<string, int>
CmdParser::OBJ_ID = {{OBJ_NAME[0], 0} ,{OBJ_NAME[1], 1}, {OBJ_NAME[2], 2}, {OBJ_NAME[3], 3}};



/// class Command
CmdParser::Command::Command() {}

CmdParser::Command::Command(CmdParser::Command const &in)
	:opcode(in.opcode),
	object(in.object),
	param(in.param) {}

void
CmdParser::Command::swap(Command &target)
{
	auto tmp = target.param;
	target.param = this->param;
	this->param = tmp;
}
