#include "TileMapEditor.h"

#include "Game.h"
#include "FormInput.h"
#include "MenuItem.h"
#include "TileMap.h"
#include "Tile.h"
#include "AnimDesc.h"
#include "TileSetBrowser.h"
#include "Anim.h"

#include <hgegui.h>
#include <hgeguictrls.h>


int PANELH=50;

#define MIN(x,y)((x>y)?y:x)

void translateQuad(hgeQuad* quad,float x,float y)
{
	for(int i=0;i<4;i++)
	{
		quad->v[i].x+=x;
		quad->v[i].y+=y;
	}
}

void setQuadPos(hgeQuad* quad,float x,float y)
{
	float w=quad->v[1].x-quad->v[0].x;
	float h=quad->v[2].y-quad->v[1].y;


	quad->v[0].x=x;quad->v[0].y=y;
	quad->v[1].x=x+w;quad->v[1].y=y;
	quad->v[2].x=x+w;quad->v[2].y=y+h;
	quad->v[3].x=x;quad->v[3].y=y+h;
}

TileMapEditor::TileMapEditor(void)
{
	gui=new hgeGUI();
	mNewMapPrompt=false;
	mMap=0;
	mShowGrid=false;
	mShowMouseTile=false;
	mxSelected=-1;
	mySelected=-1;

	createGUI();
}


TileMapEditor::~TileMapEditor(void)
{
	delete gui;
}

void TileMapEditor::update()
{
	Game::getInstance()->hge->Input_GetMousePos(&mx,&my);

	if( (mx>SCREENW-40 && my>PANELH) || Game::getInstance()->hge->Input_GetKeyState(HGEK_D))
	{
		mMap->addScroll(SCROLLSPEED,0);
		mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth+PANELW-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight+PANELH-mMap->mScrollWinH);
	}
	else if((mx<PANELW+40 && mx>PANELW && my>PANELH)  || Game::getInstance()->hge->Input_GetKeyState(HGEK_Q))
	{
		mMap->addScroll(-SCROLLSPEED,0);
		mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth+PANELW-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight+PANELH-mMap->mScrollWinH);

	}
	if((my>SCREENH-40 && mx>PANELW)  || Game::getInstance()->hge->Input_GetKeyState(HGEK_S))
	{
		mMap->addScroll(0,SCROLLSPEED);
		mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth+PANELW-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight+PANELH-mMap->mScrollWinH);

	}
	else if((my<PANELH+40 && my>PANELH && mx>PANELW)  || Game::getInstance()->hge->Input_GetKeyState(HGEK_Z))
	{
		mMap->addScroll(0,-SCROLLSPEED);
		mMap->clampScroll(0,0,mMap->mWidth*mMap->mTileWidth+PANELW-mMap->mScrollWinW,mMap->mHeight*mMap->mTileHeight+PANELH-mMap->mScrollWinH);

	}
	if(!mNewMapPrompt)
	{
		if(Game::getInstance()->hge->Input_GetKeyState(HGEK_LBUTTON) && !Game::getInstance()->hge->Input_GetKeyState(HGEK_CTRL))
		{
			int x=mMap->xWinToMap(mx-PANELW);
			int y=mMap->yWinToMap(my-PANELH);

			if(mx>PANELW && my>PANELH && x>=0 && y>=0  && x<mMap->mWidth && y<mMap->mHeight)
			{
					if(mCurrTileId!=-1)
						mMap->addTile(x,y,mCurrTileId);
				
					if(mCurrTileId==24)
					{
						if(Game::getInstance()->hge->Input_GetKeyState(HGEK_M))//if mushroom
						{
							mMap->addRedMushroomTile(x+y*mMap->mWidth);
						}
					}
			}

		}
		if(Game::getInstance()->hge->Input_GetKeyState(HGEK_RBUTTON))
		{
			int x=mMap->xWinToMap(mx-PANELW);
			int y=mMap->yWinToMap(my-PANELH);

			if(mx>PANELW && my>PANELH &&x>=0 && y>=0  && x<mMap->mWidth && y<mMap->mHeight)
			{
				
					mMap->addTile(x,y,-1);
			}

		}
	}

	if((Game::getInstance()->hge->Input_GetKeyState(HGEK_MBUTTON) && !mPrevWheelState)
		|| (Game::getInstance()->hge->Input_KeyDown(HGEK_LBUTTON) && Game::getInstance()->hge->Input_GetKeyState(HGEK_CTRL)) )
	{
		int x=mMap->xWinToMap(mx-PANELW);
		int y=mMap->yWinToMap(my-PANELH);

		if(x>=0 && y>=0  && x<mMap->mWidth && y<mMap->mHeight)
		{
			if(mxSelected==x && mySelected==y)
			{
				mxSelected=-1;
				mySelected=-1;
			}
			else
			{
				mxSelected=x;
				mySelected=y;
			}
		}
	}
	if(Game::getInstance()->hge->Input_GetKeyState(HGEK_CTRL) && Game::getInstance()->hge->Input_GetKeyState(HGEK_G) && !mPrevGState)
	{
		mShowGrid=!mShowGrid;
	}

	if(Game::getInstance()->hge->Input_KeyDown(HGEK_C))
	{
		if(mxSelected!=-1 && mySelected!=-1)
		{
			Game::getInstance()->mMap->mxCastle = mxSelected;
		}
	}

	mPrevGState=Game::getInstance()->hge->Input_GetKeyState(HGEK_G);
	mPrevWheelState=Game::getInstance()->hge->Input_GetKeyState(HGEK_MBUTTON);

	handleUI(gui->Update(Game::getInstance()->hge->Timer_GetDelta()));
}

