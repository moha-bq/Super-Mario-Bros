#pragma once

#include <hge.h>

#include "Vector2.h"
#include "AnimDesc.h"
#include "Anim.h"

class Piranha
{
public:

	enum{SLEEP,HAMHAM};

public:

	void __load();


	Piranha(int head=-1);
	~Piranha(void);

	void render();
	void update();

	
	Vector2 mPos;
	Vector2 mVel;//frame displacement
	Vector2 mInitPos;//for save files

	Anim* mAnim;

	HGE* hge;

	int mState;

	HTEXTURE mTex;
	hgeQuad mQuad;

	const float mAccelY;
	const float mSleepTime;

	float mSleepTimer;

	float mSymY;
};

