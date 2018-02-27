#include "Turtle.h"
#include "Game.h"
#include "TileMap.h"
#include "Tile.h"
#include "Goomba.h"
#include "Piranha.h"

#include <math.h>

#define SGN(x) (((x)==0)?(0):(((x)<0)?(-1):(1)))
#define MIN(x,y) ((x>=y)?y:x)
#define MAX(x,y) ((x>=y)?x:y)


void Turtle::__load()
{
	hge=Game::getInstance()->hge;
	mTex=hge->Texture_Load("data//enemies.png");

	mAnims[WALK]=new Anim(AnimDesc(16,24,11,6,7,8,true,6,0,8));
	mAnims[SHELL]=new Anim(AnimDesc(16,16,11,10,10,8,false,10,0,16));
	mAnims[WAKING]=new Anim(AnimDesc(16,16,12,11,11,8,false,11,0,16));
	
	mCurrAnim=WALK;

	mPos = mInitPos;
	mVel=Vector2(mInitHead*mAccelX,0);

	mState=WALK;

	mDimY=48;

	mWakingTimer=0;
	mSleepTimer=0;
	mWakingCounter=0;

	mActive = false;
}

Turtle::Turtle(int head):mAccelX(2),mShellAccelX(8),mGravAccelY(0.4875),mJumpAccelY(3.5),mSleepTime(2),mWakingTime(0.1)
{
	hge=Game::getInstance()->hge;
	mTex=hge->Texture_Load("data//enemies.png");

	mAnims[WALK]=new Anim(AnimDesc(16,24,11,6,7,8,true,6,0,8));
	mAnims[SHELL]=new Anim(AnimDesc(16,16,11,10,10,8,false,10,0,16));
	mAnims[WAKING]=new Anim(AnimDesc(16,16,12,11,11,8,false,11,0,16));

	mCurrAnim=WALK;

	mPos=Vector2(100,50);

	mVel=Vector2(head*mAccelX,0);

	mState=WALK;

	mDimY=48;

	mWakingTimer=0;
	mSleepTimer=0;
	mWakingCounter=0;

	mInitHead = head;

	mActive = false;
}


Turtle::~Turtle(void)
{
	hge->Texture_Free(mTex);
	for(int i=0;i<TURTLE_ANIM_NUM;i++)
	{
		delete mAnims[i];
	}
}

