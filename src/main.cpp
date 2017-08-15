#include "main.hpp"

using namespace std;
using namespace OpenCG3;

int main(int argc, char** argv)
{
	// parse some args...
	int arg;
	while ((arg = getopt(argc, argv, "h")) != -1) {
		switch (arg)
		{
		case 'h':
			puts("\nSimple 3D Viewer\nMade by KVD the PIG and ADL the sweet potato in 2017\n\n");
			printf("%s -[opt]\n\n", argv[0]);
			puts("\t-h\thelp: print this message.\n");
			return EXIT_SUCCESS;
		case '?':
			puts("Don\'t input strange things into me. Ahhhhhhhhh~~~");
			return EXIT_FAILURE;
		}
	}
	// Start background stdin recepter... (non-blocking)
	thread Thread_stdin_handler = thread(&Input::StdinHandler, std::ref(Input::CommandQueue));

	OpenCG3::App = Gtk::Application::create(argc, argv);

    OpenCG3::MainWindow root_win;
    return OpenCG3::App->run(root_win);
}
