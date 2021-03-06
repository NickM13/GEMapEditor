#pragma once

#include "engine\utils\Globals.h"
#include "_Component.h"
#include "..\font\Font.h"
#include <iostream>

struct NumberField : public Component
{
public:
	NumberField(std::string p_compName, std::string p_title, Vector2<Sint32> p_pos, Vector2<Sint32> p_size, Vector2<Sint32> p_numBounds, Sint32 p_numValue, Sint8 p_colorTheme = 1);

	void input(Sint8& p_interactFlags, Sint8* p_keyStates, Sint8* p_mouseStates, Vector2<Sint32> p_mousePos);
	void update(GLfloat p_deltaUpdate);
	void render();
private:
	Sint32 m_minNum, m_maxNum;
};
