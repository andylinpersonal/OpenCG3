#ifndef OPENCG3_WINDOW_HEADER
#define OPENCG3_WINDOW_HEADER

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include <unordered_map>
#include <string>

#include <mutex>

#include "Common.hpp"
#include "InputHandler.hpp"
#include "Canvas.hpp"

namespace OpenCG3 {

	extern Glib::RefPtr<Gtk::Application> App;

	class MainWindow : public Gtk::Window {
	public:
		/// Constants for default setting...
		static const Size2D DefaultWindowSize;
		static const Glib::ustring DefaultWindowName;
		static string IconFileName;
		string title;
		/// ctor
		MainWindow(
			string const& icon = IconFileName,
			Size2D const& sz_win = DefaultWindowSize,
			Glib::ustring const& title = DefaultWindowName
			);
		/// set get
	protected:
		/// Canvas
		Canvas canvas;

		/// Gtk event handlers ...
		/** close window
		* @return type: bool
		* value: not important
		*/
		bool on_my_delete_event(GdkEventAny *any_event);

		/** Idle function:
		* @return type: bool
		* value:
		*     true:  Keep this event handler on Glib::signal_idle(), run this over and over.
		*     false: After this run, disconnect this event handler. (stop function)
		*/
		bool on_idle(void);



		/*
		*  Working functions.
		*/
		typedef void (MainWindow::*op_handler_t)(CmdParser::Command *const arg);
		/* 
		*  Call method by corresponding key.
		*/
		const static unordered_map<string, unordered_map<string, op_handler_t>> Operation;
		/* set window attributes
		*  create window $label_name:S
		*/
		void op_create_window(CmdParser::Command *const arg);
		/* close current window
		*  delete window $label_name:S Opt_$message:S
		*/
		void op_delete_window(CmdParser::Command *const arg);
		void op_create_camera(CmdParser::Command *const arg);
		void op_delete_camera(CmdParser::Command *const arg);
		void op_remove_camera(CmdParser::Command *const arg);
		void op_select_camera(CmdParser::Command *const arg);
		/* create point
		*  create point $point_lbl:S $coord(Vertex3d)
		*/
		void op_create_point(CmdParser::Command *const arg);
		/* delete point
		*  delete point $point_lbl:S
		*/
		void op_delete_point(CmdParser::Command *const arg);

		void op_create_line(CmdParser::Command *const arg);
		void op_delete_line(CmdParser::Command *const arg);

		void op_create_attrib(CmdParser::Command *const arg);
		void op_delete_attrib(CmdParser::Command *const arg);
		void op_attach_attrib(CmdParser::Command *const arg);
		void op_detach_attrib(CmdParser::Command *const arg);

	};
}

#endif //!OPENCG3_WINDOW_HEADER
