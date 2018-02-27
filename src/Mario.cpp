#include "Mario.h"
#include "Game.h"
#include "TileMap.h"
#include "Tile.h"
#include "Goomba.h"
#include "Turtle.h"
#include "Piranha.h"
#include "Mushroom.h"

#include <math.h>

#define SGN(x) (((x)==0)?(0):(((x)<0)?(-1):(1)))
#define MIN(x,y) ((x>=y)?y:x)
#define MAX(x,y) ((x>=y)?x:y)
#define PI 3.14159f

Mario::Mario(void):mAccelX(0.2),mDecelX(0.3),mFricX(0.2),mMaxVelX(6),mBreakVelX(4.5),mAirAccelX(0.09375),
	mAirDecelX(0.09375),mDieAccelY(-10),mJumpAccelY(3.5),mGravAccelY(0.4875),mMaxVelY(16),mMaxJumpTime(0.25),mMutationTime(0.6)
{
	hge=Game::getInstance()->hge;
	mTex=Game::getInstance()->mMarioTex;

	mAnims[WALK]=new Anim(AnimDesc(16,16,14,0,2,8,true,0,0,32));
	mAnims[IDLE]=new Anim(AnimDesc(16,16,14,6,6,8,false,6,0,32));
	mAnims[SKID]=new Anim(AnimDesc(16,16,14,3,3,8,false,3,0,32));
	mAnims[JUMP]=new Anim(AnimDesc(16,16,14,4,4,8,false,4,0,32));
	mAnims[DYING]=new Anim(AnimDesc(16,16,14,5,5,8,false,5,0,32));
	mAnims[SWIM]=new Anim(AnimDesc(16,16,14,9,13,8,true,9,0,32));
	mAnims[B_DOWN]=new Anim(AnimDesc(16,32,15,5,5,8,false,5,0,0));
	mAnims[FLAGPOLE]=new Anim(AnimDesc(16,16,14,8,8,8,false,8,0,32));
	mCurrAnim=IDLE;

	mPos=Vector2(100,320);
	mVel=Vector2(0,0);
	mGrounded=false;

	mState=IDLE;

	mJumpTime=0;

	mCoins=0;

	mAABB.mCenter=Vector2(8,8);
	mAABB.mHalf=Vector2(8,8);

	mForm=SMALL;
	formUpdates();

	mMutationTimer = 0;
	dead = false;
	dying = false;

	god=false;
	mutating =false;
	lives=3;
}

void Mario::init()
{
	mCurrAnim=IDLE;
	mPos.x = 32+32+16;
	mPos.y = 9*32 + 12*32;
	mVel=Vector2(0,0);
	mGrounded=false;

	mState=IDLE;

	mJumpTime=0;

	mCoins=0;

	mForm=SMALL;
	formUpdates();

	mMutationTimer = 0;
	dead = false;
	dying = false;

	mLeftHeaded = false;

	god=false;
	mutating =false;

	lives=3;
}

Mario::~Mario(void)
{
	hge->Texture_Free(mTex);

	for(int i=0;i<MARIO_ANIM_NUM;i++)
	{
		delete mAnims[i];
	}
}

