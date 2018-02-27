#include "Mushroom.h"
#include "Piranha.h"
#include "Game.h"
#include "TileMap.h"
#include "Tile.h"

inline static float lerp(float a,float t,float b)
{
	return (t*b + (1-t)*a);
}

Mushroom::Mushroom(void):mAccelX(3),mGravAccelY(0.4875),mPropultionVel(6),mShowUpTime(1.2)
{
	hge=Game::getInstance()->hge;
	mTex=Game::getInstance()->mItemsTex;

	mPos=Vector2(0,0);
	mVel=Vector2(0,0);
	mEaten = false;
	mGrounded = false;

	mShowUpTimer = 0;
	mState = SHOWUP;

	mY = 0;
}


Mushroom::~Mushroom(void)
{
}

void Mushroom::update()
{
	if(mEaten)
		return;

	if(mState==SHOWUP)
	{
		mPos.y = mY - lerp(0,mShowUpTimer/mShowUpTime,32);
		mShowUpTimer+=Game::getInstance()->hge->Timer_GetDelta();
		if(mShowUpTimer>mShowUpTime)
		{
			mState = NORMAL;
			//mVel.y-= mPropultionVel;
			mVel.x = mAccelX;
		}
	}
	else
	{
		if(!mGrounded)
		{
			mVel.y+=mGravAccelY;
		}

		mGrounded=false;


		TileMap* map=Game::getInstance()->mMap;
		int mariox,marioy,mariox_end,marioy_end;

		mPos.x+=mVel.x;

		mariox=map->xWorldToMap(mPos.x);marioy=map->yWorldToMap(mPos.y);
		mariox_end=map->xWorldToMap(mPos.x+32);marioy_end=map->yWorldToMap(mPos.y+32-1);

		if(mariox>=map->mWidth || marioy_end>=map->mHeight || mariox<0 || marioy<0)
			return;

		int init_direction = mVel.x>0;
		int changed_direction = 0;
		for(int i=mariox;i<=mariox_end;i++)
		{
			for(int j=marioy;j<=marioy_end;j++)
			{
				if(map->isTile(i,j,IMPENETRABLE))
				{
					changed_direction = 1;
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
		if(changed_direction)
		{
			if(init_direction)mVel.x = -mAccelX;
			else mVel.x = mAccelX;
		}


		mPos.y+=mVel.y;

		mariox=map->xWorldToMap(mPos.x);marioy=map->yWorldToMap(mPos.y);
		mariox_end=map->xWorldToMap(mPos.x+31);marioy_end=map->yWorldToMap(mPos.y+32);

		if(mariox>=map->mWidth || marioy_end>=map->mHeight || mariox<0 || marioy<0)
			return;

		for(int i=mariox;i<=mariox_end;i++)
		{
			for(int j=marioy;j<=marioy_end;j++)
			{
				if(map->isTile(i,j,IMPENETRABLE))
				{
					if(mVel.y>0)
					{
						mPos.y=j*map->mTileHeight-32;
						mGrounded=true;
						mVel.y=0;
					}
					if(mVel.y<0)
					{
						mPos.y=(j+1)*map->mTileHeight;
						mVel.y=0;
					}
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
					mVel.y = -mPropultionVel;
					if(mPos.x<xi*map->mTileWidth)
					{
						mVel.x = -mAccelX;
					}
					else
					{
						mVel.x = mAccelX;
					}
				}
			}
		}

	}
}


void Mushroom::render()
{
	if(mEaten)
		return;

	TileMap* map=Game::getInstance()->mMap;
	float x,y,w,h;
	x=0;y=0;w=16;h=16;

	mQuad.v[0].x=mPos.x-map->mxScroll;
	mQuad.v[0].y=mPos.y-map->myScroll;
	mQuad.v[0].tx=x/hge->Texture_GetWidth(mTex);
	mQuad.v[0].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[1].x=mPos.x+32-map->mxScroll;
	mQuad.v[1].y=mPos.y-map->myScroll;
	mQuad.v[1].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[1].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[2].x=mPos.x+32-map->mxScroll;
	mQuad.v[2].y=mPos.y+32-map->myScroll;
	mQuad.v[2].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[2].ty=(y+h)/hge->Texture_GetHeight(mTex);

	mQuad.v[3].x=mPos.x-map->mxScroll;
	mQuad.v[3].y=mPos.y+32-map->myScroll;
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

	//@HACK to draw mushroom behind
	if(mState==SHOWUP)
	{
		Game* game=Game::getInstance();
		TileMap* map = game->mMap;

		int texh=hge->Texture_GetHeight(game->mTileSet);
		int texw=hge->Texture_GetWidth(game->mTileSet);
		float tx,ty,w,h;

		tx=(float)((27)%(33)*16);
		ty=(float)((27)/(33)*16);

		w=16;
		h=16;
		int ox=0,oy=0;
		int i = mPos.x/map->mTileWidth;
		int j = mY/map->mTileHeight;
	
		hgeQuad quad;

		quad.v[0].x=i*map->mTileWidth+ox-map->mxScroll;
		quad.v[0].y=j*map->mTileHeight+oy-map->myScroll;
		quad.v[0].tx=tx/texw;
		quad.v[0].ty=ty/texh;

		quad.v[1].x=(i+1)*map->mTileWidth+ox-map->mxScroll;
		quad.v[1].y=j*map->mTileHeight+oy-map->myScroll;
		quad.v[1].tx=(tx+w)/texw;
		quad.v[1].ty=ty/texh;

		quad.v[2].x=(i+1)*map->mTileWidth+ox-map->mxScroll;
		quad.v[2].y=(j+1)*map->mTileHeight+oy-map->myScroll;
		quad.v[2].tx=(tx+w)/texw;
		quad.v[2].ty=(ty+h)/texh;

		quad.v[3].x=i*map->mTileWidth+ox-map->mxScroll;
		quad.v[3].y=(j+1)*map->mTileHeight+oy-map->myScroll;
		quad.v[3].tx=tx/texw;
		quad.v[3].ty=(ty+h)/texh;

		quad.tex=game->mTileSet;
		quad.blend=BLEND_DEFAULT;

		for(int k=0;k<4;k++)
		{
			quad.v[k].z=0.5f;
		}
		quad.v[0].col = 
			quad.v[1].col = 
			quad.v[2].col = 
			quad.v[3].col = 0xffffffff;
		Game::getInstance()->hge->Gfx_RenderQuad(&quad);

	}
}