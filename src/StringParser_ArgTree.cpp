#include "StringParser_ArgTree.hpp"

using namespace OpenCG3;
using namespace std;

StringParser::ArgTree::Node::Node(Type t, string const& val)
	:type(t), str_val(string(val)){ }

StringParser::ArgTree::Node::Node(Node const &src)
	:type(src.type), str_val(src.str_val), num_val(src.num_val)
{
	this->child.clear();
	if (!this->child.empty())
	{
		this->child = deque<Node *>(src.child.size());
		for (Node *subtree : src.child)
		{
			this->child.push_back(new Node(*subtree));
		}
	}
}

StringParser::ArgTree::Node::~Node()
{
	if (!this->child.empty())
	{
		for (size_t i = 0; i < this->child.size(); ++i)
		{
			if (this->child[i])
				delete this->child[i];
		}
		this->child.clear();
	}
}

string
StringParser::ArgTree::Node::to_string(void) const
{
	ExtensibleString out = this->aux_to_string();
	return out.to_string();
}

ExtensibleString
StringParser::ArgTree::Node::aux_to_string(void) const
{
	ExtensibleString out;
	auto concatenate = [&](char const front = 0, char const back = 0)
	{
		if (front) out += front;
		for (Node *node : this->child)
		{
			out += node->aux_to_string();
			out += " "_xs;
		}
		if (front) out += back;
	};
	switch (this->type)
	{
	case ArgTree::Type::Ctnr_Root:
		concatenate();
		break;
	case ArgTree::Type::Ctnr_Set:
		concatenate('{','}');
		break;
	case ArgTree::Type::Ctnr_Tuple:
		concatenate('(', ')');
		break;
	case ArgTree::Type::Ctnr_Univ:
		concatenate('[', ']');
		break;
	case ArgTree::Type::Ctnr_Vector:
		concatenate('<', '>');
		break;
	case ArgTree::Type::Natural:
	case ArgTree::Type::Real:
	case ArgTree::Type::Str:
	case ArgTree::Type::Invalid:
	case ArgTree::Type::Empty:
		out += this->str_val;
		break;
	default:
		break;
	}
	return out;
}

StringParser::ArgTree::Node *
StringParser::ArgTree::Node::get_deep_copy(void) const
{
	Node *out = new Node(*this);
	return out;
}

StringParser::ArgTree::ArgTree() 
	:phy_line_number(0), log_line_number(0)
{
	this->root = new Node();
	this->root->type = Ctnr_Root;
	this->root->str_val = STR_NULL;
	this->root->child.clear();
	// First level is root
	this->iter = Iterator(this->root);
}

StringParser::ArgTree::~ArgTree()
{
	delete this->root;
}

bool
StringParser::ArgTree::clear(void)
{
	this->logical_line_no = 0;
	this->physical_line_no = 0;
	this->pattern = STR_NULL;
	this->root->clear();
	return false;
}

StringParser::ArgTree::Iterator::Iterator()
	:current(NULL), traverse_stack(stack<Node *>()) {}

StringParser::ArgTree::Iterator::Iterator(Node *root)
{
	this->current = root;
	this->traverse_stack = stack<Node *>();
	this->traverse_stack.push(root);
}

StringParser::ArgTree::Iterator::~Iterator()
{
	while (this->traverse_stack.size())
		this->traverse_stack.pop();
}

bool
StringParser::ArgTree::Iterator::to_child_back()
{
	if (!this->current->child.size())
		return false;
	this->current = this->current->child.back();
	this->traverse_stack.push(this->current);
	return true;
}

bool 
StringParser::ArgTree::Iterator::to_child_front()
{
	if (!this->current->child.size())
		return false;
	this->current = this->current->child.front();
	this->traverse_stack.push(this->current);
	return true;
}



bool
StringParser::ArgTree::Iterator::to_child_at(size_t idx)
{
	// out of index ...
	if (this->current->child.size() < idx)
		return false;
	this->traverse_stack.push(this->current->child[idx]);
	this->current = this->traverse_stack.top();
	return true;
}

bool
StringParser::ArgTree::Iterator::to_neighbor_next()
{
	Node *old = this->traverse_stack.top();
	this->traverse_stack.pop();
	Node *tmp = this->traverse_stack.top();
	size_t old_idx = 0, sz_child = tmp->child.size();
	this->traverse_stack.pop();
	for (size_t i = 0; i < sz_child; ++i)
	{
		if (old == tmp->child[i])
		{
			old_idx = i + 1;
			if (old_idx >= sz_child) break;
			this->current = tmp->child[old_idx];
			this->traverse_stack.push(this->current);
			return true;
		}
	}
	this->current = old;
	this->traverse_stack.push(old);
	return false;
}

bool
StringParser::ArgTree::Iterator::to_neighbor_back()
{
	this->traverse_stack.pop();
	Node *tmp = this->traverse_stack.top();
	this->current = tmp->child.back();
	this->traverse_stack.push(this->current);
	return true;
}

bool
StringParser::ArgTree::Iterator::to_parent()
{
	if (this->traverse_stack.size() <= 1)
		return false;
	this->traverse_stack.pop();
	this->current = this->traverse_stack.top();
	return true;
}

bool
StringParser::ArgTree::Iterator::push_back_curr_lv(Node *new_node)
{
	// Level 1 is root node...
	Node *old = this->current;
	if (this->traverse_stack.size() <= 1) return false;
	this->traverse_stack.pop();
	this->traverse_stack.top()->child.push_back(new_node);
	this->current = old;
	this->traverse_stack.push(old);
	return true;
}

bool
StringParser::ArgTree::Iterator::to_root(Node * root)
{
	if (root) {
		this->current = root;
		while(this->traverse_stack.size())
			this->traverse_stack.pop();
		this->traverse_stack.push(root);
	}
	else {
		while (this->traverse_stack.size() > 1)
			this->traverse_stack.pop();
		this->current = this->traverse_stack.top();
	}
	return true;
}

size_t
StringParser::ArgTree::Iterator::get_node_idx_in_parent(void)
{
	if (this->traverse_stack.size() <= 1)
		return ID_NOT_FOUND;
	Node *now = this->current;
	this->traverse_stack.pop();
	Node *parent = this->traverse_stack.top();
	size_t sz_child = parent->child.size();
	for (size_t i = 0; i < sz_child; ++i)
	{
		if (parent->child[i] == now)
			return i;
	}
	return ID_NOT_FOUND;
}
