#pragma once
#include <hge.h>

#include "Vector2.h"
#include "AnimDesc.h"
#include "Anim.h"

#define GOOMBA_ANIM_NUM 2

class Goomba
{
public:

	enum {WALK=0,DYING,DEAD,INVERT};
public:
	Goomba(int head=-1);
	~Goomba(void);

	void __load();

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

	int mCurrAnim;
	Anim* mAnims[GOOMBA_ANIM_NUM];

	int mState;

	HGE* hge;

	HTEXTURE mTex;
	hgeQuad mQuad;

	float mDyingTime;
	bool mGrounded;

	const float mAccelX;
	const float mGravAccelY;
	const float mJumpAccelY;

	const float mDieTime;

	Vector2 mInitPos;//for save files
	int mInitHead;

	bool active;//was inside window for once
};