void Mario::update()
{
	if(mState == DISAPEAR || dead)
		return;

	if(dying)
	{
		mForm = SMALL;
		formUpdates();

		mVel.x=0;
		mCurrAnim=DYING;
		mAnims[mCurrAnim]->update(hge->Timer_GetDelta());
		mVel.y+=mGravAccelY;
		mPos+=mVel;

		if(mPos.y>Game::getInstance()->mMap->myScroll+16*32)
		{
			static int timer = 0;timer++;
			if(timer>128)
			{
				timer=0;
				dead = true;
			}
		}
		return;
	}

	if(Game::getInstance()->mLevelComplete)
	{
		TileMap *map = Game::getInstance()->mMap;
		if(mState!=END_FLAGDOWN_RIGHT && mState!=END_WALK && mState!=DISAPEARING)
		{
			mState=END_FLAGDOWN_LEFT;
			mCurrAnim = FLAGPOLE;
			mVel.x=0;
			mVel.y=2.3;
			float end = map->myScroll + 384 - 32;
			if(mForm==BIG)
				end-=32;

			if(mPos.y>=end)
			{
				mPos.y=end;
				mState=END_FLAGDOWN_RIGHT;
				mVel.y=0;
				mPos.x+=22;
			}
		}
		if(mState==END_FLAGDOWN_RIGHT)
		{
			//hack timing
			mCurrAnim = FLAGPOLE;
			static int frame_counter = 0;
			frame_counter++;
			if(frame_counter>24)
			{
				mState=END_WALK;
				frame_counter = 0;
				mPos.y+=32;
				hge->Channel_StopAll();
				hge->Effect_Play(Game::getInstance()->mLevelClearSnd);
			}
		}
		if(mState==END_WALK || mState==DISAPEARING )
		{
			mVel.x=mMaxVelX;
			mAnims[WALK]->mDesc.mFreq=16;
			mCurrAnim = WALK;

			if(mPos.x>=map->mxCastle * map->mTileWidth)
			{
				mState = DISAPEARING;
				mVel.x=0;
			}
			static int timer = 0;timer++;
			if(timer>380)
			{
				timer=0;
				Game::getInstance()->mMode=LEVEL_COMPLETE; 
				Game::getInstance()->credity=SCREENH+SCREENH*0.1;
				Game::getInstance()->music_channel=hge->Stream_Play(Game::getInstance()->mMusic,true,50);
			}
		}

		mAnims[mCurrAnim]->update(hge->Timer_GetDelta());
		mPos+=mVel;
		return;
	}

	if(mutating)
	{
		mMutationTimer+=Game::getInstance()->hge->Timer_GetDelta();

		if((int)(mMutationTimer/mMutationTime*12)%2)//oscillate between the two forms
			mForm = BIG;
		else
			mForm = SMALL;

		formUpdates();

		if(mMutationTimer>mMutationTime)
		{
			mForm = mMutateTo;
			formUpdates();
			mMutationTimer = 0;
			mState = WALK;
			mutating = false;
		}
	}
	else
	{
		handleMovement();

		if(!mGrounded)
		{
			//mVel.y+=mGravAccelY;
		}
		mVel.y+=mGravAccelY;

		//mPos+=mVel;

		mGrounded=false;

		handleSpecialTiles();
		handleWorldCollision();
		handleEnemiesCollision(); 
		updateState();

		mCurrAnim=mState;
		mAnims[mCurrAnim]->update(hge->Timer_GetDelta());

		if(god)
		{
			static int god_timer=0;
			god_timer++;
			if(god_timer>100)
			{
				god_timer=0;
				god=false;
			}
		}
	}

	handleItemsCollision();

	if(mPos.x<0)
	{
		mPos.x=0;mVel.x=0;
	}

	if(mPos.y-9*32>16*32 || Game::getInstance()->mGameTime==Game::getInstance()->mTimeLimit)
	{
		mVel.y=-3.5*mJumpAccelY;
		mGrounded=false;
		hge->Channel_StopAll();
		hge->Effect_Play(Game::getInstance()->mDieSnd);
		mState=DYING;
		dying = true;
		Game::getInstance()->mStopGameTime = true;
	}
}

