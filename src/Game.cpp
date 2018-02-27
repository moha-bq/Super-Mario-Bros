#include "Game.h"
#include <hge.h>
#include <hgefont.h>

#include "TileMap.h"
#include "TileMapEditor.h"
#include "Mario.h"
#include "Piranha.h"
#include "Tile.h"


Game* Game::mInst=0;
bool Game::exit = false;

const char* lvl_name="lvl1";

Game::Game(void)
{
	exit = false;

	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_LOGFILE, "game.log");
	hge->System_SetState(HGE_FRAMEFUNC,Game::FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC,Game::RenderFunc);
	hge->System_SetState(HGE_WINDOWED, false);
	hge->System_SetState(HGE_FPS,60);

	hge->System_SetState(HGE_SCREENWIDTH, SCREENW);
	hge->System_SetState(HGE_SCREENHEIGHT, SCREENH);
	hge->System_SetState(HGE_SCREENBPP, 32);

	hge->System_SetState(HGE_TEXTUREFILTER, false);
	hge->System_SetState(HGE_SHOWSPLASH, true);
	hge->System_SetState(HGE_HIDEMOUSE, false);

//	hge->System_SetState(HGE_USESOUND, true);
	hge->System_SetState(HGE_TITLE,"Super Mario");
	hge->System_SetState(HGE_ZBUFFER,true);

}


Game::~Game(void)
{
	hge->System_Shutdown();
	hge->Release();
}

Game* Game::getInstance()
{
	if(mInst)
		return mInst;

	mInst=new Game;

	return mInst;
}

void Game::destroy()
{

	delete mInst;
	mInst=0;
}

bool Game::FrameFunc()
{
	Game::getInstance()->dt=Game::getInstance()->hge->Timer_GetDelta();
	if (Game::exit) return true;

	Game::getInstance()->update();

	return false;
}

bool Game::RenderFunc()
{
	Game::getInstance()->hge->Gfx_BeginScene();
	Game::getInstance()->hge->Gfx_Clear(0);

	Game::getInstance()->render();

	Game::getInstance()->hge->Gfx_EndScene();

	return false;
}

