#include "../StringParser_ArgTree.hpp"
using namespace OpenCG3::StringParser;
#include <iostream>
using namespace std;
int main()
{
	
	ArgTree *test = new ArgTree();
	test->iter.new_child(new ArgTree::Node(ArgTree::Type::Str, string("hello_world!")));
	test->iter.to_child_back();
	cout << test->iter->str_val << endl;
	test->iter.to_root();
	delete test;
}