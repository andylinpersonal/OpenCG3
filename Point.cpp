#include "Point.hpp"

/* class Point2D
 *
 */
// ctor
using namespace OpenCG3;

Point::Point(void)
	:_x(0), _y(0) {}

Point::Point(int x, int y)
	:_x(x), _y(y) {}

// operator
Point Point::operator+(Point const &p) const
{
	Point out = Point(*this);
	out._x += p._x;
	out._y += p._y;
	return out;
}
Point &Point::operator+=(Point const &p)
{
	this->_x += p.getX();
	this->_y += p.getY();
	return *this;
}
Point Point::operator-(Point const &p) const
{
	Point out = Point(*this);
	out._x -= p._x;
	out._y -= p._y;
	return out;
}
Point &Point::operator-=(Point const &p)
{
	this->_x -= p.getX();
	this->_y -= p.getY();
	return *this;
}
ostream &OpenCG3::operator<<(ostream &out, Point const &in)
{
	out << "(" << in._x << ", " << in._y << ")";
	return out;
}

// get set
int Point::getX(void) const { return _x; }
void    Point::setX(int const i)
{
	this->_x = i;
}
int &Point::X(void)
{
	return this->_x;
}


int Point::getY(void) const { return _y; }
void    Point::setY(int const i)
{
	this->_y = i;
}
int &Point::Y(void)
{
	return this->_y;
}

// Utility
string Point::toString(void) const
{
	string out = string();
	out =  "(" + this->_x;
	out += "," + this->_y;
	out += ")";
	return out;
}


/* class Point3D
 *
 */
// ctor
Vertex3i::Vertex3i()
	:Point(0, 0), _z(0) {}

Vertex3i::Vertex3i(int x, int y, int z)
	: Point(x, y), _z(z) {}

// operator
Vertex3i Vertex3i::operator+(Vertex3i const &p) const
{
	Vertex3i out = Vertex3i(*this);
	out.X() += p.getX();
	out.Y() += p.getY();
	out._z  += p._z;
	return out;
}
Vertex3i &Vertex3i::operator+=(Vertex3i const &p)
{
	this->X() += p.getX();
	this->Y() += p.getY();
	this->_z += p.getZ();
	return *this;
}
Vertex3i Vertex3i::operator-(Vertex3i const &p) const
{
	Vertex3i out = Vertex3i(*this);
	out.X() -= p.getX();
	out.Y() -= p.getY();
	out._z  -= p._z;
	return out;
}
Vertex3i &Vertex3i::operator-=(Vertex3i const &p)
{
	this->X() -= p.getX();
	this->Y() -= p.getY();
	this->_z -= p.getZ();
	return *this;
}
ostream &OpenCG3::operator<<(ostream &out, Vertex3i const &in)
{
	out << "(" << in.getX() << "," << in.getY() << "," << in._z << ")";
	return out;
}
// friend function: must show namespace explicitly.
istream &OpenCG3::operator >> (istream &istr, Vertex3i &target)
{
	while (istr.peek() != '(')
		istr.ignore();
	int tmp, cnt = 0;
	char ch;
	while (!isdigit(istr.peek()))
		istr.ignore();
	istr >> tmp;
	target.setX(tmp);
	while (!isdigit(istr.peek()))
		istr.ignore();
	istr >> tmp;
	target.setY(tmp);
	while (!isdigit(istr.peek()))
		istr.ignore();
	istr >> tmp;
	target.setZ(tmp);
	while (istr.peek() != ')')
		istr.ignore();
	istr.ignore();

	return istr;
}


// get set
int Vertex3i::getZ(void) const { return _z; }
void  Vertex3i::setZ(int const i)
{
	this->_z = i;
}
int &Vertex3i::Z(void)
{
	return this->_z;
}


// Utility
string Vertex3i::toString(void) const
{
	string out = string();
	out = "(" + this->getX();
	out += "," + this->getY();
	out += "," + this->_z;
	out += ")";
	return out;
}

double Vertex3i::Distance(Vertex3i const& another) const
{
	double out = sqrt(pow(
		this->getX() - another.getX(), 2) +
		pow(this->getY() - another.getY(), 2) +
		pow(this->getZ() - another.getZ(), 2)
	);
	return out;
}


/* class Vector3D
*/
// ctors
Vector3i::Vector3i()
	:Vertex3i(){}

Vector3i::Vector3i(int x, int y, int z)
	: Vertex3i(x, y, z) {}


// Operator
ostream &OpenCG3::operator<<(ostream &out, Vector3i const &in)
{
	out << "<" << in.getX() << "," << in.getY() << "," << in.getZ() << ">";
	return out;
}

istream &OpenCG3::operator >> (istream &istr, Vector3i &target)
{
	while (istr.peek() != '<')
		istr.ignore();
	stringstream tmpstr;
	int tmp, cnt = 0;
	char ch;
	while (!isdigit(istr.peek()))
		istr.ignore();
	istr >> tmp;
	target.setX(tmp);
	while (!isdigit(istr.peek()))
		istr.ignore();
	istr >> tmp;
	target.setY(tmp);
	while (!isdigit(istr.peek()))
		istr.ignore();
	istr >> tmp;
	target.setZ(tmp);
	while (istr.peek() != '>')
		istr.ignore();
	istr.ignore();

	return istr;
}

// Utility
string Vector3i::toString(void) const
{
	string out = string();
	out = "<" + this->getX();
	out += "," + this->getY();
	out += "," + this->getZ();
	out += ">";
	return out;
}

double Vector3i::Cosine(Vector3i const &another) const
{
	double num = this->getX() * another.getX() + this->getY() * another.getY() + this->getZ() * another.getZ();
	double denom = this->Length() * another.Length();
	return num / denom;
}

double Vector3i::Length(void) const
{
	double out = sqrt(pow(this->getX(), 2) + pow(this->getY(), 2) + pow(this->getZ(), 2));
	return out;
}
