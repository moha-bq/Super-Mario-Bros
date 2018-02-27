#pragma once

#define IMPENETRABLE 1
#define COINY (1<<1)
#define MOVY (1<<2)
#define GENEROUS (1<<3)
#define BREAKABLE (1<<4)
#define POLE (1<<5)

class Anim;

class Tile
{
public:
	Tile(void);
	~Tile(void);

	bool is(int flag);
	void set(int flag);
	void unset(int flag);
	void toggle(int flag);

	int mFlags;
	int mId;
	int mSheet;
	Anim* mAnim;

	float myOffset;
	float mVelY;
};

