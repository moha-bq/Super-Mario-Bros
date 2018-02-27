#pragma once
#include <hge.h>

#include "Vector2.h"
#include "AnimDesc.h"
#include "Anim.h"

#define TURTLE_ANIM_NUM 3

class Turtle
{
public:
	enum {WALK=0,SHELL,WAKING,INVERT,DEAD};//dead when off the world limits in shell
public:

	void __load();
	Turtle(int head=-1);
	~Turtle(void);

	void render();
	void update();

	void move(Vector2 d);
	void move(float dx,float dy);
	void impulse(Vector2 i);
	void impulse(float ix,float iy);

	void handleWorldCollision();
	void handleEnemiesCollision();

	Vector2 mPos;
	Vector2 mVel;//frame displacement
	Vector2 mInitPos;//for save files
	int mInitHead;

	bool mActive;

	int mCurrAnim;
	Anim* mAnims[TURTLE_ANIM_NUM];

	int mState;

	HGE* hge;

	bool mGrounded;

	HTEXTURE mTex;
	hgeQuad mQuad;

	const float mAccelX;
	const float mGravAccelY;
	const float mJumpAccelY;
	const float mShellAccelX;
	const float mSleepTime;
	const float mWakingTime;

	int mWakingCounter;
	float mDimY;
	float mSleepTimer;
	float mWakingTimer;
};

