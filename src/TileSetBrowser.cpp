#include "TileSetBrowser.h"
#include "Game.h"

#define MIN(x,y)((x>y)?y:x)

TileSetBrowser::TileSetBrowser(int _id, float x, float y, float _w, float _h)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;

	rect.Set(x, y, x+_w, y+_h);
}


TileSetBrowser::~TileSetBrowser(void)
{
}

void TileSetBrowser::Render()
{
		int x=rect.x1,y=rect.y1;
		int w=rect.x2-rect.x1,h=rect.y2-rect.y1;

		quad.v[0].x=x;quad.v[0].y=y;
		quad.v[0].tx=0;
		quad.v[0].ty=0;

		quad.v[1].x=x+w;quad.v[1].y=y;
		quad.v[1].tx=1;
		quad.v[1].ty=0;

		quad.v[2].x=x+w;quad.v[2].y=y+h;
		quad.v[2].tx=1;
		quad.v[2].ty=1;

		quad.v[3].x=x;quad.v[3].y=y+h;
		quad.v[3].tx=0;
		quad.v[3].ty=1;

		quad.blend=BLEND_DEFAULT;
		quad.tex=Game::getInstance()->mTileSet;

		for(int k=0;k<4;k++)
		{
			quad.v[k].z=0.5f;
		}
		quad.v[0].col = 
		quad.v[1].col = 
		quad.v[2].col = 
		quad.v[3].col = 0xffffffff;

		Game::getInstance()->hge->Gfx_RenderQuad(&quad);

}

bool TileSetBrowser::MouseMove(float x, float y)
{
	mx=x; my=y;
	return false;
}

bool TileSetBrowser::MouseLButton(bool bDown)
{
		if(bDown)
		{
			float cw=rect.x2-rect.x1;
			float ch=rect.y2-rect.y1;

			if(mx<0 || mx>cw-1 || my<0 || my>ch-1)return false;

			int xcell=int(mx)/16;
			int ycell=int(my)/16;

			selected=ycell*33+xcell;

			return true;
		}
	return false;
}

void TileSetBrowser::getSelected(hgeQuad* q)
{
	float cw=rect.x2-rect.x1;
	float ch=rect.y2-rect.y1;

	int x=(selected%int(33))*16;
	int y=(selected/int(33))*16;

	int w=16,h=16;

	hgeQuad quad=*q;
	quad.v[0].x=0;quad.v[0].y=0;
	quad.v[0].tx=(float)(x)/cw;
	quad.v[0].ty=(float)(y)/ch;

	quad.v[1].x=w;quad.v[1].y=0;
	quad.v[1].tx=(float)(x+16)/cw;
	quad.v[1].ty=(float)(y)/ch;

	quad.v[2].x=w;quad.v[2].y=h;
	quad.v[2].tx=(float)(x+16)/cw;
	quad.v[2].ty=(float)(y+16)/ch;

	quad.v[3].x=0;quad.v[3].y=h;
	quad.v[3].tx=(float)(x)/cw;
	quad.v[3].ty=(float)(y+16)/ch;

	quad.blend=BLEND_DEFAULT;
	quad.tex=Game::getInstance()->mTileSet;

	for(int k=0;k<4;k++)
	{
		quad.v[k].z=0.5f;
	}

	quad.v[0].col = 
	quad.v[1].col = 
	quad.v[2].col = 
	quad.v[3].col = 0xffffffff;
}