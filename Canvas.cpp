#include "Canvas.hpp"

using namespace OpenCG3;

Canvas::OperationState
Canvas::addItem(string const &key, Point const &item)
{
	if (this->point_list.count(key))
		return Item_Exist;
	else
	{
		this->point_list.insert({ string(key), Point(item) });
		return Successfully_Insert;
	}
	return Unknown_Error;
}

Canvas::OperationState
Canvas::delItem(string const & key)
{
	if (this->point_list.count(key)) {
		this->point_list.erase(key);
		return Successfully_Delete;
	}
	else
		return Not_Found;
	return Unknown_Error;
}

bool Canvas::refresh_screen()
{

	return false;
}
