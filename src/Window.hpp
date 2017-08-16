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
		const static void(*OP_FUNC[][])(MainWindow* _this, CmdParser::Command *const arg);
		/* set window attributes
		*  create window title(Str) coord_win_cent(Vertex3d) \
		*  direct(Tuple(Vx(Vector), Vy(Vector), Vz(Vector)))
		*/
		void op_create_window(CmdParser::Command *const arg);
		/* close current window
		*  delete window message(Str)
		*/
		void op_delete_window(CmdParser::Command *const arg);
		/* create point
		*  create point point_lbl(Str) coord(Vertex3d)
		*/
		void op_create_point(CmdParser::Command *const arg);
		/* delete point
		*  delete point point_lbl(Str)
		*/
		void op_delete_point(CmdParser::Command *const arg);
		/* set attribute for object
		*  attach attrib palette(Str) point_lbl(Str)
		*/
		void op_attach_attrib(CmdParser::Command *const arg);

	};
}

#endif //!OPENCG3_WINDOW_HEADER
