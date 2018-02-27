#include "TileMap.h"
#include "Game.h"
#include "Anim.h"
#include "Goomba.h"
#include "Turtle.h"
#include "Piranha.h"
#include "Tile.h"
#include "Mario.h"
#include "Mushroom.h"

TileMap::TileMap(int w,int h,int tilew,int tileh)
{
	mWidth=w;
	mHeight=h;
	mTileWidth=tilew;
	mTileHeight=tileh;

	mTiles=new int[w*h];

	for(int i=0;i<w*h;i++)
		mTiles[i]=-1;

	mNumGoombas=0;
	for(int i=0;i<MAX_GOOMBAS;i++)
		mGoombas[i]=0;

	mNumTurtles=0;
	for(int i=0;i<MAX_TURTLES;i++)
		mTurtles[i]=0;

	mNumPiranhas=0;
	for(int i=0;i<MAX_PIRANHAS;i++)
		mPiranhas[i]=0;

	mxScroll=0;
	myScroll=0;

	mScrollWinH=SCREENH;
	mScrollWinW=SCREENW;

	mMovyTiles[0]=mMovyTiles[1]=-1;
	mMovyOffsets[0]=mMovyOffsets[1]=0;
	mMovyVel[0]=mMovyVel[1]=0;
	mMovyId=0;

	mNumFlyingCoin=0;

	mNumRedMushrooms = 0;
	mNumRedMushroomTiles = 0;
	mNumDebris = 0;
	mScore = 0;
}

TileMap::TileMap(std::string path)
{
	mTiles=0;

	mNumGoombas=0;
	for(int i=0;i<MAX_GOOMBAS;i++)
		mGoombas[i]=0;

	mNumTurtles=0;
	for(int i=0;i<MAX_TURTLES;i++)
		mTurtles[i]=0;

	mNumPiranhas=0;
	for(int i=0;i<MAX_PIRANHAS;i++)
		mPiranhas[i]=0;

	mxScroll=0;
	myScroll=0;

	mScrollWinH=SCREENH;
	mScrollWinW=SCREENW;

	mMovyTiles[0]=mMovyTiles[1]=-1;
	mMovyOffsets[0]=mMovyOffsets[1]=0;
	mMovyVel[0]=mMovyVel[1]=0;

	mMovyId=0;

	
	mNumRedMushrooms = 0;
	mNumRedMushroomTiles = 0;
	mNumDebris = 0;

	loadFromFile(path);
	mScore = 0;
}

TileMap::TileMap(const TileMap &copy)
{
	mWidth=copy.mWidth;
	mHeight=copy.mHeight;
	mTileWidth=copy.mTileWidth;
	mTileHeight=copy.mTileHeight;

	mTiles=new int[mWidth*mHeight];

	mNumGoombas=copy.mNumGoombas;
	for(int i=0;i<MAX_GOOMBAS;i++)
		mGoombas[i]=copy.mGoombas[i];

	mNumTurtles=copy.mNumTurtles;
	for(int i=0;i<MAX_TURTLES;i++)
		mTurtles[i]=copy.mTurtles[i];

	mNumPiranhas=copy.mNumPiranhas;
	for(int i=0;i<MAX_PIRANHAS;i++)
		mPiranhas[i]=copy.mPiranhas[i];

	mNumRedMushrooms = 0;
	mNumRedMushroomTiles = 0;
	mNumDebris = 0;
	mScore = 0;

}

TileMap::~TileMap(void)
{
	if(mTiles)
		delete[] mTiles;
	
	for(int i=0;i<mNumGoombas;i++)
		delete mGoombas[i];

	for(int i=0;i<mNumTurtles;i++)
		delete mTurtles[i];

	for(int i=0;i<mNumPiranhas;i++)
		delete mPiranhas[i];
}

void TileMap::resize(int w,int h)
{
	if(mTiles)
		delete[] mTiles;

	mTiles=new int[w*h];
	mWidth=w;
	mHeight=h;
}

