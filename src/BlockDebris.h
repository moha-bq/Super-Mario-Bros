#pragma once

#include <hge.h>

#include "Vector2.h"
#include "AnimDesc.h"
#include "Anim.h"

class BlockDebris
{
public:
	BlockDebris();
	~BlockDebris(void);

	void operator=(BlockDebris& copy)
	{
		mPos=copy.mPos;
		mVel=copy.mVel;
		mTheta=copy.mTheta;
		mActive = copy.mTheta;

	}

	void render();
	void update();

	Vector2 mPos;
	Vector2 mVel;//frame displacement

	HGE* hge;

	HTEXTURE mTex;
	hgeQuad mQuad;

	float mTheta;

	const float mGravAccelY;
	int mActive;

};