bool Game::init()
{

	for(int i=0;i<198;i++)
		mTileFlags[i]=0;

	mTileFlags[24]|=IMPENETRABLE | MOVY;//24 :first question box
	mTileFlags[25]|=IMPENETRABLE;
	mTileFlags[26]|=IMPENETRABLE;
	mTileFlags[27]|=IMPENETRABLE;

	mTileFlags[0]|=IMPENETRABLE;
	//mTileFlags[3]|=POLE;
	mTileFlags[33*2]|=IMPENETRABLE;
	mTileFlags[33]|=IMPENETRABLE;
	mTileFlags[33*2+1]|=IMPENETRABLE;
	mTileFlags[33*3]|=IMPENETRABLE;
	mTileFlags[33*3+1]|=IMPENETRABLE;
	mTileFlags[1]|=BREAKABLE | MOVY | IMPENETRABLE;
	mTileFlags[2]|=BREAKABLE | MOVY | IMPENETRABLE;
	mTileFlags[57]|=COINY;
	mTileFlags[57]&=~IMPENETRABLE;
	mTileFlags[115]|=POLE;

	if(!hge->System_Initiate())
	{
		hge->System_SetState(HGE_WINDOWED, true);
		if(!hge->System_Initiate())
		{
			return false;
		}
	}
	
		
	mJumpSnd[0] = hge->Effect_Load("data//smb_jump_small.wav");
	mJumpSnd[1] = hge->Effect_Load("data//smb_jump_super.wav");

	mBreakBlockSnd = hge->Effect_Load("data//smb_breakblock.wav");
	mCoinSnd = hge->Effect_Load("data//smb_coin.wav");
	mGameOverSnd = hge->Effect_Load("data//smb_gameover.wav");
	mDieSnd = hge->Effect_Load("data//smb_mariodie.wav");
	mPauseSnd = hge->Effect_Load("data//smb_pause.wav");
	mPowerUpSnd = hge->Effect_Load("data//smb_powerup.wav");
	mPowerUpShowSnd = hge->Effect_Load("data//smb_powerup_appears.wav");
	mStompSnd = hge->Effect_Load("data//smb_stomp.wav");
	mKickSnd = hge->Effect_Load("data//smb_kick.wav");
	mBumpSnd = hge->Effect_Load("data//smb_bump.wav");
	mLevelClearSnd = hge->Effect_Load("data//smb_stage_clear.wav");
	mFlagPoleSnd = hge->Effect_Load("data//smb_flagpole.wav");
	mPipeSnd = hge->Effect_Load("data//smb_pipe.wav");
	mAlertSnd = hge->Effect_Load("data//smb_warning.wav");
	
	mMusic = hge->Stream_Load("data//music.ogg");

	mTileSet=hge->Texture_Load("data//tileset.png");
	mTitleScreen=hge->Texture_Load("data//title.png");
	mItemsTex=hge->Texture_Load("data//itemsobj.png");
	mMarioTex = hge->Texture_Load("data//mariobros.png");

	mTSW=hge->Texture_GetWidth(mTileSet);
	mTSH=hge->Texture_GetHeight(mTileSet);

	mRndBlockAnim=new Anim(AnimDesc(16,16,33,24,26,8));
	mCoinAnim=new Anim(AnimDesc(16,16,33,24,26,8,true,24,0,16));


	mLevelComplete = false;
	mMap = new TileMap(lvl_name);

	/*mMap=new TileMap(212,32,32,32);

	for(int i=0;i<212;i++)
	{
		mMap->addTile(i,SCREENH/32-2,0);
		mMap->addTile(i,SCREENH/32-1,0);
	}*/
	
	mSmallFont=new hgeFont("data//small_emulogic.fnt");
	mNormalFont=new hgeFont("data//normal_emulogic.fnt");
	mBigFont=new hgeFont("data//big_emulogic.fnt");

	mEditor=new TileMapEditor();
	mMode=MENUMODE;

	mMario=new Mario();
	mMario->mPos.x = 32+32+16;
	mMario->mPos.y = 9*32 + 12*32;
	mMario->mLeftHeaded = false;

	mNumScoreDisplays = 0;
	float yscroll = 9*32;
	mMap->setScroll(mMario->mPos.x-SCREENW/2,yscroll);
	mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);

	mMenuCursor = 0;
	mTimeLimit = 200;
	mTimeWarned = false;
	mStopGameTime = false;

	return true;
}

void Game::shutdown()
{
	delete mMap;
	delete mSmallFont;
	delete mBigFont;
	delete mNormalFont;
	delete mEditor;
	delete mMario;

	delete mCoinAnim;
	delete mRndBlockAnim;
}

