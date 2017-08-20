/*
*  Resolving raw command from InputHandler thread,
*  and push processed Command obj to queue for further using.
*/

#ifndef OPENCG3_CMDPARSER_H
#define OPENCG3_CMDPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_map>
using namespace std;

#include "IObject.hpp"
#include "StringParser.hpp"
#include "Common.hpp"

namespace OpenCG3 {
	namespace CmdParser{


#define CMD_ROOT(cmd) ARG_ROOT_PTR((cmd)->param)
#define DBG_DMP_INVALID_CMD(cmd, extra_format, ...)                           \
	fprintf(stderr, "Internal Error:\n\tInvalid Command Format in %s\n"       \
		extra_format, ##__VA_ARGS__);
		/// const
		// defines
#define OP_ID_CREATE 1
#define OP_ID_DELETE 2
#define OP_ID_ASSIGN 3
#define OP_ID_ATTACH 4

#define OBJ_ID_WINDOW 1
#define OBJ_ID_POINT  2
#define OBJ_ID_INSTR  3

		// Operation and object name
		extern const char* OP_NAME[5];
		extern const char* OBJ_NAME[6];
		// Operation and object ID
		extern const unordered_map<string, int> OP_ID;
		extern const unordered_map<string, int> OBJ_ID;

		/*
		*  resolving command and convert to corresponding command object;
		*  append it to command queue for further using by main thread.
		*/
		void safe_queue_maker(
			deque<StringParser::ArgTree *> *const raw_arg,
			deque<StringParser::ArgTree *> &queue,
			mutex &Lock
		);
	}

}

#endif // OPENCG3_CMDPARSER_H
