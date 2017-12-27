#include "stdafx.h"
#include "BaseGame.h"




namespace xr {	
	BaseGame::BaseGame() :
		clearColor(0, 0, 0, 1)
	{
	}

	Window & BaseGame::getWindow()
	{
		return *this->window;
	}

	void BaseGame::setClearColor(float r, float g, float b, float a)
	{
		this->clearColor = { r, g, b, a };
	}
}