void Turtle::update()
{
	if(!mActive && Game::getInstance()->mMap->isVisible(mPos))
		mActive= true;

	if(mState==DEAD || !mActive)
		return;

	if(mState==SHELL)
	{
		if(mVel.x==0 && mVel.y==0)
		{
			if(mSleepTimer>mSleepTime)
			{
				mSleepTimer=0;
				mState=WAKING;
			}
			mSleepTimer+=Game::getInstance()->hge->Timer_GetDelta();
		}
		else
		{
			mSleepTimer=0;
		}
	}
	else if(mState==WAKING)
	{
		if(mWakingTimer>mWakingTime)
		{
			mWakingTimer=0;

			if(mCurrAnim==SHELL)
			{
				mCurrAnim=WAKING;
				mWakingCounter++;
			}
			else if(mCurrAnim==WAKING)
				mCurrAnim=SHELL;

			if(mWakingCounter==10)
			{
				mVel.x=mAccelX;
				mState=WALK;mDimY=48;
				mPos.y-=16;//up a little bit..the walking turtle is a little bit tall
				mWakingCounter=0;
			}
		}

		mWakingTimer+=Game::getInstance()->hge->Timer_GetDelta();
		return;
	}

	if(!mGrounded)
	{
		mVel.y+=mGravAccelY;
	}

	mPos+=mVel;

	mGrounded=false;
	if(mState!=INVERT)
	{
		handleWorldCollision();
		handleEnemiesCollision();
	}

	if(mState==INVERT)
	{
		//test out_of_world_limits//TSLIMIT
		return;
	}

	//test out_of_world_limits//TSLIMIT
	mCurrAnim=mState;
	mAnims[mCurrAnim]->update(hge->Timer_GetDelta());
}
void Turtle::render()
{
	TileMap* map=Game::getInstance()->mMap;
	float x,y,w,h;
	

	if(mState==INVERT)
	{
		mAnims[SHELL]->getClip(&x,&y,&w,&h);
		y=y+h;
		h=-h;
	}
	else
	{
		mAnims[mCurrAnim]->getClip(&x,&y,&w,&h);
	}

	if(mVel.x>0)
	{
		x=x+w;
		w=-w;
	}

	mQuad.v[0].x=mPos.x-map->mxScroll;
	mQuad.v[0].y=mPos.y-map->myScroll;
	mQuad.v[0].tx=x/hge->Texture_GetWidth(mTex);
	mQuad.v[0].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[1].x=mPos.x+32-map->mxScroll;
	mQuad.v[1].y=mPos.y-map->myScroll;
	mQuad.v[1].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[1].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[2].x=mPos.x+32-map->mxScroll;
	mQuad.v[2].y=mPos.y+mDimY-map->myScroll;
	mQuad.v[2].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[2].ty=(y+h)/hge->Texture_GetHeight(mTex);

	mQuad.v[3].x=mPos.x-map->mxScroll;
	mQuad.v[3].y=mPos.y+mDimY-map->myScroll;
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

void Turtle::handleWorldCollision()
{
	TileMap* map=Game::getInstance()->mMap;
	int mariox=map->xWorldToMap(mPos.x),marioy=map->yWorldToMap(mPos.y);

	int tl=0,tr=0,bl=0,br=0,ml=0,mr=0;

	if(mariox>=map->mWidth || marioy>=map->mHeight || mariox<0 || marioy<0)
		return;
	if(mState==WALK)
	{
		tl=map->isTile(mariox,marioy,IMPENETRABLE);
		if(mariox+1<map->mWidth && marioy<map->mHeight)
			tr=map->isTile(mariox+1,marioy,IMPENETRABLE);
		if(mariox<map->mWidth && marioy+1<map->mHeight)
			ml=map->isTile(mariox,marioy+1,IMPENETRABLE);
		if(mariox+1<map->mWidth && marioy+1<map->mHeight)
			mr=map->isTile(mariox+1,marioy+1,IMPENETRABLE);
		if(mariox<map->mWidth && marioy+2<map->mHeight)
			bl=map->isTile(mariox,marioy+2,IMPENETRABLE);
		if(mariox+1<map->mWidth && marioy+2<map->mHeight)
			br=map->isTile(mariox+1,marioy+2,IMPENETRABLE);

		if(tl)
		{
			float xoverlap=(mariox+1)*map->mTileWidth-mPos.x;
			float yoverlap=(marioy+1)*map->mTileHeight-mPos.y;
			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth;
				mVel.x=mAccelX;
			}
			else
			{
				mPos.y=(marioy+1)*map->mTileHeight;
				mVel.y=0;
			}
		}
		if(tr)
		{
			//32 is player x dimension
			float xoverlap=mPos.x+32-(mariox+1)*map->mTileWidth;
			float yoverlap=(marioy+1)*map->mTileHeight-mPos.y;


			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth-32;
				mVel.x=-mAccelX;
			}
			else
			{
				mPos.y=(marioy+1)*map->mTileHeight;
				mVel.y=0;
			}
		}
		if(br)
		{
			//16 is player x dimension
			float xoverlap=mPos.x+32-(mariox+1)*map->mTileWidth;
			float yoverlap=mPos.y+mDimY-(marioy+2)*map->mTileHeight;
			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth-32;
				mVel.x=-mAccelX;
			}
			else
			{
				mPos.y=(marioy+2)*map->mTileHeight-mDimY;
				mVel.y=0;
				mGrounded=true;
			}
		}
		if(bl)
		{
			float xoverlap=(mariox+1)*map->mTileWidth-mPos.x;
			float yoverlap=mPos.y+mDimY-(marioy+2)*map->mTileHeight;
			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth;
				mVel.x=mAccelX;
			}
			else
			{
				mPos.y=(marioy+2)*map->mTileHeight-mDimY;
				mVel.y=0;
				mGrounded=true;
			}
		}

		if(mr)
		{
			mPos.x=(mariox+1)*map->mTileWidth-32;
			mVel.x=-mAccelX;
		}

		if(ml)
		{
			mPos.x=(mariox+1)*map->mTileWidth;
			mVel.x=mAccelX;
		}
	}
	else
	{
		tl=map->isTile(mariox,marioy,IMPENETRABLE);
		if(mariox+1<map->mWidth && marioy<map->mHeight)
			tr=map->isTile(mariox+1,marioy,IMPENETRABLE);
		if(mariox<map->mWidth && marioy+1<map->mHeight)
			bl=map->isTile(mariox,marioy+1,IMPENETRABLE);
		if(mariox+1<map->mWidth && marioy+1<map->mHeight)
			br=map->isTile(mariox+1,marioy+1,IMPENETRABLE);

		if(tl)
		{
			float xoverlap=(mariox+1)*map->mTileWidth-mPos.x;
			float yoverlap=(marioy+1)*map->mTileHeight-mPos.y;
			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth;
				mVel.x=(mState==SHELL)?mShellAccelX:mAccelX;
			}
			else
			{
				mPos.y=(marioy+1)*map->mTileHeight;
				mVel.y=0;
			}
		}
		if(tr)
		{
			//32 is player x dimension
			float xoverlap=mPos.x+32-(mariox+1)*map->mTileWidth;
			float yoverlap=(marioy+1)*map->mTileHeight-mPos.y;


			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth-32;
				mVel.x=(mState==SHELL)?-mShellAccelX:-mAccelX;
			}
			else
			{
				mPos.y=(marioy+1)*map->mTileHeight;
				mVel.y=0;
			}
		}
		if(br)
		{
			//16 is player x dimension
			float xoverlap=mPos.x+32-(mariox+1)*map->mTileWidth;
			float yoverlap=mPos.y+32-(marioy+1)*map->mTileHeight;
			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth-32;
				mVel.x=(mState==SHELL)?-mShellAccelX:-mAccelX;
			}
			else
			{
				mPos.y=(marioy+1)*map->mTileHeight-32;
				mVel.y=0;
				mGrounded=true;
			}
		}
		if(bl)
		{
			float xoverlap=(mariox+1)*map->mTileWidth-mPos.x;
			float yoverlap=mPos.y+32-(marioy+1)*map->mTileHeight;
			if(xoverlap<yoverlap)
			{
				mPos.x=(mariox+1)*map->mTileWidth;
				mVel.x=(mState==SHELL)?mShellAccelX:mAccelX;
			}
			else
			{
				mPos.y=(marioy+1)*map->mTileHeight-32;
				mVel.y=0;
				mGrounded=true;
			}
		}
	}

	//collsion against movy tiles
		for(int i=0;i<2;i++)
		{
			if(map->mMovyTiles[i]!=-1)
			{
				int xi = map->mMovyTiles[i]%map->mWidth;
				int yi = map->mMovyTiles[i]/map->mWidth;

				if(!(mPos.x+32<xi*map->mTileWidth || mPos.x>xi*map->mTileWidth+32 
					|| mPos.y>yi*map->mTileHeight-map->mMovyOffsets[i]+32 || mPos.y+32<yi*map->mTileHeight-map->mMovyOffsets[i]))
				{
					mState=INVERT;
					mDimY=32;
					mVel.y=-1.5*mJumpAccelY;
					mVel.x=0;
					mGrounded=false;
				}
			}
		}

}


