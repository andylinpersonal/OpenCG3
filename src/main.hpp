#ifndef OPENCG3_H
#define OPENCG3_H

#include <getopt.h>
#include <iostream>
#include <string>

#include <thread>

#include <gtkmm.h>

#include "Window.hpp"
#include "InputHandler.hpp"

extern std::thread Thread_stdin_handler;
extern Glib::RefPtr<Gtk::Application> App;

#endif //!OPENCG3_H
