#include "CmdParser.hpp"

using namespace std;
using namespace OpenCG3;
using namespace OpenCG3::CmdParser;

/// constants
const char*
CmdParser::OP_NAME[5] = {"", "create", "delete", "assign", "attach"};

const char*
CmdParser::OBJ_NAME[6] = {"", "window", "point", "line", "camera", "attrib"};



const unordered_map<string, int>
CmdParser::OP_ID = {{OP_NAME[0], 0} ,{OP_NAME[1], 1}, {OP_NAME[2], 2}, {OP_NAME[3], 3}};

const unordered_map<string, int>
CmdParser::OBJ_ID = {{OBJ_NAME[0], 0} ,{OBJ_NAME[1], 1}, {OBJ_NAME[2], 2}, {OBJ_NAME[3], 3}};

void
CmdParser::safe_queue_maker(deque<StringParser::ArgTree*> *const raw_arg, deque<StringParser::ArgTree *> &Queue, mutex &Lock)
{
	/* because an input may not include a complete assign command
	*  We must have a variable to store opcode and object of this assign
	*/
	static StringParser::ArgTree cached_assign_cmd;
	// for assign command processing
	static bool unfinished_assign_op = false;
	static size_t unfinished_assign_count = 0;
	static deque<StringParser::ArgTree *> cmd_cache;

	
	{
		// don't delete this, because it'll be send into global command queue.
		StringParser::ArgTree *temp_tree = new StringParser::ArgTree();
		deque<StringParser::ArgTree *>::iterator it = raw_arg->begin();
		/* expanding assign opname and copy other command as-is.
		 * command format:
		 * assign $opname:S $op:S $class:S $count:N
		 */
		while (it != raw_arg->end())
		{
			if (GET_ROOT_ELEMT(it, 0).get_str_val() == "assign")
			{
				
				unfinished_assign_op = true;
				try {
					unfinished_assign_count = GET_ROOT_ELEMT(it, 4).get_uint();
					// copy $op and $class into template (deep copy)
					cached_assign_cmd.root->child.push_back(new StringParser::ArgTree::Node(GET_ROOT_ELEMT(it, 2)));
					cached_assign_cmd.root->child.push_back(new StringParser::ArgTree::Node(GET_ROOT_ELEMT(it, 3)));
				}
				catch (exception e) {
					fprintf(stderr, "Error: \n Invalid \"assign\" command:\n    %s", (*it)->to_string().c_str());
					fprintf(stderr, "  Message from exception:\n%s", e.what());
					DEBUG_TRACEBACK;
				}
				// free the assign op itself
				delete *it;
			}
			if (!unfinished_assign_count)
			{
				unfinished_assign_op = false;
				cached_assign_cmd.clear();
			}
			else if (unfinished_assign_op)
			{
				// Generate new tree based on each command, then append the op and obj from template...
				// copy by pointer
				temp_tree = (*it);
				for (deque<StringParser::ArgTree::Node *>::reverse_iterator rit_cached_op = cached_assign_cmd.root->child.rbegin();
					rit_cached_op != cached_assign_cmd.root->child.rend();
					++rit_cached_op)
				{
					temp_tree->root->child.push_front(new StringParser::ArgTree::Node(*(*rit_cached_op))); // deep copy from template
				}
				// oooooo->new
				cmd_cache.push_back(temp_tree); // by pointer
                --unfinished_assign_count;
			}
			else
			{
				cmd_cache.push_back(*it); // copy reference
			}
			++it;
		}
	}
	/*
	*  if there is any unfinished assign op, return for
	*  next command input
	*/
	if (unfinished_assign_op) return;
	
	// set op type of expand cmds
	for (StringParser::ArgTree *node : cmd_cache)
	{
		StringParser::cmd_get_op_obj_type(*node);
	}

	// Expand compound commands
	{
		// 
		StringParser::ArgTree *template_tree = NULL;

		auto set_template = [&](StringParser::ArgTree *tmplt, StringParser::ArgTree *src)
		{
			if (tmplt) delete tmplt;
			tmplt = src->get_deep_copy();
		};

		deque<StringParser::ArgTree *>::iterator it = cmd_cache.begin();
		while (it != cmd_cache.end())
		{
			switch ((*it)->get_opcode_type())
			{
			case StringParser::ArgTree::Op_t::Create:
				switch ((*it)->get_object_type())
				{
				case StringParser::ArgTree::Obj_t::Attrib:
					
					break;
				case StringParser::ArgTree::Obj_t::Line:
					break;
				case StringParser::ArgTree::Obj_t::Point:
					break;
				default:
					break;
				}
				break;
			case StringParser::ArgTree::Op_t::Delete:
				switch ((*it)->get_object_type())
				{
				case StringParser::ArgTree::Obj_t::Attrib:
				case StringParser::ArgTree::Obj_t::Line:
				case StringParser::ArgTree::Obj_t::Point:
					set_template(template_tree, *it);
					if (GET_ROOT_ELEMT(it, 2).get_type() == StringParser::ArgTree::NodeType::Ctnr_Set)
					{
						deque<StringParser::ArgTree *> tmp_lst;
						// resolved tree for Queue
						StringParser::ArgTree *tmp_tree = NULL;
						for (size_t i = 0; i < GET_ROOT_ELEMT(it, 2).child.size(); ++i)
						{
							tmp_tree = new StringParser::ArgTree(*template_tree);
							// append item in set to each new-generated arg-tree.
							// deep copy from raw command
							tmp_tree->root->child.push_back(new StringParser::ArgTree::Node(*GET_ROOT_ELEMT(it, 2)[i]));
							tmp_lst.push_back(tmp_tree); // copy by pointer...
							tmp_tree = NULL;
						}
						delete *it;
					}
					break;
				default:
					break;
				}
				break;
			case StringParser::ArgTree::Op_t::Detach:
				switch ((*it)->get_object_type())
				{
				case StringParser::ArgTree::Obj_t::Attrib:
					break;
				default:
					break;
				}
				break;
			case StringParser::ArgTree::Op_t::Attach:
				switch ((*it)->get_object_type())
				{
				case StringParser::ArgTree::Obj_t::Attrib:
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			delete template_tree;
			template_tree = NULL;
			++it;
		}
	}

	// move cached cmd to global queue
	AUTOLOCK(mutex, Lock)
		for (deque<StringParser::ArgTree *>::reverse_iterator rit = cmd_cache.rbegin(); rit != cmd_cache.rend(); ++rit)
			Queue.push_front(*rit);
	AUTOUNLOCK;
	// because cmd_cache is a static varible, we must clear it manually.
	cmd_cache.clear();
}
