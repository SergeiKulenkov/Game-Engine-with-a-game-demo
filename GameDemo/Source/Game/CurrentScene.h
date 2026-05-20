#pragma once
#include <Scene/Scene.h>

class Game;

////////////////////

class CurrentScene : public Scene
{
public:
	CurrentScene() {}
	virtual ~CurrentScene() {}

private:
	friend class Game;
};