void Mario::render()
{
	if(mState == DISAPEAR || dead || mState == DISAPEARING)
		return;

	TileMap* map=Game::getInstance()->mMap;
	float x,y,w,h;
	mAnims[mCurrAnim]->getClip(&x,&y,&w,&h);
	//w=hge->Texture_GetWidth(mTex);h=hge->Texture_GetHeight(mTex);
	//x=0;y=0;

	if(mLeftHeaded || mState==END_FLAGDOWN_RIGHT)
	{
		//left headed; made mario head left!
		x=x+w;
		y=y;
		w=-w;
		h=h;
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

	mQuad.blend=BLEND_ALPHABLEND;
	mQuad.tex=mTex;
	for(int k=0;k<4;k++)
	{
		mQuad.v[k].z=0.5f;
	}
	if(!god)
	{
		mQuad.v[0].col = mQuad.v[1].col = mQuad.v[2].col = mQuad.v[3].col = 0xffffffff;
	}
	else
	{
		mQuad.v[0].col = mQuad.v[1].col = mQuad.v[2].col = mQuad.v[3].col = 0xaeffffff;
	}

	hge->Gfx_RenderQuad(&mQuad);
}

void Mario::handleWorldCollision()
{
	Game* game=Game::getInstance();
	TileMap* map=game->mMap;

	int mariox,marioy,mariox_end,marioy_end;
	int tl=0,tr=0,bl=0,br=0,ml=0,mr=0;



	mPos.x+=mVel.x;

	mariox=map->xWorldToMap(mPos.x);marioy=map->yWorldToMap(mPos.y);
	mariox_end=map->xWorldToMap(mPos.x+32);marioy_end=map->yWorldToMap(mPos.y+mDimY-1);

	if(mariox>=map->mWidth || marioy_end>=map->mHeight || mariox<0 || marioy<0)
		return;

	for(int i=mariox;i<=mariox_end;i++)
	{
		for(int j=marioy;j<=marioy_end;j++)
		{
			if(map->isTile(i,j,IMPENETRABLE))
			{
				if(mVel.x>0)
				{
					mPos.x=i*map->mTileWidth-32;
					mVel.x=0;
				}
				if(mVel.x<0)
				{
					mPos.x=(i+1)*map->mTileWidth;
					mVel.x=0;
				}
			}
		}
	}

	mPos.y+=mVel.y;

	mariox=map->xWorldToMap(mPos.x);marioy=map->yWorldToMap(mPos.y);
	mariox_end=map->xWorldToMap(mPos.x+31);marioy_end=map->yWorldToMap(mPos.y+mDimY);

	if(mariox>=map->mWidth || marioy_end>=map->mHeight || mariox<0 || marioy<0)
		return;

	//handle movy tiles

	for(int i=mariox;i<=mariox_end;i++)
	{
		for(int j=marioy;j<=marioy_end;j++)
		{
			if(map->isTile(i,j,MOVY))
			{
				if(mVel.y<0)
				{
					//move the tile in function of its neighbours and the position of touch!
					if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) && i+1<map->mWidth && map->isTile(i+1,j,IMPENETRABLE) )
					{
						if((mPos.x+32<(i+1)*map->mTileWidth && mPos.x+32>(i+0.5)*map->mTileWidth) ||( mPos.x<(i+0.5)*map->mTileWidth && mPos.x>i*map->mTileWidth))
						{
							map->mMovyTiles[map->mMovyId]=j*map->mWidth+i;
							map->mMovyVel[map->mMovyId]=3;
							map->mMovyId=!map->mMovyId;
							if(mForm == SMALL || !map->isTile(i,j,BREAKABLE))
							{
								hge->Effect_Play(Game::getInstance()->mBumpSnd);

							}
							else if(map->isTile(i,j,BREAKABLE))
							{
								map->addTile(i,j,-1);

								map->addDebris(Vector2(i*map->mTileWidth,j*map->mTileHeight));

								hge->Effect_Play(Game::getInstance()->mBreakBlockSnd);
							}
						}
					}
					else if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) )
					{
						if(mPos.x+32>(i+0.5)*map->mTileWidth && mPos.x<(i+1)*map->mTileWidth)
						{map->mMovyTiles[map->mMovyId]=j*map->mWidth+i;
						map->mMovyVel[map->mMovyId]=3;
						map->mMovyId=!map->mMovyId;
						if(mForm == SMALL|| !map->isTile(i,j,BREAKABLE))
						{

							hge->Effect_Play(Game::getInstance()->mBumpSnd);
						}
						else if(map->isTile(i,j,BREAKABLE))
						{
							map->addTile(i,j,-1);
							map->addDebris(Vector2(i*map->mTileWidth,j*map->mTileHeight));
							hge->Effect_Play(Game::getInstance()->mBreakBlockSnd);
						}
						}
					}
					else if(i+1<map->mWidth &&  map->isTile(i+1,j,IMPENETRABLE))
					{
						if(mPos.x+32>(i)*map->mTileWidth && mPos.x<(i+0.5)*map->mTileWidth)
						{map->mMovyTiles[map->mMovyId]=j*map->mWidth+i;
						map->mMovyVel[map->mMovyId]=3;
						map->mMovyId=!map->mMovyId;
						if(mForm == SMALL|| !map->isTile(i,j,BREAKABLE))
						{

							hge->Effect_Play(Game::getInstance()->mBumpSnd);
						}
						else if(map->isTile(i,j,BREAKABLE))
						{
							map->addTile(i,j,-1);
							map->addDebris(Vector2(i*map->mTileWidth,j*map->mTileHeight));
							hge->Effect_Play(Game::getInstance()->mBreakBlockSnd);
						}
						}
					}
					else
					{	map->mMovyTiles[map->mMovyId]=j*map->mWidth+i;
					map->mMovyVel[map->mMovyId]=3;
					map->mMovyId=!map->mMovyId;
					if(mForm == SMALL|| !map->isTile(i,j,BREAKABLE))
					{

						hge->Effect_Play(Game::getInstance()->mBumpSnd);
					}
					else if(map->isTile(i,j,BREAKABLE))
					{
						map->addTile(i,j,-1);
						map->addDebris(Vector2(i*map->mTileWidth,j*map->mTileHeight));
						hge->Effect_Play(Game::getInstance()->mBreakBlockSnd);
					}
					}
				}
			}

			if(map->isTile(i,j,GENEROUS))
			{
				if(mVel.y<0)
				{
					//move the tile in function of its neighbours and the position of touch!
					if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) && i+1<map->mWidth && map->isTile(i+1,j,IMPENETRABLE) )
					{
						if((mPos.x+32<(i+1)*map->mTileWidth && mPos.x+32>(i+0.5)*map->mTileWidth) ||( mPos.x<(i+0.5)*map->mTileWidth && mPos.x>i*map->mTileWidth))
						{
							map->addFlyingCoin(Vector2(i*map->mTileWidth,(j-1)*map->mTileHeight));
						}
					}
					else if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) )
					{
						if(mPos.x+32>(i+0.5)*map->mTileWidth && mPos.x<(i+1)*map->mTileWidth)
						{
							map->addFlyingCoin(Vector2(i*map->mTileWidth,(j-1)*map->mTileHeight));
						}
					}
					else if(i+1<map->mWidth &&  map->isTile(i+1,j,IMPENETRABLE))
					{
						if(mPos.x+32>(i)*map->mTileWidth && mPos.x<(i+0.5)*map->mTileWidth)
						{
							map->addFlyingCoin(Vector2(i*map->mTileWidth,(j-1)*map->mTileHeight));
						}
					}
					else
					{
						map->addFlyingCoin(Vector2(i*map->mTileWidth,(j-1)*map->mTileHeight));

					}
				}
			}

			if(map->getTile(i,j)==24)//24= question tile
			{
				if(mVel.y<0)
				{
					int mushroom_tile = 0;
					for(int k=0;k<map->mNumRedMushroomTiles;k++)
					{
						if(map->mRedMushroomTiles[k]==j*map->mWidth+i)
						{
							mushroom_tile = 1;
							break;
						}
					}

					if(mushroom_tile)
					{
						//move the tile in function of its neighbours and the position of touch!
						if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) && i+1<map->mWidth && map->isTile(i+1,j,IMPENETRABLE) )
						{
							if((mPos.x+32<(i+1)*map->mTileWidth && mPos.x+32>(i+0.5)*map->mTileWidth) ||( mPos.x<(i+0.5)*map->mTileWidth && mPos.x>i*map->mTileWidth))
							{
								map->addTile(i,j,27);
								map->addRedMushroom(Vector2(i*map->mTileWidth, j*map->mTileHeight));
								hge->Effect_Play(Game::getInstance()->mPowerUpShowSnd);
							}
						}
						else if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) )
						{
							if(mPos.x+32>(i+0.5)*map->mTileWidth && mPos.x<(i+1)*map->mTileWidth)
							{
								map->addTile(i,j,27);
								map->addRedMushroom(Vector2(i*map->mTileWidth, j*map->mTileHeight));
								hge->Effect_Play(Game::getInstance()->mPowerUpShowSnd);
							}
						}
						else if(i+1<map->mWidth &&  map->isTile(i+1,j,IMPENETRABLE))
						{
							if(mPos.x+32>(i)*map->mTileWidth && mPos.x<(i+0.5)*map->mTileWidth)
							{
								map->addTile(i,j,27);
								map->addRedMushroom(Vector2(i*map->mTileWidth, j*map->mTileHeight));
								hge->Effect_Play(Game::getInstance()->mPowerUpShowSnd);
							}
						}
						else
						{
							map->addTile(i,j,27);
							map->addRedMushroom(Vector2(i*map->mTileWidth, j*map->mTileHeight));
							hge->Effect_Play(Game::getInstance()->mPowerUpShowSnd);
						}
					}
					else
					{
						//move the tile in function of its neighbours and the position of touch!
						if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) && i+1<map->mWidth && map->isTile(i+1,j,IMPENETRABLE) )
						{
							if((mPos.x+32<(i+1)*map->mTileWidth && mPos.x+32>(i+0.5)*map->mTileWidth) ||( mPos.x<(i+0.5)*map->mTileWidth && mPos.x>i*map->mTileWidth))
							{
								map->addTile(i,j,27);
								map->addFlyingCoin(Vector2(i*map->mTileWidth,(j)*map->mTileHeight));
								hge->Effect_Play(Game::getInstance()->mCoinSnd);

							}
						}
						else if(i-1>=0 && map->isTile(i-1,j,IMPENETRABLE) )
						{
							if(mPos.x+32>(i+0.5)*map->mTileWidth && mPos.x<(i+1)*map->mTileWidth)
							{
								map->addTile(i,j,27);
								map->addFlyingCoin(Vector2(i*map->mTileWidth,(j)*map->mTileHeight));
								hge->Effect_Play(Game::getInstance()->mCoinSnd);
							}
						}
						else if(i+1<map->mWidth &&  map->isTile(i+1,j,IMPENETRABLE))
						{
							if(mPos.x+32>(i)*map->mTileWidth && mPos.x<(i+0.5)*map->mTileWidth)
							{
								map->addTile(i,j,27);
								map->addFlyingCoin(Vector2(i*map->mTileWidth,(j)*map->mTileHeight));
								hge->Effect_Play(Game::getInstance()->mCoinSnd);
							}
						}
						else
						{
							map->addTile(i,j,27);
							map->addFlyingCoin(Vector2(i*map->mTileWidth,(j)*map->mTileHeight));
							hge->Effect_Play(Game::getInstance()->mCoinSnd);
						}
					}

				}
			}

		}
	}

	//solid tiles
	for(int i=mariox;i<=mariox_end;i++)
	{
		for(int j=marioy;j<=marioy_end;j++)
		{

			if(map->isTile(i,j,IMPENETRABLE))
			{
				if(mVel.y>0)
				{
					mPos.y=j*map->mTileHeight-mDimY;
					mGrounded=true;
					mVel.y=0;
				}
				if(mVel.y<0)
				{
					if(mPos.x+32<i*map->mTileWidth+6 && i-1>=0 && !map->isTile(i,j,IMPENETRABLE) )
					{
						mPos.x=i*map->mTileWidth-32;
						continue;
					}
					else if(mPos.x>(i+1)*map->mTileWidth-6 && i+1<map->mWidth && !map->isTile(i,j,IMPENETRABLE) )
					{
						mPos.x=(i+1)*map->mTileWidth;
						continue;
					}
					else
					{
						mPos.y=(j+1)*map->mTileHeight;
						mVel.y=0;
					}
				}
			}
		}
	}
}

