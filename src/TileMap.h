#pragma once

#include <string>
#include <hge.h>
#include "AnimDesc.h"
#include "Vector2.h"
#include "BlockDebris.h"

#define MAX_GOOMBAS 64
#define MAX_TURTLES 64
#define MAX_PIRANHAS 32
#define MAX_MOVYTILE 32
#define MAX_FLYINGCOIN 3
#define MAX_MUSHROOMS 32
#define MAX_DEBRIS 64

class Anim;
class Goomba;
class Turtle;
class Piranha;
class Mushroom;

class TileMap
{
public:
	TileMap(int w,int h,int tilew,int tileh);
	TileMap(std::string path);
	TileMap(const TileMap &copy);
	~TileMap(void);

	void update();
	void render(int ox=0,int oy=0);

	void loadFromFile(std::string path);
	void saveToFile(std::string path);

	void resize(int w,int h);

	void addTile(int x,int y,int id);

	void addGoomba(Vector2 pos,int head=-1);
	void addTurtle(Vector2 pos,int head=-1);
	void addPiranha(Vector2 pos);
	void addRedMushroom(Vector2 pos);
	void addDebris(Vector2 pos);

	void addRedMushroomTile(int p);

	void addFlyingCoin(Vector2 pos);

	int getTile(int x,int y);

	bool isTile(int x,int y,int flag);

	int xWorldToMap(float x);
	int yWorldToMap(float y);

	int xWinToMap(float x);
	int yWinToMap(float y);

	void setScroll(float x,float y);
	void addScroll(float dx,float dy);
	void clampScroll(float minx,float miny,float maxx,float maxy);

	bool isVisible(Vector2 pos);

	int mScore;

	int mWidth;
	int mHeight;
	int mTileWidth;
	int mTileHeight;

	float mxScroll,myScroll;
	float mScrollWinH,mScrollWinW;

	int mNumGoombas;
	int mNumTurtles;
	int mNumPiranhas;
	int mNumRedMushrooms;

	int* mTiles;

	Goomba* mGoombas[MAX_GOOMBAS];
	Turtle* mTurtles[MAX_TURTLES];
	Piranha* mPiranhas[MAX_PIRANHAS];
	Mushroom* mRedMushrooms[MAX_MUSHROOMS];

	int mMovyTiles[2];
	float mMovyOffsets[2];
	float mMovyVel[2];

	int mMovyId;//hold the next mMovyTiles indice to fill;

	Vector2 mFlyingCoinPos[MAX_FLYINGCOIN];
	float mFlyingCoinVelY[MAX_FLYINGCOIN];
	float mFlyingCoinOffsetY[MAX_FLYINGCOIN];
	int mNumFlyingCoin;

	int mNumRedMushroomTiles;
	int mRedMushroomTiles[MAX_MUSHROOMS];//tile index of red mushroom

	int mNumDebris;
	BlockDebris mDebris[MAX_DEBRIS];

	int mxCastle;
};

