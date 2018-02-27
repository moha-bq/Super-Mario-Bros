#pragma once
#include <hge.h>
#include <string>

#define PANELW 128

extern int PANELH;

#define SCROLLSPEED 12


#define BTN_NEWMAP 1 
#define INP_MAPW 2
#define INP_MAPH 3
#define INP_MAPNAME 4
#define BTN_GONEWMAP 5
#define INP_MAPTW 6
#define INP_MAPTH 7
#define BTN_LOADMAP 8 
#define BTN_SAVEMAP 9
#define BTN_CANCELNEWMAP 10
#define TAB_MAP 11
#define TAB_TILESET 12
#define TSBROWSER 13
#define TAB_ENTITY 14
#define BTN_NEWGOOMBA 15
#define BTN_NEWKOOPA 16
#define BTN_NEWPIRANHA 17


#define buttonGetState(id)		((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define getInputText(id)			((Forminput*)gui->GetCtrl(id))->getText()
#define buttonSetState(id,b)	((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define sliderSetValue(id,f)	((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define sliderGetValue(id)		((hgeGUISlider*)gui->GetCtrl(id))->GetValue()

class hgeGUI;
class TileMap;

class TileMapEditor
{
public:
	TileMapEditor(void);
	~TileMapEditor(void);

	void update();
	void render();

	void setMapx(TileMap* map);
	void setMap(TileMap* map){mMap=map;}

	void createGUI();
	void enableNewMapBox(bool e);

	void toggleTab(int tab);
	void hideMapTabElt();
	void showMapTabElt();
	void hideTileSetTabElt();
	void showTileSetTabElt();
	void hideEntityTabElt();
	void showEntityTabElt();

	void handleUI(int id);

	hgeGUI* gui;

	hgeQuad mNewMapBox;

	bool mNewMapPrompt;

	TileMap* mMap;
	std::string mMapPath;
	
	bool mShowGrid;
	bool mShowMouseTile;

	int mPrevGState;

	int mPrevWheelState;

	int mCurrTileId;

	int mxSelected;
	int mySelected;

	float mx,my;//mouse pos

	hgeQuad mMouseTile;
};

