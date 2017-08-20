#include "main.hpp"

using namespace std;
using namespace OpenCG3;

int main(int argc, char** argv)
{
	// parse some args...
	int arg;
	string icon_filename;
	while ((arg = getopt(argc, argv, "h")) != -1) {
		switch (arg)
		{
		case 'h':
			puts("\nSimple 3D Viewer\nMade by KVD the PIG and ADL the sweet potato in 2017\n\n");
			printf("%s -[opt]\n\n", argv[0]);
			puts("\t-h\thelp: print this message.\n");
			puts("\t-i\ticon: custom icon.\n");
			return EXIT_SUCCESS;
		case 'i':
			icon_filename = string(optarg);
			break;
		case '?':
			puts("*spew* : some rot foods ...");
			puts(optarg);
			puts("Don\'t input strange things into me. Ahhhhhhhhh~~~");
			return EXIT_FAILURE;
		}
	}
	// Start background stdin recepter... (non-blocking)
	thread Thread_stdin_handler = thread(&Input::stdin_handle_worker, std::ref(Input::CommandQueue));

	OpenCG3::App = Gtk::Application::create(argc, argv);

    OpenCG3::MainWindow root_win(icon_filename);

	int ret = OpenCG3::App->run(root_win);
	Thread_stdin_handler.join();
    return ret;
}
