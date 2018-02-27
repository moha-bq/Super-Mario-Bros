#pragma once
#include <hgegui.h>
#include <hgesprite.h>


class TileSet;

class TileSetBrowser:public hgeGUIObject
{
public:
	TileSetBrowser(int id, float x, float y, float w, float h);
	~TileSetBrowser();

	virtual void	Render();
	virtual bool	MouseMove(float x, float y);
	virtual bool	MouseLButton(bool bDown);

	void getSelected(hgeQuad* q);

public:

	float xScroll,yScroll;

	hgeQuad         quad;
	float			mx, my;
	bool			bPressed;

	int selected;
};

