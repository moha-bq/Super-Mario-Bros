#pragma once
//holds anim related info

struct AnimDesc
{
    AnimDesc(int frameW=0,int frameH=0,int nCol=0,int from=0,int to=0,float freq=0,bool loop=true,int start=-1,int offsetX=0,int offsetY=0)
	{
		mFrom=from;
		mTo=to;
		mW=frameW;
		mH=frameH;
		mNumColumn=nCol;
		mOffsetX=offsetX;
		mOffsetY=offsetY;
		if(start<0)
		mStart=from;
		else
		mStart=start;
		mFreq=freq;
		mLoop=loop;
	}

    int mFrom,mTo;
    int mNumColumn;
    int mW,mH;
	int mStart;
	float mFreq;
	bool mLoop;//could be put in  the anim class instead!!

	int mOffsetX,mOffsetY;//offsets...used so many AnimDesc share the same texture(Atlas);

};
