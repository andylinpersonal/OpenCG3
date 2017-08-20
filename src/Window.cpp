#include "Window.hpp"

using namespace OpenCG3;

const Size2D
MainWindow::DefaultWindowSize = Size2D(400, 300);

const Glib::ustring
MainWindow::DefaultWindowName = "OpenCG3 - Default";

string
MainWindow::IconFileName = "ocg3.png";
// instance of this Gtk::Application ...
Glib::RefPtr<Gtk::Application> App;

const unordered_map<string, unordered_map<string, MainWindow::op_handler_t>>
MainWindow::Operation = 
{
	{ "create", {
		{ "window", &(MainWindow::op_create_window) },
		{ "camera",  &(MainWindow::op_create_camera) },
		{ "point",  &(MainWindow::op_create_point) },
		{ "line",  &(MainWindow::op_create_line) },
		{ "attrib",  &(MainWindow::op_create_attrib) }
	} },
	{ "delete", {
		{ "window", &(MainWindow::op_delete_window) },
		{ "point", &(MainWindow::op_delete_point) },
		{ "camera",  &(MainWindow::op_delete_camera) },
		{ "line",  &(MainWindow::op_delete_line) },
		{ "attrib",  &(MainWindow::op_delete_attrib) }
	} },
	{ "remove", {
		{ "camera", &(MainWindow::op_remove_camera) }
	} },
	{ "select",{
		{ "camera", &(MainWindow::op_select_camera) }
	} },
	{ "attach",{
		{ "attrib", &(MainWindow::op_attach_attrib) }
	} },
	{ "detach",{
		{ "camera", &(MainWindow::op_detach_attrib) }
	} }
};

MainWindow::MainWindow(string const& icon, Size2D const& sz_win, Glib::ustring const& title)
    :Window()
{
    set_border_width(5);
    set_default_size(sz_win.getX(), sz_win.getY());
    set_title(title);
    try{
		if(icon != STR_NULL)
			set_icon_from_file(icon);
		else
			set_icon_from_file(IconFileName);
    }
    catch(std::exception e) {
        cerr << "Error:\n\tIcon file: \"" << IconFileName << "\" missing.\n" << endl;
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
	CmdParser::Command *tmp = NULL;

	AUTOLOCK(mutex, Input::mutex_CommandQueue)
		if (!Input::CommandQueue.empty())
		{
			tmp = Input::CommandQueue.back();
			Input::CommandQueue.pop_back();
		}
	AUTOUNLOCK;
	if (tmp)
	{
		try
		{
			// Search in Operation and Call Corresponding Function
			(this->*Operation.at((*tmp)[0]->str_val).at((*tmp)[1]->str_val))(tmp);
		}
		catch (out_of_range e)
		{
			fprintf(stderr, "  at file %s, line %s,\n    in function %s\n"
				"    command: %s\n"
				"    message: %s\n",
				__FILE__, __LINE__, __FUNCTION__, tmp->param->to_string().c_str(), 
				"Error: Command Not Found In MainMenu::Operation"
			);
			fprintf(stderr, "  Message from exception:\n   ", e.what());
		}
		// clear item
		delete tmp;
	}
    
    // Always return true to keep handler connected.
    return true;
}
/*
   Op functions
*/

void
MainWindow::op_create_window(CmdParser::Command * const arg)
{
	this->title = (*arg)[2]->str_val;
	this->set_title(this->title);
#ifdef DEBUG
	cout << "Message:\n " << << arg->to_string() << endl;
#endif // DEBUG
}

void
MainWindow::op_delete_window(CmdParser::Command * const args)
{
	fputs(" info: delete window\n  message: ", stderr);
	Glib::ustring msg = "Good bye!";
	if ((*args)[2])
		msg = (*args)[2]->str_val;
	cout << msg << endl;
	Gtk::MessageDialog msgbox(
		*this,
		msg,
		false,
		Gtk::MESSAGE_INFO,
		Gtk::BUTTONS_OK_CANCEL,
		false
		);

	int result = msgbox.run();
	switch (result)
	{
	case Gtk::RESPONSE_OK:
		this->close();
		break;
	case Gtk::RESPONSE_CANCEL:
		fputs(" Info: action abort", stderr);
		break;
	default:
		fputs(" Error: Unexpected response!", stderr);
		fprintf(stderr, "  at file %s, line %s,\n    in function %s\n", __FILE__, __LINE__, __FUNCTION__);
		break;
	}
	
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