void TileMapEditor::render()
{
	if(mMap)
		mMap->render(PANELW,PANELH);

	if(mShowMouseTile && !mNewMapPrompt)
	{
		if(mx>PANELW && my>PANELH)
		{
			setQuadPos(&mMouseTile,mx,my);
			Game::getInstance()->hge->Gfx_RenderQuad(&mMouseTile);
		}
	}

	//draw grid
	if(mShowGrid)
	{
		for(int i=1;i<=mMap->mWidth;i++)
		{
			Game::getInstance()->hge->Gfx_RenderLine((i*mMap->mTileWidth-mMap->mxScroll>0)?i*mMap->mTileWidth-mMap->mxScroll+PANELW:PANELW,PANELH,
				(i*mMap->mTileWidth-mMap->mxScroll>0)?i*mMap->mTileWidth-mMap->mxScroll+PANELW:PANELW,mMap->mScrollWinH+PANELH,
				0xff303030);
		}
		for(int i=1;i<=mMap->mHeight;i++)
		{
			Game::getInstance()->hge->Gfx_RenderLine(PANELW,(i*mMap->mTileHeight-mMap->myScroll>0)?i*mMap->mTileHeight-mMap->myScroll+PANELH:PANELH,
				mMap->mScrollWinW+PANELW,(i*mMap->mTileHeight-mMap->myScroll>0)?i*mMap->mTileHeight-mMap->myScroll+PANELH:PANELH,
				0xff303030);
		}
	}

	//draw selection box
	if(mx>PANELW && my>PANELH)
	{
		int x=(mMap->xWinToMap(mx-PANELW))*mMap->mTileWidth+PANELW-mMap->mxScroll;
		int y=(mMap->yWinToMap(my-PANELH))*mMap->mTileHeight+PANELH-mMap->myScroll;
		if(x<mMap->mWidth*mMap->mTileWidth+PANELW && y<mMap->mHeight*mMap->mTileHeight+PANELH)
		{
			Game::getInstance()->hge->Gfx_RenderLine(x,y,x+mMap->mTileWidth,y);
			Game::getInstance()->hge->Gfx_RenderLine(x+mMap->mTileWidth,y,x+mMap->mTileWidth,y+mMap->mTileHeight);
			Game::getInstance()->hge->Gfx_RenderLine(x+mMap->mTileWidth,y+mMap->mTileHeight,x,y+mMap->mTileHeight);
			Game::getInstance()->hge->Gfx_RenderLine(x,y+mMap->mTileHeight,x,y);
		}
	}

	//draw selected box
	if(mxSelected!=-1 && mySelected!=-1)
	{
		int x=mxSelected*mMap->mTileWidth+PANELW-mMap->mxScroll;
		int y=mySelected*mMap->mTileHeight-mMap->myScroll+PANELH;

		Game::getInstance()->hge->Gfx_RenderLine(x,y,x+mMap->mTileWidth,y,0xffff0000);
		Game::getInstance()->hge->Gfx_RenderLine(x+mMap->mTileWidth,y,x+mMap->mTileWidth,y+mMap->mTileHeight,0xffff0000);
		Game::getInstance()->hge->Gfx_RenderLine(x+mMap->mTileWidth,y+mMap->mTileHeight,x,y+mMap->mTileHeight,0xffff0000);
		Game::getInstance()->hge->Gfx_RenderLine(x,y+mMap->mTileHeight,x,y,0xffff0000);
	}

	//draw separators
	Game::getInstance()->hge->Gfx_RenderLine(PANELW,0,PANELW,SCREENH);
	Game::getInstance()->hge->Gfx_RenderLine(PANELW,PANELH,SCREENW,PANELH);

	if(mNewMapPrompt)
	{
		Game::getInstance()->hge->Gfx_RenderQuad(&mNewMapBox);	
	}

	gui->Render();
}

