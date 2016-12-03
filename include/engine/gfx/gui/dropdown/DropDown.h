#pragma once

#include "..\base\Component.h"
#include <vector>

class CDropDown : public Component
{
private:
	Sint16 m_fontSize;
	Uint16 m_selectedItem;
	Sint32 m_hoverItem;

	bool m_update;

	std::vector<std::string> m_itemList;
public:
	CDropDown(std::string p_compName, std::string p_title, Vector2<Sint32> p_pos, Vector2<Sint32> p_size, Sint16 p_fontSize, Sint8 p_colorTheme = 0);

	std::string getItem(Uint16 p_index);
	void addItem(std::string p_item);
	
	void setSelectedItem(Uint16 p_selectedItem);

	void input(Sint8& p_interactFlags, Sint8* p_keyStates, Sint8* p_mouseStates, Vector2<Sint32> p_mousePos);
	void update(GLfloat p_deltaUpdate);
	void render();

	Uint8 isUpdated();

	Uint16 getSelectedItem();
};
