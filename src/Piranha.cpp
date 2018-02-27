#include "Piranha.h"
#include "Game.h"
#include "TileMap.h"
#include "Tile.h"


void Piranha::__load()
{
	hge=Game::getInstance()->hge;
	mTex=hge->Texture_Load("data//enemies.png");

	mAnim=new Anim(AnimDesc(16,24,14,12,13,8,true,12,0,8));

	mPos = mInitPos;
	mSymY = mInitPos.y;

	mVel=Vector2(0,0);

	mState=SLEEP;
	mSleepTimer=0;
}

Piranha::Piranha(int head):mAccelY(1.5),mSleepTime(1.5)
{
	hge=Game::getInstance()->hge;
	mTex=hge->Texture_Load("data//enemies.png");

	mAnim=new Anim(AnimDesc(16,24,14,12,13,8,true,12,0,8));

	mPos=Vector2(100,480);
	mSymY=480;
	mVel=Vector2(0,0);

	mState=SLEEP;
	mSleepTimer=0;

}


Piranha::~Piranha(void)
{
	hge->Texture_Free(mTex);
	delete mAnim;
}

void Piranha::update()
{
	if(mState==SLEEP)
	{
		if(mSleepTimer>mSleepTime)
		{
			mSleepTimer=0;
			mState=HAMHAM;
			mVel.y=-mAccelY;
		}

		mSleepTimer+=Game::getInstance()->hge->Timer_GetDelta();
	}
	else
	{
		if(mPos.y+45<=mSymY)
		{
			mPos.y=mSymY-45;
			if(mSleepTimer>mSleepTime)
			{
				mSleepTimer=0;
				mVel.y=mAccelY;
			}
			mSleepTimer+=Game::getInstance()->hge->Timer_GetDelta();

		}
		else if(mPos.y>mSymY)
		{
			mPos.y=mSymY;
			mState=SLEEP;
			mVel.y=0;
		}

		mPos+=mVel;
	}
	
	mAnim->update(hge->Timer_GetDelta());
}

void Piranha::render()
{
	TileMap* map=Game::getInstance()->mMap;
	float x,y,w,h;

	mAnim->getClip(&x,&y,&w,&h);


	mQuad.v[0].x=mPos.x-map->mxScroll;
	mQuad.v[0].y=mPos.y-map->myScroll;
	mQuad.v[0].tx=x/hge->Texture_GetWidth(mTex);
	mQuad.v[0].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[1].x=mPos.x+32-map->mxScroll;
	mQuad.v[1].y=mPos.y-map->myScroll;
	mQuad.v[1].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[1].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[2].x=mPos.x+32-map->mxScroll;
	mQuad.v[2].y=mPos.y+48-map->myScroll;
	mQuad.v[2].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[2].ty=(y+h)/hge->Texture_GetHeight(mTex);

	mQuad.v[3].x=mPos.x-map->mxScroll;
	mQuad.v[3].y=mPos.y+48-map->myScroll;
	mQuad.v[3].tx=x/hge->Texture_GetWidth(mTex);
	mQuad.v[3].ty=(y+h)/hge->Texture_GetHeight(mTex);

	mQuad.blend=BLEND_DEFAULT;
	mQuad.tex=mTex;
	for(int k=0;k<4;k++)
	{
		mQuad.v[k].z=0.5f;
	}
	mQuad.v[0].col = mQuad.v[1].col = mQuad.v[2].col = mQuad.v[3].col = 0xffffffff;

	hge->Gfx_RenderQuad(&mQuad);
}
