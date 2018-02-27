#pragma once

#include <hge.h>

#include "Vector2.h"
#include "AnimDesc.h"
#include "Anim.h"

class Mushroom
{
public:

	enum{SHOWUP=0,NORMAL};

public:
	Mushroom(void);
	~Mushroom(void);

	void render();
	void update();

	Vector2 mPos;
	Vector2 mVel;//frame displacement

	Anim* mAnim;
	HGE* hge;

	HTEXTURE mTex;
	hgeQuad mQuad;

	int mState;
	float mY;

	bool mEaten;
	bool mGrounded;

	float mShowUpTimer;

	const float mGravAccelY;
	const float mAccelX;
	const float mPropultionVel;
	const float mShowUpTime;
};

