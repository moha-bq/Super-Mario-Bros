/*THIS IS A MODIFIED VERSION OF:*/
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Tutorial 06 - Creating menus
*/

#include "MenuItem.h"
#include "Game.h"
// This is a GUI control constructor,
// we should initialize all the variables here
hgeGUIMenuItem::hgeGUIMenuItem(int _id, float _x, float _y, char *_title,DWORD bc,DWORD hbc)
{
	float w;
	
	id=_id;
	fnt=Game::getInstance()->mSmallFont;
	delay=0;
	title=_title;

	offset=0.0f;

	bStatic=false;
	bVisible=true;
	bEnabled=true;

	hboxColor=hbc;
	boxColor=bc;

	w=fnt->GetStringWidth(title);
	rect.Set(_x, _y, _x+w,_y+fnt->GetHeight());

	box.v[0].x=_x-5;box.v[0].y=_y-5;
	box.v[1].x=_x+w+5;box.v[1].y=_y-5;
	box.v[2].x=_x+w+5;box.v[2].y=_y+fnt->GetHeight()+5;
	box.v[3].x=_x-5;box.v[3].y=_y+fnt->GetHeight()+5;
	box.blend=BLEND_DEFAULT;
	box.tex=0;
	for(int k=0;k<4;k++)
	{
		box.v[k].z=0.5f;
		box.v[k].col=bc;
	}
}

// This method is called when the control should be rendered
void hgeGUIMenuItem::Render()
{
	//fnt->Render(rect.x1+offset+3, rect.y1+3, HGETEXT_LEFT, title);
	Game::getInstance()->hge->Gfx_RenderQuad(&box);
	fnt->Render(rect.x1, rect.y1+offset, HGETEXT_LEFT, title);
}

// This method is called each frame,
// we should update the animation here
void hgeGUIMenuItem::Update(float dt)
{
}

// This method is called when the GUI
// is about to appear on the screen
void hgeGUIMenuItem::Enter()
{
}

// This method is called when the GUI
// is about to disappear from the screen
void hgeGUIMenuItem::Leave()
{
}

// This method is called to test whether the control
// have finished it's Enter/Leave animation
bool hgeGUIMenuItem::IsDone()
{
	return true;
}

// This method is called when the control
// receives or loses keyboard input focus
void hgeGUIMenuItem::Focus(bool bFocused)
{
	if(bFocused)
	{
		for(int k=0;k<4;k++)
		{
		box.v[k].col=hboxColor;
		}
	}
	else
	{
		for(int k=0;k<4;k++)
		{
		box.v[k].col=boxColor;
		}
	}
}

// This method is called to notify the control
// that the mouse cursor has entered or left it's area
void hgeGUIMenuItem::MouseOver(bool bOver)
{
	if(bOver)
	{
		for(int k=0;k<4;k++)
		{
		box.v[k].col=hboxColor;
		}
	}
	else
	{
		for(int k=0;k<4;k++)
		{
		box.v[k].col=boxColor;
		}
	}
}

// This method is called to notify the control
// that the left mouse button state has changed.
// If it returns true - the caller will receive
// the control's ID
bool hgeGUIMenuItem::MouseLButton(bool bDown)
{
	if(!bDown)
	{
		offset=0;
		return true;
	}
	else 
	{
		offset=4;
		return false;
	}
}

// This method is called to notify the
// control that a key has been clicked.
// If it returns true - the caller will
// receive the control's ID
bool hgeGUIMenuItem::KeyClick(int key, int chr)
{
	if(key==HGEK_ENTER || key==HGEK_SPACE)
	{
		MouseLButton(true);
		return MouseLButton(false);
	}

	return false;
}