void TileMap::addDebris(Vector2 pos)
{
	if(mNumDebris+3>=MAX_DEBRIS)
		return;

	int i=mNumDebris;
	for(int k=0;k<4;k++){
		mDebris[k+i].mPos = pos;
		mDebris[k+i].mActive = true;
	}
	
	float vx1=5,vx2=3,vy1=-6,vy2=-3;
//	vx1=vx2=vy1=vy2=0;

	mDebris[i].mVel = Vector2(vx1,vy1);
	mDebris[i+1].mVel = Vector2(-vx1,vy1);
	mDebris[i+2].mVel = Vector2(vx2,vy2);
	mDebris[i+3].mVel = Vector2(-vx2,vy2);

	mNumDebris+=4;
}

void TileMap::addTile(int x,int y,int id)
{
	if(x>=mWidth || y>=mHeight)
		return;

	mTiles[y*mWidth+x]=id;
}

void TileMap::addRedMushroomTile(int p)
{
	if(mNumRedMushroomTiles==MAX_MUSHROOMS)
		return;

	for(int i=0;i<mNumRedMushroomTiles;i++)
		if(mRedMushroomTiles[i]==p)
			return;

	mRedMushroomTiles[mNumRedMushroomTiles++] = p;
}

void TileMap::addRedMushroom(Vector2 pos)
{
	if(mNumRedMushrooms==MAX_MUSHROOMS)
		return;

	mRedMushrooms[mNumRedMushrooms]=new Mushroom();
	mRedMushrooms[mNumRedMushrooms]->mPos=pos;
	mRedMushrooms[mNumRedMushrooms]->mY=pos.y;

	mNumRedMushrooms++;
}

void TileMap::addGoomba(Vector2 pos,int head)
{
	if(mNumGoombas==MAX_GOOMBAS)
		return;

	mGoombas[mNumGoombas]=new Goomba(head);
	mGoombas[mNumGoombas]->mPos=pos;
	mGoombas[mNumGoombas]->mInitPos=pos;

	mNumGoombas++;
}

void TileMap::addTurtle(Vector2 pos,int head)
{
	if(mNumTurtles==MAX_TURTLES)
		return;

	mTurtles[mNumTurtles]=new Turtle(head);
	mTurtles[mNumTurtles]->mPos=pos;
	mTurtles[mNumTurtles]->mInitPos=pos;

	mNumTurtles++;
}

void TileMap::addPiranha(Vector2 pos)
{
	if(mNumPiranhas==MAX_TURTLES)
		return;

	mPiranhas[mNumPiranhas]=new Piranha();
	mPiranhas[mNumPiranhas]->mPos=pos;
	mPiranhas[mNumPiranhas]->mInitPos=pos;
	mPiranhas[mNumPiranhas]->mSymY=pos.y;

	mNumPiranhas++;
}

void TileMap::update()
{
	if(Game::getInstance()->mMode==GAMEMODE)
	{

		for(int i=0;i<mNumDebris;i++)
			mDebris[i].update();
		
		for(int i=0;i<mNumDebris;i++)
		{
			if(!mDebris[i].mActive)//remove debris
			{
				for(int j=i;j<mNumDebris-1;j++)
					mDebris[j]=mDebris[j+1];
				mNumDebris--;
			}
		}

		for(int i=0;i<mNumGoombas;i++)
			mGoombas[i]->update();

		for(int i=0;i<mNumGoombas;i++)
		{
			if(mGoombas[i]->mState==Goomba::DEAD)//remove debris
			{
				for(int j=i;j<mNumGoombas-1;j++)
					mGoombas[j]=mGoombas[j+1];
				mNumGoombas--;
			}
		}

		for(int i=0;i<mNumRedMushrooms;i++)
		{
			mRedMushrooms[i]->update();
		}

		for(int i=0;i<mNumTurtles;i++)
			mTurtles[i]->update();
		for(int i=0;i<mNumTurtles;i++)
		{
			if(mTurtles[i]->mState==Turtle::DEAD)//remove debris
			{
				for(int j=i;j<mNumTurtles-1;j++)
					mTurtles[j]=mTurtles[j+1];
				mNumTurtles--;
			}
		}

		for(int i=0;i<mNumPiranhas;i++)
		{
			mPiranhas[i]->update();
		}
		//update flying coins
		for(int i=0;i<mNumFlyingCoin;i++)
		{
			if(mFlyingCoinVelY[i]<0)
			{
				Game::getInstance()->addScoreDisplay(mFlyingCoinPos[i].x-mxScroll,mFlyingCoinPos[i].y-myScroll-mFlyingCoinOffsetY[i],200);
				mScore+=200;
				Game::getInstance()->mMario->mCoins++;//@FUCK

				//flying coin should die..array should be consistent!
				for(int j=i;j<mNumFlyingCoin-1;j++)
				{
					mFlyingCoinOffsetY[j]=mFlyingCoinOffsetY[j+1];
					mFlyingCoinPos[j]=mFlyingCoinPos[j+1];
					mFlyingCoinVelY[j]=mFlyingCoinVelY[j+1];
				}
				mNumFlyingCoin--;
			}
			if(mFlyingCoinOffsetY[i]>64)
			{
				mFlyingCoinVelY[i]=-mFlyingCoinVelY[i];
			}

			mFlyingCoinOffsetY[i]+=mFlyingCoinVelY[i];
		}

		//update movy tiles
		for(int i=0;i<2;i++)
		{
			if(mMovyTiles[i]!=-1)
			{
				if(mMovyVel[i]!=0)
				{
					if(mMovyOffsets[i]<0)
					{
						mMovyOffsets[i]=0;
						mMovyVel[i]=0;
						mMovyTiles[i]=-1;
					}
					else if(mMovyOffsets[i]>16)
					{
						mMovyVel[i]=-mMovyVel[i];
					}

					mMovyOffsets[i]+=mMovyVel[i];
				}
			}
		}
	}
}

