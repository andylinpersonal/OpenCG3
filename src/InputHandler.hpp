#ifndef OPENCG3_INPUT_HANDLER_H
#define OPENCG3_INPUT_HANDLER_H

#include <deque>
#include <vector>

#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

#include "CmdParser.hpp"
#include "StringParser.hpp"

using namespace std;

namespace OpenCG3 {

	namespace Input {

	extern deque<string> CmdQueue;
	extern deque<OpenCG3::CmdParser::Command> CommandQueue;

	void StdinHandler(deque<string> &Queue);
	}
}

#endif // OPENCG3_INPUT_HANDLER_H
