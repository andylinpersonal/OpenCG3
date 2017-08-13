#pragma once
#ifndef OPENCG3_STRINGPARSER_ARGTREE_H
#define OPENCG3_STRINGPARSER_ARGTREE_H

#include <stack>
#include <deque>
#include <string>
#include <stdexcept>
#include <climits>
using namespace std;

namespace OpenCG3 {

	namespace StringParser {

		class ArgTree {
		public:
			/// Type of node...
			// Set = {}, Tuple = (), Vector = <>, Universal = [] (no special purpose now)
			// Only Ctnr_Root and Ctnr_Univ can contain different type of child...
			enum Type
			{
				Invalid, Str, Real, Natural, Ctnr_Set, Ctnr_Tuple, Ctnr_Vector, Ctnr_Univ, Ctnr_Root, Empty
			};
			
			class Node {
			public:
#define ID_NOT_FOUND ULLONG_MAX
#define STR_NULL  string("")
				const static size_t npos = ULLONG_MAX;
				Node(Type t = Invalid, string const& val = "");
				~Node();
				typedef union num_val
				{
					long double fp64;
					uint64_t     n64;
				} Number;

				Type type;
				string str_val;
				Number num_val;

				deque<Node *> child;

				size_t phy_line_no = 0, logical_line_no = 0;

				/// member function
				bool isLeaf();
				Node *operator[](size_t pos);
				Node *at(size_t pos);

			};

			class Iterator {
			public:
				Iterator();
				Iterator(Node *root);
				~Iterator();
				/// members
				stack<ArgTree::Node *> traverse_stack;
				ArgTree::Node *current;
				/// methods
				Node *operator->();
				/// if operation failed, return false, iterator remain as is.
				bool to_child_back();
				bool to_child_front();
				bool to_child_at(size_t idx = 0);
				bool to_neighbor_next();
				bool to_neighbor_back();
				bool to_parent();
				bool new_neighber_at_end(Node *in = NULL);
				bool new_child(Node *in = NULL);
				bool to_root(Node *root = NULL);
				/// get current node index of parent deque...
				size_t get_current_node_idx(void);
			};

			ArgTree();
			~ArgTree();
			// member ...
			Node *root;
			Iterator iter;
			// methods ...
			void iter_return_root();
			// get set
			string const& get_pattern();
			void set_pattern(string const&);
			void set_phy_line_no(size_t no);
			size_t const get_phy_line_no();

		private:
			string pattern;
			size_t phy_line_no;
		};
	}
}
#endif // !OPENCG3_STRINGPARSER_ARGTREE_H
