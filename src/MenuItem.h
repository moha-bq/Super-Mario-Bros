/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Tutorial 06 - Creating menus
*/

// In menuitem.cpp/h we define the
// behaviour of our custom GUI control

#include <hge.h>
#include <hgegui.h>
#include <hgefont.h>
#include <hgecolor.h>


class hgeGUIMenuItem : public hgeGUIObject
{
public:
	hgeGUIMenuItem(int id, float x, float y, char *title,DWORD bc=0xff909090,DWORD hbc=0xfffcd63c);

	virtual void	Render();
	virtual void	Update(float dt);

	virtual void	Enter();
	virtual void	Leave();
	virtual bool	IsDone();
	virtual void	Focus(bool bFocused);
	virtual void	MouseOver(bool bOver);

	virtual bool	MouseLButton(bool bDown);
	virtual bool	KeyClick(int key, int chr);

private:
	hgeFont		*fnt;
	float		delay;
	char		*title;

	DWORD     boxColor;
	DWORD     hboxColor;
	float		offset;
	hgeQuad     box;
};
