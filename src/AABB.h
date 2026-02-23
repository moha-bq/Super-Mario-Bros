#pragma once
#include "Vector2.h"


class AABB
{
    public:
        AABB(){mCenter=Vector2(0,0);mHalf=Vector2(0,0);}
        AABB(Vector2 center,Vector2 half);
        virtual ~AABB();

        void extent(Vector2 v){mHalf+=v;}
        void extent(int x,int y){mHalf.x+=x;mHalf.y+=y;}

		Vector2 getBottomLeft(){return mCenter+Vector2(-mHalf.x,mHalf.y);}
        Vector2 getBottomRight(){return mCenter+Vector2(mHalf.x,mHalf.y);}
        Vector2 getTopLeft(){return mCenter+Vector2(-mHalf.x,-mHalf.y);}
        Vector2 getTopRight(){return mCenter+Vector2(mHalf.x,-mHalf.y);}

		bool collide(const AABB& a);
        static bool collide(const AABB& a,const AABB& b);
		static AABB minAABB(const AABB& a,const AABB& b);
		static AABB maxAABB(const AABB& a,const AABB& b);
        bool isIn(Vector2 point);

    public:

    Vector2 mCenter;
    Vector2 mHalf;
};