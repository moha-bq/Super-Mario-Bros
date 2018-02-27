#pragma once
#include "Anim.h"
#include "AnimDesc.h"
#include <hge.h>
#include <cstdio>

#define SCREENW 640
#define SCREENH 480

#define GAMEMODE 0
#define EDITORMODE 1
#define MENUMODE 2
#define LEVEL_COMPLETE 3
#define EDITOR_INST 4
#define WORLD_SCREEN 5
#define GAME_OVER 6

#define MAX_SCORE_DISPLAYS 8

class HGE;
class TileMap;
class hgeFont;
class TileMapEditor;
class Mario;
class Piranha;


struct ScoreDisplay
{
	ScoreDisplay()
	{
		x=0;
		y=0;
		value= 0;
		time = 0.256;
		timer = 0;
		dead = false;
		vy=-3;
	}
	ScoreDisplay(float xx,float yy,int val);

	void update();
	void render();

	float x,y;
	float vy;
	bool dead;
	int value;
	float time;
	float timer;
};

class Game
{
private:
	Game(void);
	~Game(void);

	static Game* mInst;
public:

	static Game* getInstance();
	static void  destroy();

	bool init();
	void shutdown();

	void update();
	void render();

	static bool FrameFunc();
	static bool RenderFunc();

	void addScoreDisplay(float xx,float yy,int val);

	int getFlags(int id){
		if(id==-1){return 0;}
		else{return mTileFlags[id];}}

	HGE *hge;
	float dt;

	TileMap* mMap;
	
	hgeFont* mSmallFont;
	hgeFont* mNormalFont;
	hgeFont* mBigFont;
	
	TileMapEditor* mEditor;

	int mMode;
	int mPrevTabState;

	Mario* mMario;
	//Piranha* goomba;

	HTEXTURE mTileSet;
	HTEXTURE mTitleScreen;
	HTEXTURE mItemsTex;
	HTEXTURE mMarioTex;

	int mMenuCursor;

	int mTSW;//tileset width
	int mTSH;//tileset height

	int mTileFlags[198];

	int mGameStartTime;
	int mGameTime;
	float mTimePassedInsideEditor;

	int mTimeLimit;
	bool mStopGameTime;

	bool mTimeWarned;

	bool mLevelComplete;

	Anim* mCoinAnim;
	Anim* mRndBlockAnim;

	static bool exit;

	int mNumScoreDisplays;
	ScoreDisplay mScoreDisplays[MAX_SCORE_DISPLAYS];

	HEFFECT mJumpSnd[2];
	HEFFECT mBreakBlockSnd;
	HEFFECT mCoinSnd;
	HEFFECT mGameOverSnd;
	HEFFECT mDieSnd;
	HEFFECT mPauseSnd;
	HEFFECT mPowerUpSnd;
	HEFFECT mPowerUpShowSnd;
	HEFFECT mStompSnd;
	HEFFECT mKickSnd;
	HEFFECT mFlagPoleSnd;
	HEFFECT mBumpSnd;
	HEFFECT mLevelClearSnd;
	HEFFECT mPipeSnd;
	HEFFECT mAlertSnd;

	HSTREAM mMusic;

	HCHANNEL music_channel;

	float credity;
};

