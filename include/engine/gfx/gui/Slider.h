#pragma once

#include "engine\utils\Utilities.h"
#include "_Component.h"
#include "..\font\Font.h"

struct CSlider : public Component
{
private:
	Sint32 m_length;
	Sint32 m_maxValue;
	Sint32 m_slideValue;
	Sint32 m_height = 16;

	bool m_held = false;
	Vector2<Sint32> m_holdPos;
public:
	CSlider(std::string p_compName, std::string p_title, Vector2<Sint32> p_pos, Sint32 p_length, Sint32 p_maxValue, Sint32 p_numValue, Sint8 p_colorTheme = 1);

	void setMaxValue(Sint16 p_value);
	void setValue(Sint32 p_value);
	void addValue(Sint16 p_value);

	void input(Sint8& p_interactFlags, Sint8* p_keyStates, Sint8* p_mouseStates, Vector2<Sint32> p_mousePos);
	void update(Vector2<Sint32> p_pos);
	void render();

	Sint32 getMaxValue();
	Sint32 getLength();
};
