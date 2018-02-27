#include "Tile.h"


Tile::Tile(void)
{
	mFlags=0;
	mId=-1;
	mSheet=-1;
	mAnim=0;
	myOffset=0;
	mVelY=0;
}


Tile::~Tile(void)
{
}

bool Tile::is(int flag)
{
	return (mFlags & flag);
}

void Tile::set(int flag)
{
	mFlags|=flag;
}

void Tile::unset(int flag)
{
	mFlags&=~flag;
}

void Tile::toggle(int flag)
{
	mFlags^=flag;
}