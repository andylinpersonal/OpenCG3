#ifndef OPENCG3_WINDOW_HEADER
#define OPENCG3_WINDOW_HEADER

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include <unordered_map>
#include <string>

#include "main.hpp"
#include "InputHandler.hpp"
#include "Canvas.hpp"

namespace OpenCG3 {

class MainWindow : public Gtk::Window {
public:
    /// Constants for default setting...
    static const Size2D DefaultWindowSize;
    static const Glib::ustring DefaultWindowName;
    /// ctor
    MainWindow(Size2D const& sz_win = DefaultWindowSize, Glib::ustring const& title = DefaultWindowName);
    /// set get



protected:
    /// Life status
    bool isAlive;
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



};

}

#endif //!OPENCG3_WINDOW_HEADER
