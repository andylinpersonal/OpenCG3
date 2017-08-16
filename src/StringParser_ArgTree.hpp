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

			/* 
			*  Type of node...
			*  Set = {}, Tuple = (), Vector = <>, Universal = [] (no special purpose now)
			*  Only Ctnr_Root and Ctnr_Univ can contain different type of child...
			*/
			typedef enum _Type
			{
				Invalid, Str, Real, Natural, Ctnr_Set, Ctnr_Tuple, Ctnr_Vector, Ctnr_Univ, Ctnr_Root, Empty
			}Type;

			class Node {
			public:

#define ID_NOT_FOUND ULLONG_MAX
#define STR_NULL  ""
#define XSTR_NULL ""_xs

				const static size_t npos = ULLONG_MAX;
				Node(Type t = Invalid, string const& val = "");
				Node(Node &src);
				~Node();
				typedef union num_val
				{
					long double fp64;
					uint64_t     n64;
				} Number;

				Type   type;
				string str_val;
				Number num_val;

				deque<Node *> child;

				/// member function
				inline void  clear(void)
				{
					for (Node *item : this->child)
						if (item != NULL) delete item;
					this->child.clear();
				}

				inline bool  isLeaf(void) { return this->child.empty(); }
				inline Node *operator[](size_t pos)
				{
					size_t sz = this->child.size();
					if (pos >= sz)
					{
						string msg = "OpenCG3::StringParser::ArgTree::Node::at(" + pos;
						msg += ") : index is out of range.";
						throw std::out_of_range(msg.c_str());
					}
					return this->child[pos];
				}

				inline Node *at(size_t pos) { return (*this)[pos]; }
				


			};

			class Iterator {
			public:
				Iterator(void);
				Iterator(Node *root);
				~Iterator(void);
				/// members
				stack<ArgTree::Node *> traverse_stack;
				ArgTree::Node *current;
				/// methods
				inline Node *operator*() { return this->current; };
				/// if operation failed, return false, iterator remain as is.
				bool to_child_back(void);
				bool to_child_front(void);
				bool to_child_at(size_t idx = 0);
				bool to_neighbor_next(void);
				bool to_neighbor_back(void);
				bool to_parent(void);
				bool new_neighber_at_end(Node *in = NULL);
				inline bool new_child(Node *in = NULL)
				{
					this->current->child.push_back(in);
					return true;
				}
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
			inline void          iter_return_root(void)               { this->iter.to_root(this->root); }
			inline ArgTree *     get_deep_copy(void)
			{
				ArgTree *out = new ArgTree(*this);
				out->root = new Node(*this->root);
				out->pattern = this->pattern;
				out->phy_line_number = this->phy_line_number;
				out->log_line_number = this->log_line_number;

				return out;
			}

			// get set
			inline string const& get_pattern(void)                { return this->pattern; }
			inline void          set_pattern(string const &pat)   { this->pattern = string(pat); }
			inline void          set_physical_line_no(size_t no)  { this->phy_line_number = no; }
			inline size_t &      physical_line_no(void)           { return this->phy_line_number; }
			inline size_t        get_physical_line_no(void) const { return this->phy_line_number; }

			inline void          set_logical_line_no(size_t no)   { this->log_line_number = no; }
			inline size_t &      logical_line_no(void)            { return this->log_line_number; }
			inline size_t        get_logical_line_no(void) const  { return this->log_line_number; }

		private:
			string pattern;
			size_t phy_line_number;
			size_t log_line_number;
		};
	}
}
#endif // !OPENCG3_STRINGPARSER_ARGTREE_H
