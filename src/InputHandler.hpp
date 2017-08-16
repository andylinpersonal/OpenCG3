#ifndef OPENCG3_INPUT_HANDLER_H
#define OPENCG3_INPUT_HANDLER_H

#include <deque>
#include <vector>

#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

#include <mutex>

#include "StringParser.hpp"
#include "Common.hpp"
#include "CmdParser.hpp"

using namespace std;

namespace OpenCG3 {

	namespace Input {


		/// global variable for data exchange ...
		/**
		*  Mutex for CommandQueue.
		*  Any action on CommandQueue should be locked by AUTOLOCK(T, mutex).
		*  When action is finished unlock the CommandQueue by AUTOUNLOCK.
		*  Relate macro is defined in Common.hpp
		*/
		extern mutex mutex_CommandQueue;
		extern deque<CmdParser::Command *> CommandQueue;

		/*
		* if main thread is closing, set this variable to false,
		* shut down the infinite loop inside stdin_handle_worker.
		*/
		extern volatile bool is_alive;
		/*
		*  main function for command input ...
		*/
		void stdin_handle_worker(deque<CmdParser::Command *> &Queue);
		// auxilliary functions
		
	}
}

#endif // OPENCG3_INPUT_HANDLER_H
