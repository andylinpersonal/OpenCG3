#include "StringParser_ArgTree.hpp"

using namespace OpenCG3;
using namespace std;

StringParser::ArgTree::Node::Node(Type t, string const& val)
	:type(t), str_val(string(val)){ }

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

bool
StringParser::ArgTree::Node::isLeaf()
{
	return this->child.empty();
}

StringParser::ArgTree::Node *
StringParser::ArgTree::Node::operator[](size_t pos)
{
	size_t sz = this->child.size();
	if ((pos < 0) || (pos >= sz))
	{
		string msg = "OpenCG3::StringParser::ArgTree::Node::at(" + pos;
		msg += ") : index is out of range.";
		throw std::out_of_range(msg.c_str());
	}
	return this->child[pos];
}

StringParser::ArgTree::Node *
StringParser::ArgTree::Node::at(size_t pos)
{
	return (*this)[pos];
}


StringParser::ArgTree::ArgTree()
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

void
StringParser::ArgTree::iter_return_root()
{
	this->iter.to_root(this->root);
}

string const &
StringParser::ArgTree::get_pattern()
{
	return this->pattern;
}

void OpenCG3::StringParser::ArgTree::set_pattern(string const&pat)
{
	this->pattern = string(pat);
}

void
StringParser::ArgTree::set_phy_line_no(size_t no)
{
	this->phy_line_no = no;
}

size_t
StringParser::ArgTree::get_phy_line_no()
{
	return this->phy_line_no;
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

StringParser::ArgTree::Node *
StringParser::ArgTree::Iterator::operator->()
{
	return this->current;
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
StringParser::ArgTree::Iterator::new_neighber_at_end(Node *new_node)
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
StringParser::ArgTree::Iterator::new_child(Node *new_node)
{
	this->current->child.push_back(new_node);
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
StringParser::ArgTree::Iterator::get_current_node_idx(void)
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
