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
CmdParser::Command::Command()
	:isValid(false) {}

CmdParser::Command::Command(OpenCG3::CmdParser::Command const &in)
	: isValid(in.isValid),
	opcode(in.opcode),
	object(in.object),
	param(vector<string>(in.param.begin(), in.param.end())) {}

CmdParser::Command
CmdParser::Parser(string &raw)
{
	Command result = Command();
	// using stringstream to profile string
	stringstream tmpbfr;
	tmpbfr << raw;

	string op, obj;
	tmpbfr >> op >> obj;

	// if OP or OBJ is not found, return default command object (isValid = false)
	if (!CmdParser::OP_ID.count(op) || !CmdParser::OBJ_ID.count(obj))
		return result;

	result.object = CmdParser::OBJ_ID.at(obj);
	result.opcode = CmdParser::OP_ID.at(op);
	/// Profile command
	// any invalid input will yield default Command object.
	switch (result.opcode)
	{
	case OP_ID_CREATE:
		switch (result.object)
		{
		case OBJ_ID_WINDOW:
			// create window <str>title <Point>window_centre <tuple<Point, 3>>(vw, vh, vc)
			result.param.push_back(string());
			result.param.push_back(string());
			result.param.push_back(string());
		case OBJ_ID_POINT:
			// create point <str>label <Point>point
			result.param.push_back(string());
			result.param.push_back(string());
			tmpbfr >> result.param.at(0);
			getline(tmpbfr, result.param.at(1));
			return result;
		default:
			return Command();
		}
	case OP_ID_DELETE:
		switch (result.object)
		{
		case OBJ_ID_WINDOW:
			// delete window <str>quit_string
			result.param.push_back(string());
			getline(tmpbfr, result.param.at(0));
			return result;
		case OBJ_ID_POINT:
			// delete point

		default:
			return Command();
		}
	case OP_ID_ASSIGN:
		switch (result.object)
		{
		case OBJ_ID_INSTR:
		default:
			return Command();
		}
	default:
		return Command();
	}
}

