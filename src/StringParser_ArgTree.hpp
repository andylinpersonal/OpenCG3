#pragma once
#ifndef OPENCG3_STRINGPARSER_ARGTREE_H
#define OPENCG3_STRINGPARSER_ARGTREE_H

#include <stack>
#include <deque>
#include <string>
#include <stdexcept>
#include <climits>
using namespace std;

#include "Common.hpp"

namespace OpenCG3 {

	namespace StringParser {

		class ArgTree {
		public:

			/* 
			*  Type of node...
			*  Set = {}, Tuple = (), Vector = <>, Universal = [] (no special purpose now)
			*  Only Ctnr_Root and Ctnr_Univ can contain different type of child...
			*/
			typedef enum _Op_Type
			{
				Undefined_Op, Create, Delete, Attach, Detach, Remove, Select
			} Op_t;

			typedef enum _Obj_Type
			{
				Undefined_Obj, Window, Camera, Line, Point, Attrib
			} Obj_t;

			typedef enum _Nd_Type
			{
				Invalid_Node, Str, Real, Natural, Ctnr_Set, Ctnr_Tuple, Ctnr_Vector, Ctnr_Univ, Ctnr_Root, Empty
			} NodeType;

			class Node {
			public:
				// defines

				typedef union num_val
				{
					long double fp64;
					uint64_t     i64;
				} Number;

				const static size_t npos = ULLONG_MAX;
#define ID_NOT_FOUND ULLONG_MAX
#define STR_NULL  ""
#define XSTR_NULL ""_xs
				// get root node
#define GET_ROOT_FROM_ITER(iter) (*((*(iter))->root))
				// get node by subscript in root level
#define GET_ROOT_ELEMT(iter, idx_at_root_lvl) (*(GET_ROOT_FROM_ITER(iter)[(idx_at_root_lvl)]))
				 
				// members
			public:
				NodeType _Node_Type;
				string   _Str_val;
				Number   _Num_val;

				deque<Node *> child;
				
				// methods
			public:

				Node(NodeType t = Invalid_Node, string const& val = "");
				// deep copy ctor
				Node(Node const &src);
				~Node();

				/// member function
				inline void  clear(void)
				{
					for (Node *item : this->child)
						if (item != NULL) delete item;

					this->child.clear();
					this->_Num_val.i64 = 0;
					this->_Str_val.clear();
					this->_Node_Type = NodeType::Empty;
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
				// get set
				Node               *get_deep_copy(void) const;
				string              to_string(void) const;
				inline string const&get_str_val() { return this->_Str_val; }
				inline uint64_t    &uint() { return this->_Num_val.i64; }
				inline uint64_t     get_uint() { return this->_Num_val.i64; }
				inline long double &ldouble() { return this->_Num_val.fp64; }
				inline long double  get_ldouble() { return this->_Num_val.fp64; }
				inline NodeType const&  get_type() { return this->_Node_Type; }

			private:
				ExtensibleString aux_to_string(void) const;
			};

			class Iterator {

			/// members
			public:
				stack<ArgTree::Node *> traverse_stack;
				ArgTree::Node *current;

			public:
				Iterator(void);
				Iterator(Node *root);
				~Iterator(void);

				/// methods
				inline Node *operator*() { return this->current; };
				/// if operation failed, return false, iterator remain as is.
				bool to_child_back(void);
				bool to_child_front(void);
				bool to_child_at(size_t idx = 0);
				bool to_neighbor_next(void);
				bool to_neighbor_back(void);
				bool to_parent(void);
				bool push_back_curr_lv(Node *in = NULL);
				inline bool push_back_child(Node *in = NULL)
				{
					this->current->child.push_back(in);
					return true;
				}
				bool to_root(Node *root = NULL);
				/// get current node index of parent deque...
				size_t get_node_idx_in_parent(void);
			};


		public:
			// member ...
			Node *root;
			Iterator iter;

		private:
			string pattern;
			size_t phy_line_number;
			size_t log_line_number;

			Op_t   Op;
			Obj_t  Obj;

		public:
			ArgTree();
			// copy all without root
			ArgTree(ArgTree const&);
			~ArgTree();

			// methods ...
			inline void          iter_return_root(void) { this->iter.to_root(this->root); }
			// Copy all with root
			inline ArgTree *     get_deep_copy(void)
			{
				ArgTree *out = new ArgTree(*this);
				out->root = new Node(*this->root);
				return out;
			}
			bool                 clear(void);
			// get child in root node by subscript
			inline Node *        operator[](size_t idx)
			{
				return this->root->child[idx];
			}
			inline string const& to_string(void)
			{ 
				if (this->root)
					return this->root->to_string();
				else
					return "";
			}
			// get set
			inline string const& get_pattern(void) { return this->pattern; }
			inline void          set_pattern(string const &pat) { this->pattern = string(pat); }
			inline void          set_physical_line_no(size_t no) { this->phy_line_number = no; }
			inline size_t &      physical_line_no(void) { return this->phy_line_number; }
			inline size_t const  get_physical_line_no(void) const { return this->phy_line_number; }

			inline void          set_logical_line_no(size_t no) { this->log_line_number = no; }
			inline size_t &      logical_line_no(void) { return this->log_line_number; }
			inline size_t const  get_logical_line_no(void) const { return this->log_line_number; }
			inline Obj_t const   get_object_type(void) const { return this->Obj; }
			inline void          set_object_type(Obj_t T) { this->Obj = T; }
			inline Op_t  const   get_opcode_type(void) const { return this->Op; }
			inline void          set_opcode_type(Op_t T) { this->Op = T; }

		};
	}
}
#endif // !OPENCG3_STRINGPARSER_ARGTREE_H
