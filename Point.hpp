#pragma once
#ifndef OPENCG3_POINT_H
#define OPENCG3_POINT_H

#include <math.h>
#include <iostream>
#include <string>
#include <sstream>

#include "IObject.hpp"

using namespace std;

#define _val_0 getX()
#define _val_1 getY()
#define _val_2 getZ()

namespace OpenCG3 {
	class Point : virtual public IObject {
	private:
		int _x, _y;
	public:
		// ctor
		Point();
		Point(int x, int y);

		// operator
		Point operator+(Point const &) const;
		Point &operator+=(Point const &);
		Point operator-(Point const &) const;
		Point &operator-=(Point const &);
		friend ostream &operator<<(ostream &, Point const &);

		// set get
		int getX(void) const;
		void  setX(int const);
		int &X(void);

		int getY(void) const;
		void  setY(int const);
		int &Y(void);

		// utility
		string toString(void) const;
	};

	typedef Point Size2D;
	ostream &operator<<(ostream &, Point const &);

	class Vertex3i : public Point {
	private:
		int _z;
	public:
		// ctor
		Vertex3i();
		Vertex3i(int x, int y, int z);

		// operator
		Vertex3i operator+(Vertex3i const &) const;
		Vertex3i &operator+=(Vertex3i const &);
		Vertex3i operator-(Vertex3i const &) const;
		Vertex3i &operator-=(Vertex3i const &);
		friend ostream &operator<<(ostream &, Vertex3i const &);
		friend istream &operator >> (istream &, Vertex3i &);

		// set get
		int getZ(void) const;
		void  setZ(int const);
		int &Z(void);

		// Utility
		string toString(void) const;
		double Distance(Vertex3i const& another = Vertex3i()) const;

	};
	ostream &operator<<(ostream &, Vertex3i const &);
	istream &operator >> (istream &, Vertex3i &);

	class Vector3i : public Vertex3i {
	public:
		Vector3i();
		Vector3i(int x, int y, int z);
		// Utility
		string toString(void) const;
		double Cosine(Vector3i const &) const;
		double Length(void) const;

		// Operator
		friend ostream &operator<<(ostream &, Vector3i const &);
		friend istream &operator >> (istream &, Vector3i &);

	};
	ostream &operator<<(ostream &, Vector3i const &);
	istream &operator >> (istream &, Vector3i &);
}

#endif // OPENCG3_POINT_H