void Turtle::handleEnemiesCollision()
{
	TileMap* map=Game::getInstance()->mMap;
	for(int i=0;i<map->mNumGoombas;i++)
	{
		Goomba* goomba=map->mGoombas[i];
		if(goomba->mState==Goomba::DYING || goomba->mState==Goomba::DEAD || goomba->mState==Goomba::INVERT)
			continue;
		float a=1.5;
		if(!(mPos.x+32<goomba->mPos.x+a || mPos.x>goomba->mPos.x+32-a || mPos.y>goomba->mPos.y+32-a || mPos.y+mDimY<goomba->mPos.y+a))
		{
			if(mVel.y>0
				&& (mPos.y-mVel.y+32<=goomba->mPos.y+12))//the collision from top..(it uses prec pos; this is why mVel.x is present)
			{
				goomba->mState=Goomba::DYING;
				mVel.y=-2.5*mJumpAccelY;
				mGrounded=false;
				mPos.y=goomba->mPos.y-mDimY - 1;
				hge->Effect_Play(Game::getInstance()->mStompSnd);
			}
			else
			{

				if(mState!=SHELL)
				{
					//resolve collision
					if(mVel.x<0)
					{
						mPos.x=goomba->mPos.x+32;
					}
					else if(mVel.x>0)
					{
						mPos.x=goomba->mPos.x-32;
					}
					mVel.x=-mVel.x;
				}
			}
		}
	}

	//TURTLE

	for(int i=0;i<map->mNumTurtles;i++)
	{
		Turtle* tur=map->mTurtles[i];
		if(tur->mState==Turtle::DEAD || tur->mState==Turtle::INVERT || tur==this)
			continue;
		float a=1;
		if(!(mPos.x+32<tur->mPos.x+a || mPos.x>tur->mPos.x+32-a || mPos.y>tur->mPos.y+tur->mDimY-a || mPos.y+mDimY<tur->mPos.y+a))
		{
			if(mVel.y>0
				&& (mPos.y-mVel.y+32<=tur->mPos.y+12))//the collision from top..(it uses prec pos; this is why mVel.x is present)
			{
				if(tur->mState==Turtle::WALK)
				{
					tur->mState=Turtle::SHELL;
					tur->mDimY=32;
					tur->mVel.x=0;
				}
				else if(tur->mState==Turtle::SHELL)
				{
					if(tur->mVel.x==0)
					{
						if(mPos.x-mVel.x+32>=tur->mPos.x && mPos.x-mVel.x+32<tur->mPos.x+32)//16 half of the turtle
							tur->mVel.x=tur->mShellAccelX;
						else
							tur->mVel.x=-tur->mShellAccelX;


					}
					else
					{
						tur->mVel.x=0;
					}
				}

				mVel.y=-1.5*mJumpAccelY;
				mGrounded=false;
				mPos.y=tur->mPos.y-mDimY - 1;
				hge->Effect_Play(Game::getInstance()->mStompSnd);
			}
			else
			{
				if(tur->mState==WALK)
				{
					if(mState!=SHELL)
					{
						//resolve collision
						if(mVel.x<0)
						{
							mPos.x=tur->mPos.x+32;
						}
						else if(mVel.x>0)
						{
							mPos.x=tur->mPos.x-32;
						}
						mVel.x=-mVel.x;
					}
				}
				else if(tur->mState==Turtle::SHELL)
				{
					if(tur->mVel.x==0)
					{
						if(mPos.x-mVel.x+32<=tur->mPos.x)
							tur->mVel.x=tur->mShellAccelX;
						else if( mPos.x-mVel.x>=tur->mPos.x+32)
							tur->mVel.x=-tur->mShellAccelX;

						mVel.x=-mVel.x;
					}
					else
					{
						mState=INVERT;
						mDimY=32;
						mVel.y=-1.5*mJumpAccelY;
						mVel.x=0;
						mGrounded=false;
						hge->Effect_Play(Game::getInstance()->mStompSnd);
					}
				}	
			}
		}
	}

	//PIRANHA
	for(int i=0;i<map->mNumPiranhas;i++)
	{
		Piranha* pir=map->mPiranhas[i];
		if(pir->mState==Piranha::SLEEP)
			continue;

		//the piranha collision box is a little smaller(4px) than usual--->Fair gameplay
		if(!(mPos.x+32<pir->mPos.x+2 || mPos.x>pir->mPos.x+32-2 || mPos.y>pir->mPos.y+24-2 || mPos.y+mDimY<pir->mPos.y-2))
		{
			mState=INVERT;
			mVel.y=-1.5*mJumpAccelY;
			mVel.x=0;
			mGrounded=false;
		}
	}
}
