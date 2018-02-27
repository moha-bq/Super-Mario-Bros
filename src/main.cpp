#include <hge.h>
#include <hgesprite.h>
#include <hgeparticle.h>
#include <hgegui.h>
#include <hgeguictrls.h>
#include <hgefont.h>
#include <stdio.h>
#include "Game.h"

int CALLBACK WinMain(
 HINSTANCE hInstance,
 HINSTANCE hPrevInstance,
 LPSTR lpCmdLine,
 int nCmdShow
)
{
	if(Game::getInstance()->init())
  {
		Game::getInstance()->hge->System_Start();
  }
  else
  {	
    MessageBoxA(NULL, Game::getInstance()->hge->System_GetErrorMessage(), "Error",
               MB_OK | MB_ICONERROR | MB_APPLMODAL);
  }

	Game::getInstance()->shutdown();
	Game::destroy();

  return 0;
}