void TileMapEditor::createGUI()
{
	hgeGUIButton *button;
	hgeGUISlider *slider;
	hgeGUIText	 *text;

	gui->AddCtrl(new hgeGUIMenuItem(TAB_MAP,10,0,"MAP",0xaaff0000));
	gui->AddCtrl(new hgeGUIMenuItem(TAB_TILESET,Game::getInstance()->mSmallFont->GetStringWidth("MAP")+15,0,"TILESET",0xaaff0000));
	gui->AddCtrl(new hgeGUIMenuItem(TAB_ENTITY,Game::getInstance()->mSmallFont->GetStringWidth("MAPTILESET")+20,0,"ENTITY",0xaaff0000));

	gui->AddCtrl(new hgeGUIMenuItem(BTN_NEWMAP,10,Game::getInstance()->mSmallFont->GetHeight()+70,"NEW MAP"));
	gui->AddCtrl(new hgeGUIMenuItem(BTN_LOADMAP,10,2*Game::getInstance()->mSmallFont->GetHeight()+80,"LOAD MAP"));
	gui->AddCtrl(new hgeGUIMenuItem(BTN_SAVEMAP,10,3*Game::getInstance()->mSmallFont->GetHeight()+90,"SAVE MAP"));
	gui->AddCtrl(new Forminput(INP_MAPNAME,10,60,150,"NAME:"));

	gui->AddCtrl(new Forminput(INP_MAPW,SCREENW/2-200+10,SCREENH/2-200+10,250,"WIDTH:"));
	gui->AddCtrl(new Forminput(INP_MAPH,SCREENW/2-200+10,SCREENH/2-200+20+Game::getInstance()->mSmallFont->GetHeight(),250,"HEIGHT:"));
	gui->AddCtrl(new Forminput(INP_MAPTW,SCREENW/2-200+10,SCREENH/2-200+30+2*Game::getInstance()->mSmallFont->GetHeight(),250,"TILE WIDTH:"));
	gui->AddCtrl(new Forminput(INP_MAPTH,SCREENW/2-200+10,SCREENH/2-200+30+3*Game::getInstance()->mSmallFont->GetHeight(),250,"TILE HEIGHT:"));

	gui->AddCtrl(new hgeGUIMenuItem(BTN_GONEWMAP,SCREENW/2-60,SCREENH/2+200-40,"OK"));
	gui->AddCtrl(new hgeGUIMenuItem(BTN_CANCELNEWMAP,SCREENW/2+60,SCREENH/2+200-40,"Cancel"));


	//tileset tab

	TileSetBrowser *brs=new TileSetBrowser(TSBROWSER,180,0,Game::getInstance()->mTSW,Game::getInstance()->mTSH);
	gui->AddCtrl(brs);


	//Entity Tab
	gui->AddCtrl(new hgeGUIMenuItem(BTN_NEWGOOMBA,10,50,"NEW GOOMBA"));
	gui->AddCtrl(new hgeGUIMenuItem(BTN_NEWKOOPA,10,Game::getInstance()->mSmallFont->GetHeight()+60,"NEW KOOPA"));
	gui->AddCtrl(new hgeGUIMenuItem(BTN_NEWPIRANHA,10,2*Game::getInstance()->mSmallFont->GetHeight()+60+10,"NEW PIRANHA"));

	//////////////
	enableNewMapBox(false);

	toggleTab(TAB_MAP);

	mNewMapBox.v[0].x=SCREENW/2-200;mNewMapBox.v[0].y=SCREENH/2-200;
	mNewMapBox.v[1].x=SCREENW/2+200;mNewMapBox.v[1].y=SCREENH/2-200;
	mNewMapBox.v[2].x=SCREENW/2+200;mNewMapBox.v[2].y=SCREENH/2+200;
	mNewMapBox.v[3].x=SCREENW/2-200;mNewMapBox.v[3].y=SCREENH/2+200;
	mNewMapBox.blend=BLEND_DEFAULT;
	mNewMapBox.tex=0;
	for(int k=0;k<4;k++)
	{
		mNewMapBox.v[k].z=0.5f;
	}
}

