#pragma once
#ifndef OPENCG3_WINDOW_DRAWINGAREA_H
#define OPENCG3_WINDOW_DRAWINGAREA_H

#include <unordered_map>
#include <iterator>
#include <string>
using namespace std;

#include <gtkmm.h>

#include "Point.hpp"

namespace OpenCG3 {
	class Canvas : public Gtk::DrawingArea {
	public:
		enum OperationState{
			Unknown_Error, Successfully_Delete, Not_Found, Successfully_Insert, Item_Exist
		};
		Canvas();
		~Canvas();

		OperationState addItem(string const& key, Point const& item);
		OperationState delItem(string const& key);
		
		bool refresh_screen();
	protected:
		bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

		unordered_map<string, Point> point_list;
		

	};
}

#endif // OPENCG3_WINDOW_DRAWINGAREA_H