void TileMap::render(int ox,int oy)
{
	int minx,maxx,miny,maxy;
	Game* game=Game::getInstance();
	hgeQuad quad;
	float yoff;
	int texh=game->hge->Texture_GetHeight(game->mTileSet);
	int texw=game->hge->Texture_GetWidth(game->mTileSet);
	float tx,ty,w,h;

	//get rid of tile 1pixel-offsetting because of numerical percision
	mxScroll = (int)mxScroll;
	myScroll =(int)myScroll;

	minx=mxScroll/(mTileWidth);
	maxx=(mxScroll+mScrollWinW)/(mTileWidth-1);
	miny=myScroll/(mTileHeight);
	maxy=(myScroll+mScrollWinH)/(mTileHeight-1);


	//background tiles
	for(int i=minx;i<=maxx;i++)
	{
		for(int j=miny;j<=maxy;j++)
		{
			 yoff=0;
			if(i>=0 && i<mWidth && j>=0 && j<mHeight)
			{
			
					tx=(float)((33*5+2)%(33)*16);
					ty=(float)((33*5+2)/(33)*16);
					w=16;
					h=16;

				quad.v[0].x=i*mTileWidth+ox-mxScroll;
				quad.v[0].y=j*mTileHeight+oy-myScroll-yoff;
				quad.v[0].tx=tx/texw;
				quad.v[0].ty=ty/texh;

				quad.v[1].x=(i+1)*mTileWidth+ox-mxScroll;
				quad.v[1].y=j*mTileHeight+oy-myScroll-yoff;
				quad.v[1].tx=(tx+w)/texw;
				quad.v[1].ty=ty/texh;

				quad.v[2].x=(i+1)*mTileWidth+ox-mxScroll;
				quad.v[2].y=(j+1)*mTileHeight+oy-myScroll-yoff;
				quad.v[2].tx=(tx+w)/texw;
				quad.v[2].ty=(ty+h)/texh;

				quad.v[3].x=i*mTileWidth+ox-mxScroll;
				quad.v[3].y=(j+1)*mTileHeight+oy-myScroll-yoff;
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
	}

	//render piranhas under pipes:
	if(Game::getInstance()->mMode==GAMEMODE)
	{
		for(int i=0;i<mNumPiranhas;i++)
		{
			mPiranhas[i]->render();
		}
	}

	//rendering flying coins
	game->mCoinAnim->getClip(&tx,&ty,&w,&h);

	for(int i=0;i<mNumFlyingCoin;i++)
	{
		quad.v[0].x=mFlyingCoinPos[i].x - mxScroll;
		quad.v[0].y=mFlyingCoinPos[i].y-mFlyingCoinOffsetY[i] - myScroll;
		quad.v[0].tx=tx/texw;
		quad.v[0].ty=ty/texh;

		quad.v[1].x=mFlyingCoinPos[i].x+mTileWidth - mxScroll;
		quad.v[1].y=mFlyingCoinPos[i].y-mFlyingCoinOffsetY[i] -myScroll;
		quad.v[1].tx=(tx+w)/texw;
		quad.v[1].ty=ty/texh;

		quad.v[2].x=mFlyingCoinPos[i].x+mTileWidth - mxScroll;
		quad.v[2].y=mFlyingCoinPos[i].y+mTileHeight-mFlyingCoinOffsetY[i]-myScroll;
		quad.v[2].tx=(tx+w)/texw;
		quad.v[2].ty=(ty+h)/texh;

		quad.v[3].x=mFlyingCoinPos[i].x - mxScroll;
		quad.v[3].y=mFlyingCoinPos[i].y+mTileHeight-mFlyingCoinOffsetY[i]-myScroll;
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

	//tiles
	for(int i=minx;i<=maxx;i++)
	{
		for(int j=miny;j<=maxy;j++)
		{
			 yoff=0;
			if(i>=0 && i<mWidth && j>=0 && j<mHeight && mTiles[j*mWidth+i]!=-1)
			{
				if(mTiles[j*mWidth+i]==24)
				{
					game->mRndBlockAnim->getClip(&tx,&ty,&w,&h);
				}
				else if(mTiles[j*mWidth+i]==57)
				{
					game->mCoinAnim->getClip(&tx,&ty,&w,&h);
				}
				else
				{
					tx=(float)(mTiles[j*mWidth+i]%(33)*16);
					ty=(float)(mTiles[j*mWidth+i]/(33)*16);
					w=16;
					h=16;
				}

				//if moving movy block
				for(int ii=0;ii<2;ii++)
					if(j*mWidth+i==mMovyTiles[ii])
						yoff=mMovyOffsets[ii];

				quad.v[0].x=i*mTileWidth+ox-mxScroll;
				quad.v[0].y=j*mTileHeight+oy-myScroll-yoff;
				quad.v[0].tx=tx/texw;
				quad.v[0].ty=ty/texh;

				quad.v[1].x=(i+1)*mTileWidth+ox-mxScroll;
				quad.v[1].y=j*mTileHeight+oy-myScroll-yoff;
				quad.v[1].tx=(tx+w)/texw;
				quad.v[1].ty=ty/texh;

				quad.v[2].x=(i+1)*mTileWidth+ox-mxScroll;
				quad.v[2].y=(j+1)*mTileHeight+oy-myScroll-yoff;
				quad.v[2].tx=(tx+w)/texw;
				quad.v[2].ty=(ty+h)/texh;

				quad.v[3].x=i*mTileWidth+ox-mxScroll;
				quad.v[3].y=(j+1)*mTileHeight+oy-myScroll-yoff;
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
	}

	//rendering enemies
	if(Game::getInstance()->mMode==GAMEMODE)
	{
		for(int i=0;i<mNumGoombas;i++)
		{
			mGoombas[i]->render();
		}

		for(int i=0;i<mNumTurtles;i++)
		{
			mTurtles[i]->render();
		}

		for(int i=0;i<mNumRedMushrooms;i++)
		{
			mRedMushrooms[i]->render();
		}

		for(int i=0;i<mNumDebris;i++)
			mDebris[i].render();
	}
}

void TileMap::addFlyingCoin(Vector2 pos)
{
	if(mNumFlyingCoin>=MAX_FLYINGCOIN)
		return;

	mFlyingCoinPos[mNumFlyingCoin]=pos;
	mFlyingCoinVelY[mNumFlyingCoin]=8;
	mFlyingCoinOffsetY[mNumFlyingCoin]=0;

	mNumFlyingCoin++;
}

bool TileMap::isVisible(Vector2 pos)
{
	if(fabs(pos.x - Game::getInstance()->mMario->mPos.x)<mScrollWinW)
		return true;

	return false;
}

int TileMap::getTile(int x,int y)
{
	if(x>=mWidth || y>=mHeight || x<0 || y<0)
		return 0;

	return mTiles[y*mWidth+x];
}

bool TileMap::isTile(int x,int y,int flag)
{
	int g=getTile(x,y);
	int b=Game::getInstance()->getFlags(getTile(x,y));
	return (Game::getInstance()->getFlags(getTile(x,y)) & flag);
}

int TileMap::xWorldToMap(float x)
{
	return (int)x/mTileWidth;
}

int TileMap::yWorldToMap(float y)
{
	return (int)y/mTileHeight;
}

int TileMap::xWinToMap(float x)
{
	return (int)(x+mxScroll)/mTileWidth;
}

int TileMap::yWinToMap(float y)
{
	return (int)(y+myScroll)/mTileHeight;
}

void TileMap::setScroll(float x,float y)
{
	mxScroll=x;
	myScroll=y;
}

void TileMap::addScroll(float dx,float dy)
{
	setScroll(mxScroll+dx,myScroll+dy);
}

void TileMap::clampScroll(float minx,float miny,float maxx,float maxy)
{
	mxScroll=(mxScroll>maxx)?maxx:mxScroll;
	myScroll=(myScroll>maxy)?maxy:myScroll;
	mxScroll=(mxScroll<minx)?minx:mxScroll;
	myScroll=(myScroll<miny)?miny:myScroll;
}

void TileMap::loadFromFile(std::string path)
{
	FILE* file=fopen(path.c_str(),"rb");
	int dim[4];
	fread(dim,sizeof(int)*4,1,file);
	mTileWidth=dim[2];
	mTileHeight=dim[3];

	this->resize(dim[0],dim[1]);
	fread(mTiles,sizeof(int)*dim[0]*dim[1],1,file);

	fread(&mNumGoombas,sizeof(int),1,file);

	for(int i=0;i<mNumGoombas;i++)
	{
		mGoombas[i]=new Goomba(1);
		fread(mGoombas[i],sizeof(Goomba),1,file);
		mGoombas[i]->__load();
	}

	fread(&mNumTurtles,sizeof(int),1,file);
	for(int i=0;i<mNumTurtles;i++)
	{
		mTurtles[i]=new Turtle(1);
		fread(mTurtles[i],sizeof(Turtle),1,file);
		mTurtles[i]->__load();
	}

	fread(&mNumPiranhas,sizeof(int),1,file);
	for(int i=0;i<mNumPiranhas;i++)
	{
		mPiranhas[i]=new Piranha(1);
		fread(mPiranhas[i],sizeof(Piranha),1,file);
		mPiranhas[i]->__load();
	}

	fread(&mNumRedMushroomTiles,sizeof(int),1,file);
	for(int i=0;i<mNumRedMushroomTiles;i++)
	{
		fread(&mRedMushroomTiles[i],sizeof(int),1,file);
	}

	fread(&mxCastle,sizeof(int),1,file);

	mxScroll=0;
	myScroll=0;
	mScrollWinH=SCREENH;
	mScrollWinW=SCREENW;

	mMovyTiles[0]=mMovyTiles[1]=-1;
	mMovyOffsets[0]=mMovyOffsets[1]=0;
	mMovyVel[0]=mMovyVel[1]=0;
	mMovyId=0;

	mNumFlyingCoin=0;

	fclose(file);
}

void TileMap::saveToFile(std::string path)
{
	FILE* file=fopen(path.c_str(),"wb");

	fwrite(&mWidth,sizeof(int),1,file);
	fwrite(&mHeight,sizeof(int),1,file);
	fwrite(&mTileWidth,sizeof(int),1,file);
	fwrite(&mTileHeight,sizeof(int),1,file);

	fwrite(mTiles,sizeof(int)*mWidth*mHeight,1,file);

	fwrite(&mNumGoombas,sizeof(int),1,file);
	for(int i=0;i<mNumGoombas;i++)
		fwrite(mGoombas[i],sizeof(Goomba),1,file);

	fwrite(&mNumTurtles,sizeof(int),1,file);
	for(int i=0;i<mNumTurtles;i++)
		fwrite(mTurtles[i],sizeof(Turtle),1,file);

	fwrite(&mNumPiranhas,sizeof(int),1,file);
	for(int i=0;i<mNumPiranhas;i++)
		fwrite(mPiranhas[i],sizeof(Piranha),1,file);

	fwrite(&mNumRedMushroomTiles,sizeof(int),1,file);
	for(int i=0;i<mNumRedMushroomTiles;i++)
		fwrite(&mRedMushroomTiles[i],sizeof(int),1,file);

	fwrite(&mxCastle,sizeof(int),1,file);

	fclose(file);
}