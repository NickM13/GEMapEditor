#pragma once

#include "engine\utils\Globals.h"
#include "engine\utils\OpenGL.h"
#include "engine\utils\Singleton.h"
#include "engine\utils\variable\datatype\Rectangle.h"
#include "engine\utils\variable\manager\TextureManager.h"

#include "engine\gfx\LTexture.h"

#include "editor\board\BoardEditor.h"
#include "editor\zone\ZoneEditor.h"

#include "Items.h"
#include "Cards.h"

#include <vector>

class Game : public Singleton<Game>
{
public:
	enum EditorState
	{
		ZONE = 0,
		BOARD = 1
	} m_editorState;

	BoardEditor* m_boardEditor;
	ZoneEditor* m_zoneEditor;

	~Game() {Font::getInstance().clean();};
	bool init();
	void resize();

	void setEditorState(EditorState p_state);
	void setFadeMessage(std::string p_msg, GLfloat p_time);

	void input();
	void update();
	void render();

	void close();
private:
	struct Stamp
	{
		Vector2<Sint32> m_size;
		Uint16 **m_ground, **m_world, **m_entity, **m_sky;

		Stamp(Vector2<Sint32> p_size = {}, Uint16** p_ground = 0, Uint16** p_world = 0, Uint16** p_entity = 0, Uint16** p_sky = 0) :
			m_size(p_size),
			m_ground(p_ground),
			m_world(p_world),
			m_entity(p_entity),
			m_sky(p_sky) {}
	};

	GLfloat m_lastUpdate, m_deltaUpdate;

protected:
	GLfloat m_fadeTimer;
	std::string m_fadeMsg;
};