void Mario::handleSpecialTiles()
{
	Game* game=Game::getInstance();
	TileMap* map=Game::getInstance()->mMap;
	int mariox=map->xWorldToMap(mPos.x),marioy=map->yWorldToMap(mPos.y);
	int mariox_end=map->xWorldToMap(mPos.x+31),marioy_end=map->yWorldToMap(mPos.y+mDimY-1);

	int tl=0,tr=0,bl=0,br=0,ml=0,mr=0;

	if(mariox>=map->mWidth || marioy_end>=map->mHeight || mariox<0 || marioy<0)
		return;
	for(int i=mariox;i<=mariox_end;i++)
	{
		for(int j=marioy;j<=marioy_end;j++)
		{
			if(map->isTile(i,j,COINY))
			{
				mCoins++;
				map->addTile(i,j,-1);
				Game::getInstance()->addScoreDisplay(mPos.x-map->mxScroll,mPos.y-map->myScroll,200);
				map->mScore+=200;
			}
			if(map->isTile(i,j,POLE) && !Game::getInstance()->mLevelComplete)
			{
				Game::getInstance()->mStopGameTime = true;


				int s=(int)(32*16-mPos.y+map->myScroll)%16;s*=100;
				//if(s>1000)s*=1.5f;
				s+=(Game::getInstance()->mTimeLimit - Game::getInstance()->mGameTime)*15;
				Game::getInstance()->mStopGameTime=true;
				Game::getInstance()->addScoreDisplay(mPos.x-map->mxScroll,mPos.y-map->myScroll,s);
				map->mScore+=s;

				Game::getInstance()->mLevelComplete = true;
				mPos.x+=6;

				hge->Channel_StopAll();
				hge->Effect_Play(Game::getInstance()->mFlagPoleSnd);
			}
		}
	}
}

