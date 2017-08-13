#ifndef OPENCG3_CMDPARSER_H
#define OPENCG3_CMDPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
using namespace std;

#include <gtkmm.h>

#include "IObject.hpp"
#include "StringParser.hpp"

namespace OpenCG3 {
	namespace CmdParser{

		class Command {
		public:
			bool isValid;
			int opcode;
			int object;
			string type;
			vector<string> param;

			Command();
			Command(OpenCG3::CmdParser::Command const&);
			

		};
		/// const
		// defines
#define OP_ID_CREATE 1
#define OP_ID_DELETE 2
#define OP_ID_ASSIGN 3

#define OBJ_ID_WINDOW 1
#define OBJ_ID_POINT  2
#define OBJ_ID_INSTR  3

		// Operation and object name
		extern const string OP_NAME[4];
		extern const string OBJ_NAME[4];
		// Operation and object ID
		extern const map<string, int> OP_ID;
		extern const map<string, int> OBJ_ID;
		extern const string TYPE_STR[4][4];

		/// Generate command item
		Command Parser(string &raw);


	}

}

#endif // OPENCG3_CMDPARSER_H
