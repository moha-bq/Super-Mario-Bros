#include "BlockDebris.h"
#include "Piranha.h"
#include "Game.h"
#include "TileMap.h"
#include "Tile.h"

#define DEG_TO_RAD 0.01745329f


static void rotateQuad(hgeQuad& quad,float t)
{
	float cost = cosf(t*DEG_TO_RAD);
	float sint = sinf(t*DEG_TO_RAD);
	
	float tx1, ty1, tx2, ty2;

	tx1 =0;
	ty1 =0;
	tx2 = quad.v[1].x - quad.v[0].x;
	ty2 = quad.v[3].y - quad.v[0].y;

	float x=quad.v[0].x;
	float y=quad.v[0].y;

	quad.v[0].x  = tx1*cost - ty1*sint + x;
	quad.v[0].y  = tx1*sint + ty1*cost + y;	

	quad.v[1].x  = tx2*cost - ty1*sint + x;
	quad.v[1].y  = tx2*sint + ty1*cost + y;	

	quad.v[2].x  = tx2*cost - ty2*sint + x;
	quad.v[2].y  = tx2*sint + ty2*cost + y;	

	quad.v[3].x  = tx1*cost - ty2*sint + x;
	quad.v[3].y  = tx1*sint + ty2*cost + y;
}

BlockDebris::BlockDebris():mGravAccelY(0.4875)
{
	hge=Game::getInstance()->hge;
	mTex=Game::getInstance()->mTileSet;
	mPos=Vector2(0,0);
	mTheta = 0;
	mActive = true;
}

BlockDebris::~BlockDebris(void)
{
}

void BlockDebris::update()
{
	if(!mActive)
		return;

	mVel.y +=mGravAccelY;

	mPos+=mVel;
	mTheta +=2;
	if(mTheta>360)
		mTheta-=360;

	if(mPos.y>SCREENW*2)
		mActive = false;
}

void BlockDebris::render()
{
	TileMap* map=Game::getInstance()->mMap;
	float x,y,w,h;
	x=32;y=0;w=16;h=16;

	int a=16;

	mQuad.v[0].x=mPos.x-map->mxScroll;
	mQuad.v[0].y=mPos.y-map->myScroll;
	mQuad.v[0].tx=x/hge->Texture_GetWidth(mTex);
	mQuad.v[0].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[1].x=mPos.x+a-map->mxScroll;
	mQuad.v[1].y=mPos.y-map->myScroll;
	mQuad.v[1].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[1].ty=y/hge->Texture_GetHeight(mTex);

	mQuad.v[2].x=mPos.x+a-map->mxScroll;
	mQuad.v[2].y=mPos.y+a-map->myScroll;
	mQuad.v[2].tx=(x+w)/hge->Texture_GetWidth(mTex);
	mQuad.v[2].ty=(y+h)/hge->Texture_GetHeight(mTex);

	mQuad.v[3].x=mPos.x-map->mxScroll;
	mQuad.v[3].y=mPos.y+a-map->myScroll;
	mQuad.v[3].tx=x/hge->Texture_GetWidth(mTex);
	mQuad.v[3].ty=(y+h)/hge->Texture_GetHeight(mTex);

	mQuad.blend=BLEND_DEFAULT;
	mQuad.tex=mTex;
	for(int k=0;k<4;k++)
	{
		mQuad.v[k].z=0.5f;
	}
	mQuad.v[0].col = mQuad.v[1].col = mQuad.v[2].col = mQuad.v[3].col = 0xffffffff;

	rotateQuad(mQuad,mTheta);
	hge->Gfx_RenderQuad(&mQuad);
}