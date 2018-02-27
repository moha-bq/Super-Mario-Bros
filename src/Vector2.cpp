#include <cmath>
#include <cstdlib>
#include "Vector2.h"

float frand(float x=1.0f)
{
	return (rand() / (float) RAND_MAX) * x;
}

Vector2::Vector2()
{
    x = 0;
    y = 0;
}

Vector2::Vector2(float ex,float ey)
{
    x = ex;
    y = ey;
}

/*Vector2::Vector2(const Vector2& v)
{
    x = v.x;
    y = v.y;
}*/

Vector2::Vector2(const Vector2 & from,const Vector2 & to)
{
    x = to.x - from.x;
    y = to.y - from.y;
}

Vector2 & Vector2::operator= (const Vector2 & v)
{
    x = v.x;
    y = v.y;
    return *this;
}

Vector2 & Vector2::operator+= (const Vector2 & v)
{
    x += v.x;
    y += v.y;
    return *this;
}

Vector2 Vector2::operator+ (const Vector2 & v) const
{
    Vector2 t = *this;
    t += v;
    return t;
}

Vector2 & Vector2::operator-= (const Vector2 & v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2 Vector2::operator- (const Vector2 & v) const
{
    Vector2 t = *this;
    t -= v;
    return t;
}

Vector2 & Vector2::operator*= (const float a)
{
    x *= a;
    y *= a;
    return *this;
}

Vector2 Vector2::operator* (const float a)const
{
    Vector2 t = *this;
    t *= a;
    return t;
}

Vector2 operator* (const float a,const Vector2 & v)
{
    return Vector2(v.x*a,v.y*a);
}

Vector2 & Vector2::operator/= (const float a)
{
    x /= a;
    y /= a;
    return *this;
}

Vector2 Vector2::operator/ (const float a)const
{
    Vector2 t = *this;
    t /= a;
    return t;
}


float Vector2::length()const
{
    return sqrt( x*x + y*y);
}

Vector2 & Vector2::normalize()
{
    (*this) /= length();
    return (*this);
}

Vector2 Vector2::direction(void) const
{
		Vector2 temp(*this);

		temp.normalize();

		return temp;
}

Vector2& Vector2::rotate(float angle)
{
		float tx = x;
		x =  x * cos(angle) - y * sin(angle);
		y = tx * sin(angle) + y * cos(angle);

		return *this;
}

Vector2& Vector2::rotate(const Vector2& centre, float angle)
{
		Vector2 D = *this - centre;
		D.rotate(angle);

		*this = centre + D;

		return *this;
}

void Vector2::clamp(const Vector2& min, const Vector2& max)
{
		x = (x < min.x)? min.x : (x > max.x)? max.x : x;
		y = (y < min.y)? min.y : (y > max.y)? max.y : y;
}
void Vector2::up_bound(const Vector2& max)
{
	x = (x > max.x)? max.x : x;
	y = (y > max.y)? max.y : y;
}
void Vector2::randomise(const Vector2& min, const Vector2& max)
{
		x = frand(max.x - min.x) + min.x;
		y = frand(max.y - min.y) + min.y;
}

void Vector2::project(const Vector2 img)
{
	*this=this->projection(img);
}

Vector2 Vector2::projection(const Vector2 img)
{
	float scale=Vector2::dot(*this,img);
	float l=img.length();
	Vector2 r;

	r.x=scale*img.x/l;
	r.y=scale*img.y/l;

	return r;
}

Vector2 Vector2::normal()
{
	Vector2 r;
	r.x=-this->y;r.y=this->x;
	return r;
}