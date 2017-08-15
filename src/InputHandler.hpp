#ifndef OPENCG3_INPUT_HANDLER_H
#define OPENCG3_INPUT_HANDLER_H

#include <deque>
#include <vector>

#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

#include <mutex>

#include "CmdParser.hpp"
#include "StringParser.hpp"

using namespace std;

namespace OpenCG3 {

	namespace Input {

#define AUTOLOCK(MutexType, to_lock)                                          \
{   lock_guard<MutexType> lock(to_lock);

#define AUTOLOCK_END                                                          \
}

		/// global variable for data exchange ...
		extern mutex mutex_CommandQueue;

		extern deque<CmdParser::Command> CommandQueue;
		/*
		* if main thread is closing, set this variable to false,
		* shut down the infinite loop inside stdin_handle_worker.
		*/
		extern volatile bool is_alive;

		/*
		*  main function for command input ...
		*/
		void stdin_handle_worker(deque<CmdParser::Command> &Queue);
		// auxilliary functions
		void safe_queue_maker(deque<StringParser::ArgTree *> *raw_arg, deque<CmdParser::Command> &queue);
		
	}
}

#endif // OPENCG3_INPUT_HANDLER_H