void Mario::handleItemsCollision()
{
	TileMap* map=Game::getInstance()->mMap;

	//RED_MUSHROOMS
	for(int i=0;i<map->mNumRedMushrooms;i++)
	{
		Mushroom* mush=map->mRedMushrooms[i];
		if(mush->mEaten || mush->mState==Mushroom::SHOWUP)
			continue;
		if(!(mPos.x+32<mush->mPos.x || mPos.x>mush->mPos.x+32 || mPos.y>mush->mPos.y+32 || mPos.y+mDimY<mush->mPos.y))
		{
			//STATE CHANGE
			mMutateTo = BIG;//handle fire mutation;
			mutating =true;
			mush->mEaten = true;
			if(mGrounded)
				mPos.y-=32;
			hge->Effect_Play(Game::getInstance()->mPowerUpSnd);
			Game::getInstance()->addScoreDisplay(mPos.x-map->mxScroll,mPos.y-map->myScroll,1000);
			map->mScore+=1000;
		}
	}
}

void Mario::handleEnemiesCollision()
{
	TileMap* map=Game::getInstance()->mMap;

	//GOOMBA

	for(int i=0;i<map->mNumGoombas;i++)
	{
		Goomba* goomba=map->mGoombas[i];
		if(goomba->mState==Goomba::DYING || goomba->mState==Goomba::DEAD || goomba->mState==Goomba::INVERT)
			continue;

		float a=1.5;
		if(!(mPos.x+32<goomba->mPos.x+a || mPos.x>goomba->mPos.x+32-a || mPos.y>goomba->mPos.y+32-a || mPos.y+mDimY<goomba->mPos.y+a))
		{
			if(mVel.y>=0 
				&& (mPos.y-mVel.y+mDimY<=goomba->mPos.y+12+a) )//the collision from top..(it uses prec pos; this is why mVel.x is present)
			{
				goomba->mState=Goomba::DYING;
				mVel.y=-1.5*mJumpAccelY;
				mState=JUMP;
				mGrounded=false;
				mJumpTime=mMaxJumpTime;
				hge->Effect_Play(Game::getInstance()->mStompSnd);
				Game::getInstance()->addScoreDisplay(goomba->mPos.x-map->mxScroll,goomba->mPos.y-map->myScroll,100);
				map->mScore+=100;
				mPos.y=goomba->mPos.y-mDimY - 1;

			}
			else
			{

				//player should be dead and maybe the goomba jump!//PDEAD
				if(!god)
				{
					Game::getInstance()->mStopGameTime = true;
					if(mForm==SMALL)
					{
						mVel.y=-3.5*mJumpAccelY;
						mGrounded=false;
						hge->Channel_StopAll();
						hge->Effect_Play(Game::getInstance()->mDieSnd);
						mState=DYING;
						dying = true;
					}
					else
					{
						//STATE CHANGE
						mMutateTo = SMALL;//handle fire mutation;
						hge->Effect_Play(Game::getInstance()->mPipeSnd);
						mutating = true;
						god=true;
						hge->Effect_Play(Game::getInstance()->mPipeSnd);
					}}

			}
		}
	}

	//TURTLE

	for(int i=0;i<map->mNumTurtles;i++)
	{
		Turtle* tur=map->mTurtles[i];
		if(tur->mState==Turtle::DEAD || tur->mState==Turtle::INVERT)
			continue;

		float a=1.5;
		if(!(mPos.x+32<tur->mPos.x+a || mPos.x>tur->mPos.x+32-a || mPos.y>tur->mPos.y+tur->mDimY-a || mPos.y+mDimY<tur->mPos.y+a))
		{
			if(mVel.y>=0
				&& (mPos.y-mVel.y+mDimY<=tur->mPos.y+12+a) )//the collision from top..(it uses prec pos; this is why mVel.x is present)
			{
				if(tur->mState==Turtle::WALK)
				{
					tur->mState=Turtle::SHELL;
					tur->mDimY=32;
					tur->mVel.x=0;
					hge->Effect_Play(Game::getInstance()->mStompSnd);
					Game::getInstance()->addScoreDisplay(tur->mPos.x-map->mxScroll,tur->mPos.y-map->myScroll,100);
					map->mScore+=100;
				}
				else if(tur->mState==Turtle::SHELL)
				{
					hge->Effect_Play(Game::getInstance()->mKickSnd);
					Game::getInstance()->addScoreDisplay(tur->mPos.x-map->mxScroll,tur->mPos.y-map->myScroll,400);
					map->mScore+=400;

					if(fabs(tur->mVel.x)<=1e-4)
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
				mState=JUMP;
				mGrounded=false;
				mJumpTime=mMaxJumpTime;
				mPos.y=tur->mPos.y-mDimY - 1;
			}
			else
			{
				if(tur->mState==WALK)
				{
					//player should be dead;//PDEAD
					if(!god)
					{
					Game::getInstance()->mStopGameTime = true;

						if(mForm==SMALL)
					{
						mVel.y=-3.5*mJumpAccelY;
						mGrounded=false;
						hge->Channel_StopAll();
						hge->Effect_Play(Game::getInstance()->mDieSnd);
						mState=DYING;
						dying = true;
					}
					else
					{
						//STATE CHANGE
						mMutateTo = SMALL;//handle fire mutation;
						mutating = true;
						god=true;
						hge->Effect_Play(Game::getInstance()->mPipeSnd);
					}
					}
				}
				else if(tur->mState==Turtle::SHELL)
				{
					if(tur->mVel.x==0)
					{
						hge->Effect_Play(Game::getInstance()->mKickSnd);
						Game::getInstance()->addScoreDisplay(tur->mPos.x-map->mxScroll,tur->mPos.y-map->myScroll,400);
						map->mScore+=400;
						if(mPos.x-mVel.x+32<=tur->mPos.x)
							tur->mVel.x=tur->mShellAccelX;
						else if( mPos.x-mVel.x>=tur->mPos.x+32)
							tur->mVel.x=-tur->mShellAccelX;
					}
					else
					{
						//player should be dead;//PDEAD
						if(!god)
						{
					Game::getInstance()->mStopGameTime = true;
							if(mForm==SMALL)
							{
								mVel.y=-3.5*mJumpAccelY;
								mGrounded=false;
								hge->Channel_StopAll();
								hge->Effect_Play(Game::getInstance()->mDieSnd);
								mState=DYING;
								dying = true;
							}
							else
							{
								//STATE CHANGE
								mMutateTo = SMALL;//handle fire mutation;
								mutating = true;
								god=true;
								hge->Effect_Play(Game::getInstance()->mPipeSnd);
							}
						}
					}
				}	
			}
		}
	}

	//PIRANHAS

	for(int i=0;i<map->mNumPiranhas;i++)
	{
		Piranha* pir=map->mPiranhas[i];
		if(pir->mState==Piranha::SLEEP)
			continue;

		//the piranha collision box is a little smaller(4px) than usual--->Fair gameplay
		if(!(mPos.x+32<pir->mPos.x+2 || mPos.x>pir->mPos.x+32-2 || mPos.y>pir->mPos.y+24-2 || mPos.y+mDimY<pir->mPos.y-2))
		{
			//player should be dead;//PDEAD
			if(!god)
			{
					Game::getInstance()->mStopGameTime = true;
				if(mForm==SMALL)
				{
					mVel.y=-3.5*mJumpAccelY;
					mGrounded=false;
					hge->Channel_StopAll();
					hge->Effect_Play(Game::getInstance()->mDieSnd);
					mState=DYING;
					dying = true;
				}
				else
				{
					//STATE CHANGE
					mMutateTo = SMALL;//handle fire mutation;
					mutating = true;
					god=true;
					hge->Effect_Play(Game::getInstance()->mPipeSnd);
				}
			}
		}
	}
}


void Mario::handleMovement()
{
	if(hge->Input_GetKeyState(HGEK_RIGHT))
	{
		mLeftHeaded=0;//don't want to go left
		if(mVel.x<0)
		{
			if(mGrounded)
			{
				mState=SKID;
			}
			else
			{
				mVel.x+=mAirDecelX;
			}
		}
		else
		{
			if(mGrounded)
			{
				mVel.x+=mAccelX;
				mState=WALK;
			}
			else
			{
				mVel.x+=mAirAccelX;
			}
		}
	}
	if(hge->Input_GetKeyState(HGEK_LEFT))
	{
		mLeftHeaded=1;
		if(mVel.x>0)
		{
			if(mGrounded)
			{
				mState=SKID;
			}
			else
			{
				mVel.x-=mAirDecelX;
			}
		}
		else
		{
			if(mGrounded)
			{
				mVel.x-=mAccelX;
				mState=WALK;
			}
			else
			{
				mVel.x-=mAirAccelX;
			}
		}
	}
	if(hge->Input_GetKeyState(HGEK_UP))
	{
		if(mGrounded)
		{
			hge->Effect_Play(Game::getInstance()->mJumpSnd[mForm]);

			mVel.y=-mJumpAccelY;
			mState=JUMP;
			mGrounded=false;
			mJumpTime=0;
		}

		if(mVel.y==0)
		{
			mJumpTime=mMaxJumpTime;
		}

		if(mState==JUMP && mJumpTime<mMaxJumpTime)
		{
			mJumpTime+=hge->Timer_GetDelta();
			mVel.y=-2*mJumpAccelY;
		}
	}
	else
	{
		mJumpTime=mMaxJumpTime;
	}

	if(!hge->Input_GetKeyState(HGEK_LEFT) && !hge->Input_GetKeyState(HGEK_RIGHT))
	{
		mVel.x-=MIN(fabs(mVel.x),mFricX)*SGN(mVel.x);
	}

	if(!hge->Input_GetKeyState(HGEK_LEFT) && !hge->Input_GetKeyState(HGEK_RIGHT) && !hge->Input_GetKeyState(HGEK_UP))
	{
		if(hge->Input_GetKeyState(HGEK_DOWN))
		{
			if(mForm==BIG)
			{
				mState=B_DOWN;
			}
		}
	}

	if(mState==SKID)
	{
		mVel.x-=MIN(fabs(mVel.x),mDecelX)*SGN(mVel.x);
	}

	//rule the player velocity
	if(fabs(mVel.x)>mMaxVelX)
	{
		mVel.x=SGN(mVel.x)*mMaxVelX;
		mAnims[WALK]->mDesc.mFreq=16;
	}
	else
	{
		mAnims[WALK]->mDesc.mFreq=8;
	}

	if(fabs(mVel.y)>mMaxVelY)
		mVel.y=SGN(mVel.y)*mMaxVelY;


	//if(!mGrounded)
	//mState=JUMP;

	/*DEBUG MVT*//*

				 mVel=Vector2(0,0);
				 if(hge->Input_GetKeyState(HGEK_RIGHT))
				 {
				 mVel.x=2;
				 }
				 if(hge->Input_GetKeyState(HGEK_LEFT))
				 {
				 mVel.x=-2;
				 }
				 if(hge->Input_GetKeyState(HGEK_DOWN))
				 {
				 mVel.y=2;
				 }
				 if(hge->Input_GetKeyState(HGEK_UP))
				 {
				 mVel.y=-2;
				 }*/

}

void Mario::updateState()
{
	if(mState!=JUMP)
	{
		if(mState!=B_DOWN)
		{
			if(mState==SKID)
			{
				/*//end skiding if where_we_want_to_go==where_we_are_going 
				if(mLeftHeaded && mVel.x<0)
				mState=WALK;
				if(!mLeftHeaded && mVel.x>0)
				mState=WALK;*/

				if(mVel.x==0)
					mState=IDLE;
			}
			else
			{
				if(mVel.x==0)
				{
					mState=IDLE;
				}
				else
				{
					mState=WALK;
				}
			}
		}
	}
	else
	{
		//we are jumping
		//test if we landed
		if(mGrounded)
			mState=IDLE;
	}
}

void Mario::formUpdates()
{
	if(mForm==SMALL)
	{
		mDimY=32;
		mAABB.mCenter=Vector2(16,16);
		mAABB.mHalf=Vector2(16,16);
		mAnims[WALK]->mDesc=AnimDesc(16,16,14,0,2,8,true,0,0,32);
		mAnims[IDLE]->mDesc=AnimDesc(16,16,14,6,6,8,false,6,0,32);
		mAnims[SKID]->mDesc=AnimDesc(16,16,14,3,3,8,false,3,0,32);
		mAnims[JUMP]->mDesc=AnimDesc(16,16,14,4,4,8,false,4,0,32);
		mAnims[DYING]->mDesc=AnimDesc(16,16,14,5,5,8,false,5,0,32);
		mAnims[SWIM]->mDesc=AnimDesc(16,16,14,9,13,8,true,9,0,32);
		mAnims[FLAGPOLE]->mDesc=AnimDesc(16,16,14,8,8,8,false,8,0,32);
	}
	else if(mForm==BIG)
	{
		mDimY=64;
		mAABB.mCenter=Vector2(16,32);
		mAABB.mHalf=Vector2(16,32);
		mAnims[WALK]->mDesc=AnimDesc(16,32,15,0,2,8,true,0,0,0);
		mAnims[IDLE]->mDesc=AnimDesc(16,32,15,6,6,8,false,6,0,0);
		mAnims[SKID]->mDesc=AnimDesc(16,32,15,3,3,8,false,3,0,0);
		mAnims[JUMP]->mDesc=AnimDesc(16,32,15,4,4,8,false,4,0,0);
		mAnims[SWIM]->mDesc=AnimDesc(16,32,15,9,14,8,true,9,0,0);
		mAnims[FLAGPOLE]->mDesc=AnimDesc(16,32,15,8,8,8,false,8,0,0);
	}
}