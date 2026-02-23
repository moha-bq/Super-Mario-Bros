#include "AABB.h"
#include <cmath>

#define MIN(x,y) ((x>=y)?y:x)
#define MAX(x,y) ((x>=y)?x:y)

AABB::AABB(Vector2 center,Vector2 half)
{
    mCenter=center;
    mHalf=half;
}

AABB::~AABB()
{
    //dtor
}
bool AABB::collide(const AABB &a)
{
	return AABB::collide(*this,a);
}
bool AABB::collide(const AABB& a,const AABB& b)
{
    Vector2 delta=a.mCenter-b.mCenter;
    Vector2 range=a.mHalf+b.mHalf;
    delta.x=std::fabs(delta.x);delta.y=std::fabs(delta.y);
    delta-=range;

    return (delta.x<0 && delta.y<0);

}

bool AABB::isIn(Vector2 point)
{
    Vector2 delta=mCenter-point;
    delta.x=std::fabs(delta.x);delta.y=std::fabs(delta.y);
    delta-=mHalf;
    return (delta.x<0 && delta.y<0);
}

AABB AABB::minAABB(const AABB& a,const AABB& b)
{
	float left1=a.mCenter.x-a.mHalf.x;
	float right1=a.mCenter.x+a.mHalf.x;
	float top1=a.mCenter.y-a.mHalf.y;
	float bottom1=a.mCenter.y+a.mHalf.y;

	float left2=b.mCenter.x-b.mHalf.x;
	float right2=b.mCenter.x+b.mHalf.x;
	float top2=b.mCenter.y-b.mHalf.y;
	float bottom2=b.mCenter.y+b.mHalf.y;

	Vector2 center,half;

	center.x=(MAX(left1,left2)+MIN(right1,right2))/2;
	center.y=(MAX(top1,top2)+MIN(bottom1,bottom2))/2;

	half.x=std::abs(MAX(left1,left2)-MIN(right1,right2))/2;
	half.y=std::abs(MAX(top1,top2)-MIN(bottom1,bottom2))/2;

	return AABB(center,half);
}


AABB AABB::maxAABB(const AABB& a,const AABB& b)
{
	float left1=a.mCenter.x-a.mHalf.x;
	float right1=a.mCenter.x+a.mHalf.x;
	float top1=a.mCenter.y-a.mHalf.y;
	float bottom1=a.mCenter.y+a.mHalf.y;

	float left2=b.mCenter.x-b.mHalf.x;
	float right2=b.mCenter.x+b.mHalf.x;
	float top2=b.mCenter.y-b.mHalf.y;
	float bottom2=b.mCenter.y+b.mHalf.y;

	Vector2 center,half;

	center.x=(MAX(left1,left2)+MIN(right1,right2))/2;
	center.y=(MAX(top1,top2)+MIN(bottom1,bottom2))/2;

	half.x=std::abs(MAX(left1,left2)-MIN(right1,right2))/2;
	half.y=std::abs(MAX(top1,top2)+MIN(bottom1,bottom2))/2;

	return AABB(center,half);
}

