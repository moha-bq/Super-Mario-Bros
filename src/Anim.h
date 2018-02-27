#pragma once

#include "AnimDesc.h"

class Anim
{
public:
	Anim(AnimDesc data);
	~Anim(void);

	void update(float dt);
	void play(){mPlay=true;}
	void stop(){mPlay=false;}
	void reset(){mCurrFrame=mDesc.mStart;mAnimTime=0;}

	void getClip(float *x,float *y,float* w,float* h);

	int mCurrFrame;
    bool mPlay;
	float mAnimTime;

	AnimDesc mDesc;
};