void TileMapEditor::handleUI(int id)
{
if(mNewMapPrompt)
	{
		switch(id)
		{
		case BTN_CANCELNEWMAP:
			enableNewMapBox(false);
			break;
		case BTN_GONEWMAP:

			int w=atoi(getInputText(INP_MAPW));
			int h=atoi(getInputText(INP_MAPH));
			int tw=atoi(getInputText(INP_MAPTW));
			int th=atoi(getInputText(INP_MAPTH));

			setMapx(new TileMap(w,h,tw,th));
			mMapPath=getInputText(INP_MAPNAME);

			enableNewMapBox(false);
			break;

		}
	}
	else
	{
		switch(id)
		{
		case BTN_NEWMAP:
			enableNewMapBox(true);
			break;
		case BTN_LOADMAP:
			setMapx(new TileMap(getInputText(INP_MAPNAME)));
			mMapPath=getInputText(INP_MAPNAME);
			break;
		case BTN_SAVEMAP:
			mMap->saveToFile(getInputText(INP_MAPNAME));
			break;
		case TAB_MAP:
		case TAB_TILESET:
		case TAB_ENTITY:
			toggleTab(id);
			break;
		case TSBROWSER:
			{
				TileSetBrowser* tsb=(TileSetBrowser*)gui->GetCtrl(TSBROWSER);

				int selected=tsb->selected;
				int x=(selected%int(33))*16;
				int y=(selected/int(33))*16;

				int w=16,h=16;

				mMouseTile.v[0].x=0;mMouseTile.v[0].y=0;
				mMouseTile.v[0].tx=(float)(x)/Game::getInstance()->mTSW;
				mMouseTile.v[0].ty=(float)(y)/Game::getInstance()->mTSH;

				mMouseTile.v[1].x=w;mMouseTile.v[1].y=0;
				mMouseTile.v[1].tx=(float)(x+Game::getInstance()->mTSW)/Game::getInstance()->mTSW;
				mMouseTile.v[1].ty=(float)(y)/Game::getInstance()->mTSH;

				mMouseTile.v[2].x=w;mMouseTile.v[2].y=h;
				mMouseTile.v[2].tx=(float)(x+Game::getInstance()->mTSW)/Game::getInstance()->mTSW;
				mMouseTile.v[2].ty=(float)(y+Game::getInstance()->mTSH)/Game::getInstance()->mTSH;

				mMouseTile.v[3].x=0;mMouseTile.v[3].y=h;
				mMouseTile.v[3].tx=(float)(x)/Game::getInstance()->mTSW;
				mMouseTile.v[3].ty=(float)(y+Game::getInstance()->mTSH)/Game::getInstance()->mTSH;

				mMouseTile.blend=BLEND_DEFAULT;
				mMouseTile.tex=Game::getInstance()->mTileSet;

				for(int k=0;k<4;k++)
				{
					mMouseTile.v[k].z=0.5f;
				}
				mCurrTileId=selected;
				mShowMouseTile=true;
			}
			break;
		case BTN_NEWGOOMBA:
			if(mxSelected!=-1 && mySelected!=-1)
			{
				mMap->addGoomba(Vector2(mxSelected*mMap->mTileWidth,mySelected*mMap->mTileHeight),(Game::getInstance()->hge->Input_GetKeyState(HGEK_RIGHT)?1:-1));
			}
			break;
		case BTN_NEWKOOPA:
			if(mxSelected!=-1 && mySelected!=-1)
			{
				mMap->addTurtle(Vector2(mxSelected*mMap->mTileWidth,mySelected*mMap->mTileHeight-16),(Game::getInstance()->hge->Input_GetKeyState(HGEK_RIGHT)?1:-1));
				//-16 because turtle is a little bit hight than others!
			}
			break;
		case BTN_NEWPIRANHA:
			if(mxSelected!=-1 && mySelected!=-1)
			{
				mMap->addPiranha(Vector2((mxSelected+0.5)*mMap->mTileWidth,mySelected*mMap->mTileHeight));
			}
			break;
		}
	}
}

