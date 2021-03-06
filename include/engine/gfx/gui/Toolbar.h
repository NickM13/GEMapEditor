#pragma once

#include "engine\utils\Utilities.h"
#include "_Component.h"
#include "..\font\Font.h"
#include "Panel.h"

class CToolbar : public Component
{
private:
	struct SubList
	{
		std::string m_name;
		bool m_subList;
		std::vector<SubList> m_buttons;
		Uint16 m_width;
		function m_func = 0;
		SubList() {};
		SubList(std::string p_name, function p_func = 0)
		{
			m_name = p_name;
			m_subList = false;
			m_width = 0;
			m_func = p_func;
		}

		void setFunction(function p_func)
		{
			m_func = p_func;
		}
		
		function getFunction()
		{
			return m_func;
		}

		void callFunction()
		{
			m_func();
		}

		SubList* find(std::string p_name)
		{
			if(m_subList)
				for(Uint16 i = 0; i < m_buttons.size(); i++)
					if(m_buttons[i].m_name == p_name)
						return &m_buttons[i];

			return 0;
		}

		void addButton(std::string p_name, function p_func)
		{
			m_buttons.push_back(SubList(p_name, p_func));
			m_subList = true;
			if(p_name.length() > m_width)
				m_width = Uint16(p_name.length());
		}
	};
	Panel* m_panelMain, *m_panelSub;
	SubList m_buttonsMain;

	std::string m_currDir;
	std::string m_selected; // Directory to button being hovered over

	GLfloat m_slideCounter;
public:
	CToolbar(std::string p_compName, Vector2<Sint32> p_pos, Vector2<Sint32> p_size, Sint8 p_colorTheme);

	//Directory splits with /'s or \\'s 
	void addButton(std::string p_dir, std::string p_buttonName, function p_func = 0);

	void input(Sint8& p_interactFlags, Sint8* p_keyStates, Sint8* p_mouseStates, Vector2<Sint32> p_mousePos);
	void update(GLfloat p_deltaUpdate);
	void render();
};
