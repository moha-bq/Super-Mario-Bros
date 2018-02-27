#pragma once
#include <hge.h>

#include "Vector2.h"
#include "AnimDesc.h"
#include "Anim.h"
#include "AABB.h"

#define MARIO_ANIM_NUM 8

class Mario
{
public:

	enum {WALK=0,IDLE,SKID,JUMP,DYING,SWIM,B_DOWN,FLAGPOLE,DEAD,MUTATE,END_FLAGDOWN_LEFT,END_FLAGDOWN_RIGHT,END_WALK,DISAPEAR,DISAPEARING};//@FUCK, Anim and logic state shoudn't be linked
	enum {SMALL=0,BIG,FIRE};
public:
	Mario(void);
	~Mario(void);

	void init();
	void render();
	void update();

	void move(Vector2 d);
	void move(float dx,float dy);
	void impulse(Vector2 i);
	void impulse(float ix,float iy);

	void handleWorldCollision();
	void handleMovement();
	void handleEnemiesCollision();
	void handleItemsCollision();

	void updateState();
	void formUpdates();
	void handleSpecialTiles();

	Vector2 mPos;
	Vector2 mVel;//frame displacement

	AABB mAABB;

	bool mGrounded;

	int mCurrAnim;
	Anim* mAnims[MARIO_ANIM_NUM];

	HTEXTURE mTex;
	hgeQuad mQuad;

	HGE* hge;

	int mState;
	int mLeftHeaded;//1 if left

	int mForm;
	int mMutateTo;

	float mMutationTimer; 

	int mDimY;
	bool dead;
	bool dying;

	bool god;
	bool mutating;

	int lives;

	const float mAccelX;
	const float mAirAccelX;
	const float mDecelX;
	const float mAirDecelX;
	const float mFricX;
	const float mMaxVelX;
	const float mMaxVelY;
	const float mBreakVelX;
	const float mDieAccelY;
	const float mJumpAccelY;
	const float mGravAccelY;
	const float mMaxJumpTime;
	const float mMutationTime;

	float mJumpTime;

	int mCoins;//number of coins !
};