void TileMapEditor::enableNewMapBox(bool e)
{
	if(e)
	{
		mNewMapPrompt=true;
		gui->ShowCtrl(INP_MAPW,true);
		gui->ShowCtrl(INP_MAPH,true);
		gui->ShowCtrl(INP_MAPTW,true);
		gui->ShowCtrl(INP_MAPTH,true);
		gui->ShowCtrl(BTN_GONEWMAP,true);
		gui->ShowCtrl(BTN_CANCELNEWMAP,true);

		gui->EnableCtrl(INP_MAPW,true);
		gui->EnableCtrl(INP_MAPH,true);
		gui->EnableCtrl(INP_MAPTW,true);
		gui->EnableCtrl(INP_MAPTH,true);
		gui->EnableCtrl(BTN_GONEWMAP,true);
		gui->EnableCtrl(BTN_CANCELNEWMAP,true);
	}
	else
	{
		mNewMapPrompt=false;
		gui->ShowCtrl(INP_MAPW,false);
		gui->ShowCtrl(INP_MAPH,false);
		gui->ShowCtrl(INP_MAPTW,false);
		gui->ShowCtrl(INP_MAPTH,false);
		gui->ShowCtrl(BTN_GONEWMAP,false);
		gui->ShowCtrl(BTN_CANCELNEWMAP,false);

		gui->EnableCtrl(INP_MAPW,false);
		gui->EnableCtrl(INP_MAPH,false);
		gui->EnableCtrl(INP_MAPTW,false);
		gui->EnableCtrl(INP_MAPTH,false);
		gui->EnableCtrl(BTN_GONEWMAP,false);
		gui->EnableCtrl(BTN_CANCELNEWMAP,false);

	}
}

void TileMapEditor::hideMapTabElt()
{
	gui->EnableCtrl(INP_MAPNAME,false);
	gui->EnableCtrl(BTN_NEWMAP,false);
	gui->EnableCtrl(BTN_LOADMAP,false);
	gui->EnableCtrl(BTN_SAVEMAP,false);


	gui->ShowCtrl(INP_MAPNAME,false);
	gui->ShowCtrl(BTN_NEWMAP,false);
	gui->ShowCtrl(BTN_LOADMAP,false);
	gui->ShowCtrl(BTN_SAVEMAP,false);
}
void TileMapEditor::showMapTabElt()
{
	gui->ShowCtrl(INP_MAPNAME,true);
	gui->ShowCtrl(BTN_NEWMAP,true);
	gui->ShowCtrl(BTN_LOADMAP,true);
	gui->ShowCtrl(BTN_SAVEMAP,true);

	gui->EnableCtrl(INP_MAPNAME,true);
	gui->EnableCtrl(BTN_NEWMAP,true);
	gui->EnableCtrl(BTN_LOADMAP,true);
	gui->EnableCtrl(BTN_SAVEMAP,true);
}
void TileMapEditor::hideTileSetTabElt()
{
	gui->ShowCtrl(TSBROWSER,false);
	gui->EnableCtrl(TSBROWSER,false);
	PANELH=50;
}
void TileMapEditor::showTileSetTabElt()
{
	gui->EnableCtrl(TSBROWSER,true);
	gui->ShowCtrl(TSBROWSER,true);
	PANELH=120;

}

void TileMapEditor::hideEntityTabElt()
{
	gui->EnableCtrl(BTN_NEWGOOMBA,false);
	gui->ShowCtrl(BTN_NEWGOOMBA,false);

	gui->EnableCtrl(BTN_NEWKOOPA,false);
	gui->ShowCtrl(BTN_NEWKOOPA,false);

	gui->EnableCtrl(BTN_NEWPIRANHA,false);
	gui->ShowCtrl(BTN_NEWPIRANHA,false);

}

void TileMapEditor::showEntityTabElt()
{
	gui->EnableCtrl(BTN_NEWGOOMBA,true);
	gui->EnableCtrl(BTN_NEWKOOPA,true);

	gui->ShowCtrl(BTN_NEWGOOMBA,true);
	gui->ShowCtrl(BTN_NEWKOOPA,true);

	gui->EnableCtrl(BTN_NEWPIRANHA,true);
	gui->ShowCtrl(BTN_NEWPIRANHA,true);
}


void TileMapEditor::toggleTab(int tab)
{
	if(tab==TAB_MAP)
	{
		showMapTabElt();
		hideTileSetTabElt();
		hideEntityTabElt();
	}
	else if(tab==TAB_TILESET)
	{
		hideMapTabElt();
		hideEntityTabElt();
		showTileSetTabElt();
	}
	else if(tab==TAB_ENTITY)
	{
		hideMapTabElt();
		hideTileSetTabElt();
		showEntityTabElt();
	}
}

void TileMapEditor::setMapx(TileMap* map)
{
	if(mMap && mMap!=map)
	{
		delete mMap;
	}

	mMapPath="";
	mMap=map;
	//mMap->setScroll(0,0);
	Game::getInstance()->mMap=map;
}