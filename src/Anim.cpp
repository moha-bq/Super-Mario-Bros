#include "Anim.h"

Anim::Anim(AnimDesc data)
{
	mDesc=data;
	mAnimTime=0;
	mCurrFrame=mDesc.mStart;
	mPlay=true;
}


Anim::~Anim(void)
{
}

void Anim::update(float dt)
{
	if(mPlay)
	{
			int frameAdv=mAnimTime*mDesc.mFreq;
			mAnimTime-=(float)(frameAdv/mDesc.mFreq)-dt;
			if(frameAdv)
			{
				if(frameAdv+mCurrFrame>mDesc.mTo && !mDesc.mLoop)
				{
					mCurrFrame=mDesc.mStart;
					mPlay=false;
				}
				else
				{
					mCurrFrame=(frameAdv+mCurrFrame-mDesc.mFrom)%(mDesc.mTo-mDesc.mFrom+1)+mDesc.mFrom;//ensure _looping
				}
			}
	}
}

void Anim::getClip(float *x,float *y,float* w,float* h)
{
	*x=(mCurrFrame%mDesc.mNumColumn)*mDesc.mW+mDesc.mOffsetX;
	*y=(mCurrFrame/mDesc.mNumColumn)*mDesc.mH+mDesc.mOffsetY;
	*w=mDesc.mW;
	*h=mDesc.mH;
}