void Game::update()
{
	if(hge->Input_GetKeyState(HGEK_TAB) && !mPrevTabState)
	{
		if(mMode==EDITORMODE)
		{
			mMode=GAMEMODE;
		}
		else if(mMode==GAMEMODE)
		{
			mEditor->setMap(mMap);
			mMode=EDITORMODE;
		}
	}

	mPrevTabState=hge->Input_GetKeyState(HGEK_TAB);
	

	if(mMode==MENUMODE)
	{
		mGameTime=mTimeLimit;
		mStopGameTime=false;

		if(hge->Input_KeyDown(HGEK_ESCAPE))
			exit = true;
		if(hge->Input_KeyDown(HGEK_DOWN))
		{
			mMenuCursor++;
			if(mMenuCursor==1)mMenuCursor++;

		}
		if(hge->Input_KeyDown(HGEK_UP))
		{
			mMenuCursor--;
			if(mMenuCursor==1)mMenuCursor--;
		}
		mMenuCursor+=3;
		mMenuCursor%=3;

		if(hge->Input_KeyDown(HGEK_ENTER))
		{
			if(mMenuCursor==0)
			{
				mMario->init();
				mMode = WORLD_SCREEN;
			}
			else
				mMode=EDITOR_INST;
		}

	}
	else if(mMode==EDITORMODE)
	{
		mTimePassedInsideEditor+=hge->Timer_GetDelta();

		if(hge->Input_KeyDown(HGEK_ESCAPE))
		{
			hge->Channel_StopAll();

			mMario->init();

			delete mMap;
			mMap = new TileMap(lvl_name);
			float yscroll = 9*32;
			mMap->setScroll(0,yscroll);
			mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);
			mMap->mScore =0;

			mMenuCursor = 0;
			
			mMode = MENUMODE;
		}

		mEditor->update();
	}
	else if(mMode==EDITOR_INST)
	{
		if(hge->Input_KeyDown(HGEK_ESCAPE))
			mMode=MENUMODE;
	}
	else if(mMode==WORLD_SCREEN)
	{
		static int timer=0;
		timer++;
		if(timer>128)
		{
			music_channel=hge->Stream_Play(mMusic,true,50);
			mMode=GAMEMODE;
			timer=0;
			mTimeWarned = 0;
			mTimePassedInsideEditor=0;
			mStopGameTime = false;
			mGameStartTime = hge->Timer_GetTime();
		}
	}
	else if(mMode==GAME_OVER)
	{
		static int timer=0;
		timer++;
		if(timer>256)
		{
			mMode=MENUMODE;
			delete mMap;
			mMap = new TileMap(lvl_name);
			float yscroll = 9*32;
			mMap->setScroll(0,yscroll);
			mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);
			mMap->mScore =0;
			timer=0;
		}
	}
	else if(mMode==GAMEMODE)
	{
		if(!mStopGameTime)
			mGameTime = (mMode==GAMEMODE)?(hge->Timer_GetTime()-mGameStartTime-mTimePassedInsideEditor)*3.5:mTimeLimit;

		if(hge->Input_KeyDown(HGEK_ESCAPE))
		{

			hge->Channel_StopAll();

			mMario->init();

			delete mMap;
			mMap = new TileMap(lvl_name);
			float yscroll = 9*32;
			mMap->setScroll(0,yscroll);
			mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);
			mMap->mScore =0;

			mMenuCursor = 0;

			mMode = MENUMODE;
		}

		mCoinAnim->update(hge->Timer_GetDelta());
		mRndBlockAnim->update(hge->Timer_GetDelta());

		mMario->update();
		mMap->update();

		float yboxmin=mMap->myScroll+SCREENH/2;
		float yboxmax=yboxmin+SCREENH/2-100;
		float yscroll=mMap->myScroll;
		
		if(mMario->mPos.y<yboxmin)
			yscroll-=(yboxmin-mMario->mPos.y);
		if(mMario->mPos.y>yboxmax)
			yscroll+=mMario->mPos.y-yboxmax;

		yscroll = 9*32;
		mMap->setScroll(mMario->mPos.x-SCREENW/2,yscroll);
		mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);

		for(int i=0;i<mNumScoreDisplays;i++)
			mScoreDisplays[i].update();
		//collect garbage
		for(int i=0;i<mNumScoreDisplays;i++)
		{
			if(mScoreDisplays[i].dead)
			{
				for(int j=i;j<mNumScoreDisplays-1;j++)
					mScoreDisplays[j]=mScoreDisplays[j+1];
				mNumScoreDisplays--;
			}
		}

		if(mMario->dead)
		{
			int l=--mMario->lives;
			if(mMario->lives<=0)
			{
				mMode=GAME_OVER;
				hge->Effect_Play(mGameOverSnd);
			}
			else
			{
				mMario->init();
				mMario->lives=l;

				mMode=WORLD_SCREEN;
				delete mMap;
				mMap = new TileMap(lvl_name);
				float yscroll = 9*32;
				mMap->setScroll(0,yscroll);
				mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);
				mMap->mScore =0;
			}
		}
		else if(!mMario->dying && mMario->mState!=Mario::FLAGPOLE && mMario->mState!=Mario::END_WALK
			&& mMario->mState!=Mario::END_FLAGDOWN_LEFT && mMario->mState!=Mario::END_FLAGDOWN_RIGHT && mMario->mState!=Mario::DISAPEARING
			&& mMario->mState!=Mario::DISAPEAR)
		{
			if(mGameTime>mTimeLimit-100 && !mTimeWarned)
			{
				hge->Channel_Pause(music_channel);
				static int one=0;
				if(!one)
				{
					hge->Effect_Play(mAlertSnd);
					one=1;
				}

				static int timer =0;timer++;
				if(timer>200)
				{
					hge->Channel_Resume(music_channel);
					hge->Channel_SetPitch(music_channel,1.25);
					mTimeWarned=true;one=0;
					timer=0;
				}
			}
		}
	}
	else if(mMode==LEVEL_COMPLETE)
	{
		credity-=1.2;
		float credit_height = 32*8;
		if(credity<-credit_height-20)
		{
			hge->Channel_StopAll();
			mMode = MENUMODE;
			mLevelComplete = false;
			mMario->init();

			delete mMap;
			mMap = new TileMap(lvl_name);
			float yscroll = 9*32;
			mMap->setScroll(0,yscroll);
			mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight-mMap->mScrollWinH);
			mMap->mScore =0;
		}
	}
}

