#pragma once

class Vector2
{
public:
    float x;
    float y;

    Vector2();
    Vector2(float ex,float ey);
    //Vector2(const Vector2 & v);
    Vector2(const Vector2 & from,const Vector2 & to);

	bool operator==(const Vector2 & v){return (x==v.x && y==v.y);}
    Vector2 & operator= (const Vector2 & v);

    Vector2 & operator+= (const Vector2 & v);
    Vector2 operator+ (const Vector2 & v) const;

    Vector2 & operator-= (const Vector2 & v);
    Vector2 operator- (const Vector2 & v) const;

    Vector2 & operator*= (const float a);
    Vector2 operator* (const float a)const;
    friend Vector2 operator* (const float a,const Vector2 & v);
	float operator* (const Vector2 &v)	const	{	return (x*v.x) + (y*v.y); }

	float operator ^ (const Vector2 &v)	const	{	return (x * v.y) - (y * v.x); }

    Vector2 & operator/= (const float a);
    Vector2 operator/ (const float a)const;

    float length()const;
    Vector2 & normalize();
	Vector2 xvector(){return Vector2(x,0);}
	Vector2 yvector(){return Vector2(0,y);}

	float dot(const Vector2 & v){ return(x*v.x+y*v.y); }
	static float dot(const Vector2 & v1,const Vector2 & v2){ return(v1.x*v2.x+v1.y*v2.y); }
	float cross(const Vector2 & v) { return(x*v.y-v.x*y); }
	static float cross(const Vector2 & v1,const Vector2 & v2){ return(v1.x*v2.y-v2.x*v1.y); }

	void setxy(float x,float y){ x=x; y=y;}

	Vector2 direction(void) const;
	Vector2& rotate(float angle);
	Vector2& rotate(const Vector2& centre, float angle);
	void up_bound(const Vector2& max);
	void clamp(const Vector2& min, const Vector2& max);
	void randomise(const Vector2& min, const Vector2& max);
	Vector2 projection(const Vector2 img);
	void project(const Vector2 img);
	Vector2 normal();
};



