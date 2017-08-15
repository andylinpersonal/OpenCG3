#include "Window.hpp"

using namespace OpenCG3;

const Size2D
MainWindow::DefaultWindowSize = Size2D(400, 300);

const Glib::ustring
MainWindow::DefaultWindowName = "OpenCG3 - Default";

Glib::RefPtr<Gtk::Application> App;

MainWindow::MainWindow(Size2D const& sz_win, Glib::ustring const& title)
    :Window()
{
    set_border_width(5);

    set_default_size(sz_win.getX(), sz_win.getY());
    set_title(title);
    try{
        set_icon_from_file("ocg3.png");
    }
    catch(std::exception e) {
        cerr << "Error:\n\tIcon file: \"ocg3.png\" missing.\n" << endl;
    }
    // Connecting close window event handler.
    // mem_func is the wrapper for non-static member function :
    Glib::signal_idle().connect(sigc::mem_fun(*this, &OpenCG3::MainWindow::on_idle));
    this->signal_delete_event().connect(sigc::mem_fun(*this, &OpenCG3::MainWindow::on_my_delete_event));

	/* TODO */
	/*
	this->add(this->canvas);
	this->show();
	*/
}

bool
MainWindow::on_idle()
{
	// resolve the command queue
	AUTOLOCK(mutex, Input::mutex_CommandQueue)
		if (!Input::CommandQueue.empty())
		{
			switch (true)
			{
			default:
				break;
			}
		}
	AUTOLOCK_END
    
    // Always return true to keep handler connected.
    return true;
}

bool
MainWindow::on_my_delete_event(GdkEventAny *any_event)
{
	Input::is_alive = false;

#ifdef _DEBUG
	cout << "Debug:\n\tfrom: OpenCG3::MainWindow::on_my_delete_event\n\tWindow is closing..." << endl;
#endif // _DEBUG

    App->quit();
    return true;
}