void Game::render()
{
	if(mMode==MENUMODE)
	{
		mMap->render();
		mMario->render();

		hgeQuad quad;
		quad.tex = mTitleScreen;
		int x=64,y=32,w=512,h=5*32;

		quad.v[0].x=x;
		quad.v[0].y=y;
		quad.v[0].tx=0;
		quad.v[0].ty=0;

		quad.v[1].x=x+w;
		quad.v[1].y=y;
		quad.v[1].tx=1;
		quad.v[1].ty=0;

		quad.v[2].x=x+w;
		quad.v[2].y=y+h;
		quad.v[2].tx=1;
		quad.v[2].ty=1;

		quad.v[3].x=x;
		quad.v[3].y=y+h;
		quad.v[3].tx=0;
		quad.v[3].ty=1;

		quad.blend=BLEND_DEFAULT;
		for(int k=0;k<4;k++)
		{
			quad.v[k].z=0.5f;
		}
		quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = 0xffffffff;

		hge->Gfx_RenderQuad(&quad);

		int menuy=32+h+32+16;
		mNormalFont->printf(SCREENW/2,menuy,HGETEXT_CENTER,"1  PLAYER  GAME");
		mNormalFont->printf(SCREENW/2,menuy+32,HGETEXT_CENTER,"2  PLAYER  GAME");
		mNormalFont->printf(SCREENW/2,menuy+32*2,HGETEXT_CENTER,"#  ## EDITOR ##");

		x=SCREENW/2-mNormalFont->GetStringWidth("1  PLAYER  GAME")/2-32,
			y=menuy+mMenuCursor*32,w=16,h=16;
		float xx=16+8,yy=10*16,ww=8,hh=8;

		quad.v[0].x=x;
		quad.v[0].y=y;
		quad.v[0].tx=xx/hge->Texture_GetWidth(mItemsTex);
		quad.v[0].ty=yy/hge->Texture_GetHeight(mItemsTex);

		quad.v[1].x=x+w;
		quad.v[1].y=y;
		quad.v[1].tx=(xx+ww)/hge->Texture_GetWidth(mItemsTex);
		quad.v[1].ty=yy/hge->Texture_GetHeight(mItemsTex);


		quad.v[2].x=x+w;
		quad.v[2].y=y+h;
		quad.v[2].tx=(xx+ww)/hge->Texture_GetWidth(mItemsTex);
		quad.v[2].ty=(yy+hh)/hge->Texture_GetHeight(mItemsTex);

		quad.v[3].x=x;
		quad.v[3].y=y+h;
		quad.v[3].tx=xx/hge->Texture_GetWidth(mItemsTex);
		quad.v[3].ty=(yy+hh)/hge->Texture_GetHeight(mItemsTex);

		quad.tex = mItemsTex;

		quad.blend=BLEND_DEFAULT;
		for(int k=0;k<4;k++)
		{
			quad.v[k].z=0.5f;
		}
		quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = 0xffffffff;

		hge->Gfx_RenderQuad(&quad);
	}

	if(mMode==EDITORMODE)
	{
	
		mEditor->render();
	}

	if(mMode==GAMEMODE)
	{
		mMap->render();
		//goomba->render();
		mMario->render();

		for(int i=0;i<mNumScoreDisplays;i++)
		mScoreDisplays[i].render();
	}
	if(mMode==WORLD_SCREEN)
	{
		int x=SCREENW/2-64,
			y=SCREENH/2-16,w=24,h=24;
		float xx=6*16,yy=32,ww=16,hh=16;
		hgeQuad quad;

		quad.v[0].x=x;
		quad.v[0].y=y;
		quad.v[0].tx=xx/hge->Texture_GetWidth(mMarioTex);
		quad.v[0].ty=yy/hge->Texture_GetHeight(mMarioTex);

		quad.v[1].x=x+w;
		quad.v[1].y=y;
		quad.v[1].tx=(xx+ww)/hge->Texture_GetWidth(mMarioTex);
		quad.v[1].ty=yy/hge->Texture_GetHeight(mMarioTex);


		quad.v[2].x=x+w;
		quad.v[2].y=y+h;
		quad.v[2].tx=(xx+ww)/hge->Texture_GetWidth(mMarioTex);
		quad.v[2].ty=(yy+hh)/hge->Texture_GetHeight(mMarioTex);

		quad.v[3].x=x;
		quad.v[3].y=y+h;
		quad.v[3].tx=xx/hge->Texture_GetWidth(mMarioTex);
		quad.v[3].ty=(yy+hh)/hge->Texture_GetHeight(mMarioTex);

		quad.tex = mMarioTex;

		quad.blend=BLEND_DEFAULT;
		for(int k=0;k<4;k++)
		{
			quad.v[k].z=0.5f;
		}
		quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = 0xffffffff;

		hge->Gfx_RenderQuad(&quad);

		mSmallFont->printf(SCREENW/2,SCREENH/2-64,HGETEXT_CENTER,"WORLD   1-1 ");
		mSmallFont->printf(SCREENW/2,SCREENH/2-16,HGETEXT_CENTER,"    X    %d  ",mMario->lives);
	}
	if(mMode==GAME_OVER)
	{
		mNormalFont->printf(SCREENW/2,SCREENH/2,HGETEXT_CENTER,"GAME  OVER");
	}
	//render HUD
	if(mMode!=EDITOR_INST && mMode!=EDITORMODE)
	{
		mSmallFont->printf(32*2,0,0,"MARIO");
		mSmallFont->printf(32*2,16,0,"%06d",mMap->mScore);

	
		{
			float tx,ty,w,h,ww,hh;
			int texh=hge->Texture_GetHeight(mTileSet);
			int texw=hge->Texture_GetWidth(mTileSet);
			hgeQuad quad;
			ww=hh=12;
			int xx=32*7;
			int yy=16;
			mCoinAnim->getClip(&tx,&ty,&w,&h);
			quad.v[0].x=xx;
			quad.v[0].y=yy;
			quad.v[0].tx=tx/texw;
			quad.v[0].ty=ty/texh;

			quad.v[1].x=ww+xx;
			quad.v[1].y=yy;
			quad.v[1].tx=(tx+w)/texw;
			quad.v[1].ty=ty/texh;

			quad.v[2].x=ww+xx;
			quad.v[2].y=yy+hh;
			quad.v[2].tx=(tx+w)/texw;
			quad.v[2].ty=(ty+h)/texh;

			quad.v[3].x=xx;
			quad.v[3].y=yy+hh;
			quad.v[3].tx=tx/texw;
			quad.v[3].ty=(ty+h)/texh;

			quad.tex=mTileSet;
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
			mSmallFont->printf(32*7+16,16,0,"x%02d",mMario->mCoins);
		}

		mSmallFont->printf(32*12,0,0,"WORLD");
		mSmallFont->printf(32*12+24,16,HGETEXT_CENTER,"1-1",mMap->mScore);

		 
		mSmallFont->printf(32*17,0,0,"TIME");
		mSmallFont->printf(32*17+24,16,HGETEXT_CENTER,"%03d",(mTimeLimit-mGameTime)>0?mTimeLimit-mGameTime:0);
	}
	
	if(mMode == EDITOR_INST)
	{
		mMap->render();

		mBigFont->printf(SCREENW/2,0,HGETEXT_CENTER,"Editor Commands");
		float left_tabx = 32;
		float y=64;
		mSmallFont->SetColor(0xff000000);
		mSmallFont->printf(left_tabx,y,0,"+ TAB - Toggle editor mode while playing");
		mSmallFont->printf(left_tabx,y+16,0,"+ Left mouse button - Fill tile");
		mSmallFont->printf(left_tabx,y+16*2,0,"+ Right mouse button - Empty tile");
		mSmallFont->printf(left_tabx,y+16*3,0,"+ Middle mouse button - Select tile (used to create enemies)");
		mSmallFont->printf(left_tabx,y+16*4,0,"+ Ctrl+LMB - Select tile");
		mSmallFont->printf(left_tabx,y+16*5,0,"+ WASD - Map navigation");
		mSmallFont->printf(left_tabx,y+16*6,0,"+ C - Selected tile is the castle door");
		mSmallFont->printf(left_tabx,y+16*7,0,"+ M+LMB+Question_Tile selected - fill with a question");
		mSmallFont->printf(left_tabx,y+16*8,0,"tile with a mushroom inside");
		mSmallFont->printf(left_tabx,y+16*9,0,"+ Ctrl+G - Toggle Grid");

		mSmallFont->printf(left_tabx,y+16*10,0,"+ MAP tab");
		mSmallFont->printf(left_tabx+32,y+16*11,0,"New Map - Creates a new map with 'Name'");
		mSmallFont->printf(left_tabx+32,y+16*12,0,"Load Map - Loads the 'Name' map ");
		mSmallFont->printf(left_tabx+32,y+16*13,0,"Save Map - Save current map in 'Name' file ");
		mSmallFont->printf(left_tabx,y+16*14,0,"+ TILESET tab - Select tiles by clicking on them");
		mSmallFont->printf(left_tabx,y+16*15,0,"+ ENTITY tab - Creates Entities on selected tiles");
		mSmallFont->printf(left_tabx+32,y+16*16,0,"Hold Right to change the initial direction of enemy");


		mSmallFont->SetColor(0xffffffff);
	}
	if(mMode==LEVEL_COMPLETE)
	{
		mSmallFont->printf(SCREENW/2,credity,HGETEXT_CENTER,"Super Mario Bros clone made By:");
		mNormalFont->printf(SCREENW/2,credity+32,HGETEXT_CENTER,"BEqqi mohammed");
		mNormalFont->printf(SCREENW/2,credity+32*8,HGETEXT_CENTER,"You Scored %d (Not much)",mMap->mScore);
	}
}

void Game::addScoreDisplay(float xx,float yy,int val)
{
	if(mNumScoreDisplays>=MAX_SCORE_DISPLAYS)
		return;
	mScoreDisplays[mNumScoreDisplays++] = ScoreDisplay(xx,yy,val);
}

ScoreDisplay::ScoreDisplay(float xx,float yy,int val)
{
	x=xx;
	y=yy;
	value= val;
	time = 0.456;
	timer = 0;
	dead = false;
	vy=-3;
}

void ScoreDisplay::update()
{
	if(dead)return;

	timer += Game::getInstance()->hge->Timer_GetDelta();
	if(timer>time)
	{
		dead = true;
	}
	y+=vy;
}

void ScoreDisplay::render()
{
	if(dead)return;
	char val[256];
	sprintf(val,"%d",value);
	Game::getInstance()->mSmallFont->Render(x,y,HGETEXT_LEFT,val);
}