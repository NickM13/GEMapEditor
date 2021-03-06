#include "engine\game\map\board\BoardMap.h"
#include "engine\gfx\font\Font.h"

#include <direct.h>
#include <fstream>

BoardMap::BoardMap()
{
	m_initialized = false;
}
BoardMap::BoardMap(Vector2<Uint16> p_boardSize)
{
	m_mapSize = p_boardSize;
	m_initialized = false;
	init();
	m_worldColors.push_back(Color()); // None
	m_worldColors.push_back(Color(255, 0, 0, 255)); // Solid
	m_worldColors.push_back(Color(255, 255, 255, 255)); // Slow
	m_worldColors.push_back(Color(255, 255, 0, 255)); // Trap
	m_worldColors.push_back(Color(0, 255, 0, 255)); // Switch
	m_worldColors.push_back(Color(0, 0, 255, 255)); // Solid Switch
	m_worldColors.push_back(Color(255, 0, 255, 255)); // Portal
	m_worldColors.push_back(Color(0, 255, 255, 255)); // Directional
}
BoardMap::~BoardMap()
{
	clear();
}

Uint16 BoardMap::addEntity(Entity p_entity)
{
	m_entities.push_back(p_entity);
	return Uint16(m_entities.size());
}
BoardMap::Entity& BoardMap::getEntity(Uint16 p_index)
{
	if(p_index < m_entities.size())
		return m_entities[p_index];
	return Entity();
}
Uint16 BoardMap::getEntitySize()
{
	return Uint16(m_entities.size());
}
void BoardMap::removeEntity(Uint16 p_index)
{
	m_entities.erase(m_entities.begin() + p_index);
}
void BoardMap::setEntity(Uint16 p_index, Vector2<Sint32> p_pos)
{
	if(m_editting && p_index != 0)
	{
		if(m_currentUndoEdit.m_entity.id != p_index)
			m_currentUndoEdit.m_entity = Edit::Entity(p_index, m_entities[p_index].m_pos);
		m_currentRedoEdit.m_entity = Edit::Entity(p_index, p_pos);
		m_entities[p_index].m_pos = p_pos;
	}
}

void BoardMap::undo()
{
	if(m_editting)
		stopEdit();
	if(m_cEdit >= 0)
	{
		Edit::Tile _tile;
		m_editting = true;
		for(Uint16 i = 0; i < m_undoEdits[m_cEdit].m_tile.size(); i++)
		{
			_tile = m_undoEdits[m_cEdit].m_tile[i];
			setTile(_tile.layer, _tile.x, _tile.y, _tile.id);
		}
		m_entities[m_undoEdits[m_cEdit].m_entity.id].m_pos = m_undoEdits[m_cEdit].m_entity.pos;
		m_cEdit--;
		m_editting = false;
	}
}
void BoardMap::redo()
{
	if(m_cEdit < Sint32(m_redoEdits.size()) - 1)
	{
		Edit::Tile _tile;
		m_editting = true;
		for(Uint16 i = 0; i < m_redoEdits[m_cEdit + 1].m_tile.size(); i++)
		{
			_tile = m_redoEdits[m_cEdit + 1].m_tile[i];
			setTile(_tile.layer, _tile.x, _tile.y, _tile.id);
		}
		m_entities[m_redoEdits[m_cEdit + 1].m_entity.id].m_pos = m_redoEdits[m_cEdit + 1].m_entity.pos;
		m_cEdit++;
		m_editting = false;
	}
}

void BoardMap::render(Vector2<GLfloat> p_camPos, GLfloat p_zoom)
{
	GLfloat _tileSize = m_tileSize + p_zoom;
	Map::render(p_camPos, p_zoom);
	if(m_layerVisible[2])
		for(Uint16 i = 1; i < m_entities.size(); i++)
			Font::getInstance().print(m_entities[i].m_name, Sint32(GLfloat(m_entities[i].m_pos.x) * _tileSize - p_camPos.x * _tileSize) + _tileSize / 2, Sint32(GLfloat(m_entities[i].m_pos.y) * _tileSize - p_camPos.y * _tileSize) - Font::getInstance().getHeight() / 2 - 1);
}

void BoardMap::renderEntity(Vector2<GLfloat> p_camPos, GLfloat p_tileSize)
{
	Vector2<Sint32> _tilesheetSize;
	Vector2<GLfloat> _texCorrection;
	glColor3f(1, 1, 1);
	for(Uint16 i = 1; i < m_entities.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_entities[i].m_entityTex.getId());
		_tilesheetSize = m_entities[i].m_entityTex.getSize();
		_texCorrection = Vector2<GLfloat>(0.5f, 0.5f) / _tilesheetSize;
		glPushMatrix();
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTranslatef(GLfloat(m_entities[i].m_pos.x + 1) - p_camPos.x, GLfloat(m_entities[i].m_pos.y + 1) - p_camPos.y, 0);
			glBegin(GL_QUADS);
			{
				if(_tilesheetSize.x == 0 || _tilesheetSize.y == 0)
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glVertex2f(0, 1);
					glVertex2f(1, 1);
					glVertex2f(1, 0);
					glVertex2f(0, 0);
				}
				else
				{
					glTexCoord2f(((1.f / (_tilesheetSize.x / m_tileSize)) * (m_entities[i].m_entityTexId % Sint32(ceil(GLfloat(_tilesheetSize.x) / m_tileSize)))) + _texCorrection.x, 1.f - ((1.f / (_tilesheetSize.y / m_tileSize)) * floor(m_entities[i].m_entityTexId / (GLfloat(_tilesheetSize.x) / m_tileSize)) / (GLfloat(_tilesheetSize.y) / m_tileSize) + (1.f / (_tilesheetSize.y / m_tileSize)) - _texCorrection.y));
					glVertex2f(0, 1);
					glTexCoord2f(((1.f / (_tilesheetSize.x / m_tileSize)) * (m_entities[i].m_entityTexId % Sint32(ceil(GLfloat(_tilesheetSize.x) / m_tileSize)))) + (1.f / (_tilesheetSize.x / m_tileSize)) - _texCorrection.x, 1.f - ((1.f / (_tilesheetSize.y / m_tileSize)) * floor(m_entities[i].m_entityTexId / (GLfloat(_tilesheetSize.x) / m_tileSize)) / (GLfloat(_tilesheetSize.y) / m_tileSize) + (1.f / (_tilesheetSize.y / m_tileSize)) - _texCorrection.y));
					glVertex2f(1, 1);
					glTexCoord2f(((1.f / (_tilesheetSize.x / m_tileSize)) * (m_entities[i].m_entityTexId % Sint32(ceil(GLfloat(_tilesheetSize.x) / m_tileSize)))) + (1.f / (_tilesheetSize.x / m_tileSize)) - _texCorrection.x, 1.f - ((1.f / (_tilesheetSize.y / m_tileSize)) * floor(m_entities[i].m_entityTexId / (GLfloat(_tilesheetSize.x) / m_tileSize)) / (GLfloat(_tilesheetSize.y) / m_tileSize) + _texCorrection.y));
					glVertex2f(1, 0);
					glTexCoord2f(((1.f / (_tilesheetSize.x / m_tileSize)) * (m_entities[i].m_entityTexId % Sint32(ceil(GLfloat(_tilesheetSize.x) / m_tileSize)))) + _texCorrection.x, 1.f - ((1.f / (_tilesheetSize.y / m_tileSize)) * floor(m_entities[i].m_entityTexId / (GLfloat(_tilesheetSize.x) / m_tileSize)) / (GLfloat(_tilesheetSize.y) / m_tileSize) + _texCorrection.y));
					glVertex2f(0, 0);
				}
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void BoardMap::save()
{
	save(m_mapName);
}
void BoardMap::save(std::string p_zoneName)
{
	std::cout << "Saving map map\\boards\\" << p_zoneName << "... ";

	m_mapName = p_zoneName;
	std::ofstream _file;
	_mkdir(std::string("map").c_str());
	_mkdir(std::string("map\\boards").c_str());
	_mkdir(std::string("map\\boards\\" + m_mapName).c_str());
	_mkdir(std::string("map\\boards\\" + m_mapName + "\\entities").c_str());

	// Save Info
	_file.open(std::string("map\\boards\\" + m_mapName + "\\Info.cfg"), std::ios::binary);
	{
		FileExt::writeShort(_file, m_mapSize.x);
		FileExt::writeShort(_file, m_mapSize.y);
	}
	_file.close();

	// Save Ground
	_file.open(std::string("map\\boards\\" + m_mapName + "\\Ground.tmf"));
	{
		Uint16 _tCount = 0;
		Uint16 _tile = m_tileData[0][0][0];
		for(Uint16 y = 0; y < m_mapSize.y; y++)
		{
			for(Uint16 x = 0; x < m_mapSize.x; x++)
			{
				if(_tile == m_tileData[0][x][y] && _tCount < 255)
					_tCount++;
				else
				{
					FileExt::writeShort(_file, _tCount);
					FileExt::writeShort(_file, _tile);

					_tCount = 1;
					_tile = m_tileData[0][x][y];
				}
			}
		}
		if(_tCount != 0)
		{
			FileExt::writeShort(_file, _tCount);
			FileExt::writeShort(_file, _tile);
		}
	}
	_file.close();

	// Save World
	_file.open(std::string("map\\boards\\" + m_mapName + "\\World.tmf"));
	{
		FileExt::writeShort(_file, Uint16(m_worldObjects.size()));
		std::vector<Uint8> _objData;
		for(Uint16 i = 0; i < Uint16(m_worldObjects.size()); i++)
		{
			_objData.push_back(Uint8(min(m_worldObjects[i].m_name.length(), 128)));
			for(Uint16 j = 0; j < Uint16(min(m_worldObjects[i].m_name.length(), 128)); j++)
				_objData.push_back(m_worldObjects[i].m_name[j]);
			_objData.push_back(Uint8((m_worldObjects[i].m_interactionType & 0xFF00) >> 8));
			_objData.push_back(Uint8((m_worldObjects[i].m_interactionType & 0xFF)));
			_objData.push_back(Uint8((m_worldObjects[i].m_tileTex & 0xFF00) >> 8));
			_objData.push_back(Uint8((m_worldObjects[i].m_tileTex & 0xFF)));
			switch(m_worldObjects[i].m_interactionType)
			{
			case 0: // NONE

				break;
			case 1: // SOLID

				break;
			case 2: // SWITCH
				_objData.push_back(Uint8((m_worldObjects[i].m_frequency & 0xFF00) >> 8));
				_objData.push_back(Uint8((m_worldObjects[i].m_frequency & 0xFF)));
				break;
			case 3: // SOLID SWITCH
				_objData.push_back(Uint8((m_worldObjects[i].m_frequency & 0xFF00) >> 8));
				_objData.push_back(Uint8((m_worldObjects[i].m_frequency & 0xFF)));
				break;
			case 4: // PORTAL
				_objData.push_back(Uint8(min(m_worldObjects[i].m_portalDest.length(), 128)));
				for(Uint16 j = 0; j < Uint16(min(m_worldObjects[i].m_portalDest.length(), 128)); j++)
					_objData.push_back(m_worldObjects[i].m_portalDest[j]);
				_objData.push_back(Uint8((m_worldObjects[i].m_destX & 0xFF00) >> 8));
				_objData.push_back(Uint8((m_worldObjects[i].m_destX & 0xFF)));
				_objData.push_back(Uint8((m_worldObjects[i].m_destY & 0xFF00) >> 8));
				_objData.push_back(Uint8((m_worldObjects[i].m_destY & 0xFF)));
				break;
			default:

				break;
			}

			FileExt::writeShort(_file, Uint16(_objData.size()));
			for(Uint16 j = 0; j < Uint16(_objData.size()); j++)
			{
				FileExt::writeChar(_file, _objData[j]);
			}
			_objData.clear();
		}


		Uint16 _tCount = 0;
		Uint16 _tile = m_tileData[1][0][0];
		for(Uint16 y = 0; y < m_mapSize.y; y++)
		{
			for(Uint16 x = 0; x < m_mapSize.x; x++)
			{
				if(_tile == m_tileData[1][x][y] && _tCount < 255)
					_tCount++;
				else
				{
					FileExt::writeShort(_file, _tCount);
					FileExt::writeShort(_file, _tile);

					_tCount = 1;
					_tile = m_tileData[1][x][y];
				}
			}
		}
		if(_tCount != 0)
		{
			FileExt::writeShort(_file, _tCount);
			FileExt::writeShort(_file, _tile);
		}
	}
	_file.close();

	// Save Sky
	_file.open(std::string("map\\boards\\" + m_mapName + "\\Sky.tmf"));
	{
		Uint16 _tCount = 0;
		Uint16 _tile = m_tileData[2][0][0];
		for(Uint16 y = 0; y < m_mapSize.y; y++)
		{
			for(Uint16 x = 0; x < m_mapSize.x; x++)
			{
				if(_tile == m_tileData[2][x][y] && _tCount < 255)
					_tCount++;
				else
				{
					FileExt::writeShort(_file, _tCount);
					FileExt::writeShort(_file, _tile);

					_tCount = 1;
					_tile = m_tileData[2][x][y];
				}
			}
		}
		if(_tCount != 0)
		{
			FileExt::writeShort(_file, _tCount);
			FileExt::writeShort(_file, _tile);
		}
	}
	_file.close();

	// Save Entity Name and Position
	_file.open(std::string("map\\boards\\" + m_mapName + "\\Entity.tmf"));
	{
		FileExt::writeShort(_file, Uint16(m_entities.size()));

		Entity _en;
		for(Uint16 i = 0; i < m_entities.size(); i++)
		{
			_en = m_entities[i];
			FileExt::writeShort(_file, Uint16(_en.m_name.length()));
			for(Uint16 j = 0; j < _en.m_name.length(); j++)
				FileExt::writeChar(_file, _en.m_name[j]);
			FileExt::writeShort(_file, _en.m_pos.x);
			FileExt::writeShort(_file, _en.m_pos.y);
			_mkdir(std::string("map\\boards\\" + m_mapName + "\\entities\\" + m_entities[i].m_name).c_str());
		}
	}
	_file.close();

	// Save Entity Info
	for(Uint16 i = 0; i < Uint16(m_entities.size()); i++)
	{
		_file.open(std::string("map\\boards\\" + m_mapName + "\\entities\\" + m_entities[i].m_name + "\\Info.cfg"));
		{
			FileExt::writeChar(_file, m_entities[i].m_entityType);
			FileExt::writeShort(_file, Uint16(m_entities[i].m_entityTex.getName().length()));
			for(Uint16 j = 0; j < m_entities[i].m_entityTex.getName().length(); j++)
				FileExt::writeChar(_file, m_entities[i].m_entityTex.getName()[j]);
			FileExt::writeShort(_file, Uint16(m_entities[i].m_entityTexId));
		}
		_file.close();
	}

	// Save Entity Interact Scripts
	for(Uint16 i = 0; i < Uint16(m_entities.size()); i++)
	{
		_file.open(std::string("map\\boards\\" + m_mapName + "\\entities\\" + m_entities[i].m_name + "\\Interact"));
		{
			for(Uint16 j = 0; j < m_entities[i].m_interact.length(); j++)
				FileExt::writeChar(_file, m_entities[i].m_interact[j]);
		}
		_file.close();
	}

	// Save Entity Idle Script
	for(Uint16 i = 0; i < Uint16(m_entities.size()); i++)
	{
		_file.open(std::string("map\\boards\\" + m_mapName + "\\entities\\" + m_entities[i].m_name + "\\Idle"));
		{
			for(Uint16 j = 0; j < m_entities[i].m_idle.length(); j++)
				FileExt::writeChar(_file, m_entities[i].m_idle[j]);
		}
		_file.close();
	}

	std::cout << "Complete." << std::endl;
}

bool BoardMap::load()
{
	return load(m_mapName);
}

bool BoardMap::load(std::string p_zoneName)
{
	std::cout << "Loading map map\\boards\\" << p_zoneName << "... ";

	std::ifstream _file;
	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Info.cfg").c_str(), std::ios::binary);
	{
		if(!_file.good())
		{
			std::cout << "No Info.cfg file found." << std::endl;
			return false;
		}
	}
	_file.close();
	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Ground.tmf").c_str(), std::ios::binary);
	{
		if(!_file.good())
		{
			std::cout << "No Ground.tmf file found." << std::endl;
			return false;
		}
	}
	_file.close();
	_file.open(std::string("map\\boards\\" + p_zoneName + "\\World.tmf").c_str(), std::ios::binary);
	{
		if(!_file.good())
		{
			std::cout << "No World.tmf file found." << std::endl;
			return false;
		}
	}
	_file.close();
	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Entity.tmf").c_str(), std::ios::binary);
	{
		if(!_file.good())
		{
			std::cout << "No Entity.tmf file found." << std::endl;
			return false;
		}
	}
	_file.close();
	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Sky.tmf").c_str(), std::ios::binary);
	{
		if(!_file.good())
		{
			std::cout << "No Sky.tmf file found." << std::endl;
			return false;
		}
	}
	_file.close();



	clear();

	m_mapName = p_zoneName;

	Uint32 _length, _index;
	char* _data;
	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Info.cfg").c_str(), std::ios::binary);
	{
		_index = 0;

		_file.seekg(0, _file.end);
		_length = Uint32(_file.tellg());
		_file.seekg(0, _file.beg);
		_data = new char[_length];
		_file.read(_data, _length);

		m_mapSize.x = FileExt::readShort(_data, _index);
		m_mapSize.y = FileExt::readShort(_data, _index);

		init();
		delete[] _data;
	}
	_file.close();

	Uint16 _tileCount;
	Uint16 _tileId, _mapIndex;

	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Ground.tmf").c_str(), std::ios::binary);
	{
		_mapIndex = 0;
		_index = 0;

		_file.seekg(0, _file.end);
		_length = Uint32(_file.tellg());
		_file.seekg(0, _file.beg);
		_data = new char[_length];
		_file.read(_data, _length);

		while(_mapIndex < m_mapSize.x * m_mapSize.y && _index < _length)
		{
			_tileCount = FileExt::readShort(_data, _index);
			_tileId = FileExt::readShort(_data, _index);

			for(Uint16 i = 0; i < _tileCount; i++)
			{
				m_tileData[0][_mapIndex % m_mapSize.x][int(floor(GLfloat(_mapIndex) / m_mapSize.x))] = _tileId;
				_mapIndex++;
			}
		}
		delete[] _data;
	}
	_file.close();

	_file.open(std::string("map\\boards\\" + p_zoneName + "\\World.tmf").c_str(), std::ios::binary);
	{
		_mapIndex = 0;
		_index = 0;

		_file.seekg(0, _file.end);
		_length = Uint32(_file.tellg());
		_file.seekg(0, _file.beg);
		_data = new char[_length];
		_file.read(_data, _length);


		Uint16 _objCount = FileExt::readShort(_data, _index);
		Uint16 _objNameLen = 0;
		std::string _objName;
		Uint16 _interactionType, _tileTex;

		m_worldObjects.clear();

		for(Uint16 i = 0; i < _objCount; i++)
		{
			FileExt::readShort(_data, _index); // Size of object chunk, if you want to skip this stuff
			_objName = "";
			_objNameLen = FileExt::readChar(_data, _index);
			for(Uint16 j = 0; j < Uint16(_objNameLen); j++)
				_objName = _objName + char(FileExt::readChar(_data, _index));
			_interactionType = FileExt::readShort(_data, _index);
			_tileTex = FileExt::readShort(_data, _index);
			m_worldObjects.push_back(WorldObject(_objName, _interactionType, _tileTex));
			switch(m_worldObjects[i].m_interactionType)
			{
			case 0: // NONE

				break;
			case 1: // SOLID

				break;
			case 2: // SWITCH
				m_worldObjects[i].m_frequency = FileExt::readShort(_data, _index);
				break;
			case 3: // SOLID SWITCH
				m_worldObjects[i].m_frequency = FileExt::readShort(_data, _index);
				break;
			case 4: // PORTAL
				_objName = "";
				_objNameLen = FileExt::readChar(_data, _index);
				for(Uint16 j = 0; j < Uint16(_objNameLen); j++)
					_objName = _objName + char(FileExt::readChar(_data, _index));
				m_worldObjects[i].m_portalDest = _objName;
				m_worldObjects[i].m_destX = FileExt::readShort(_data, _index);
				m_worldObjects[i].m_destY = FileExt::readShort(_data, _index);
				break;
			}
		}


		while(_mapIndex < m_mapSize.x * m_mapSize.y && _index < _length)
		{
			_tileCount = FileExt::readShort(_data, _index);
			_tileId = FileExt::readShort(_data, _index);

			for(Uint16 i = 0; i < _tileCount; i++)
			{
				m_tileData[1][_mapIndex % m_mapSize.x][int(floor(GLfloat(_mapIndex) / m_mapSize.x))] = _tileId;
				_mapIndex++;
			}
		}
		delete[] _data;
	}
	_file.close();

	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Sky.tmf").c_str(), std::ios::binary);
	{
		_mapIndex = 0;
		_index = 0;

		_file.seekg(0, _file.end);
		_length = Uint32(_file.tellg());
		_file.seekg(0, _file.beg);
		_data = new char[_length];
		_file.read(_data, _length);

		while(_mapIndex < m_mapSize.x * m_mapSize.y && _index < _length)
		{
			_tileCount = FileExt::readShort(_data, _index);
			_tileId = FileExt::readShort(_data, _index);

			for(Uint16 i = 0; i < _tileCount; i++)
			{
				m_tileData[2][_mapIndex % m_mapSize.x][int(floor(GLfloat(_mapIndex) / m_mapSize.x))] = _tileId;
				_mapIndex++;
			}
		}
		delete[] _data;
	}
	_file.close();

	_file.open(std::string("map\\boards\\" + p_zoneName + "\\Entity.tmf").c_str(), std::ios::binary);
	{
		_mapIndex = 0;
		_index = 0;

		_file.seekg(0, _file.end);
		_length = Uint32(_file.tellg());
		_file.seekg(0, _file.beg);
		_data = new char[_length];
		_file.read(_data, _length);


		Uint16 _objCount = FileExt::readShort(_data, _index);
		Uint16 _objNameLen = 0;
		std::string _objName;
		Uint16 _texNameLen = 0;
		std::string _texName;

		m_entities.clear();

		for(Uint16 i = 0; i < _objCount; i++)
		{
			_objName = "";
			_objNameLen = FileExt::readShort(_data, _index);
			for(Uint16 j = 0; j < Uint16(_objNameLen); j++)
				_objName = _objName + char(FileExt::readChar(_data, _index));
			m_entities.push_back(Entity(_objName));
			m_entities[i].m_pos.x = FileExt::readShort(_data, _index);
			m_entities[i].m_pos.y = FileExt::readShort(_data, _index);
		}
		delete[] _data;
	}
	_file.close();

	Uint16 _texNameLen = 0;
	std::string _texName = "";
	for(Uint16 i = 0; i < m_entities.size(); i++)
	{
		_file.open(std::string("map\\boards\\" + p_zoneName + "\\entities\\" + m_entities[i].m_name + "\\Info.cfg").c_str(), std::ios::binary);
		{
			_index = 0;
			_file.seekg(0, _file.end);
			_length = Uint32(_file.tellg());
			_file.seekg(0, _file.beg);
			_data = new char[_length];
			_file.read(_data, _length);
			m_entities[i].m_entityType = FileExt::readChar(_data, _index);
			_texNameLen = FileExt::readShort(_data, _index);
			_texName = "";
			for(Uint16 i = 0; i < _texNameLen; i++)
				_texName += FileExt::readChar(_data, _index);
			m_entities[i].m_entityTex = LTexture::getInstance().getImage(_texName);
			m_entities[i].m_entityTex.setName(_texName);
			m_entities[i].m_entityTexId = FileExt::readShort(_data, _index);
			delete[] _data;
		}
		_file.close();
	}

	for(Uint16 i = 1; i < m_entities.size(); i++)
	{
		_file.open(std::string("map\\boards\\" + p_zoneName + "\\entities\\" + m_entities[i].m_name + "\\Interact").c_str(), std::ios::binary);
		{
			_index = 0;
			_file.seekg(0, _file.end);
			_length = Uint32(_file.tellg());
			_file.seekg(0, _file.beg);
			_data = new char[_length];
			_file.read(_data, _length);
			for(Uint16 j = 0; j < _length; j++)
				m_entities[i].m_interact += FileExt::readChar(_data, _index);
			delete[] _data;
		}
		_file.close();
	}

	for(Uint16 i = 1; i < m_entities.size(); i++)
	{
		_file.open(std::string("map\\boards\\" + p_zoneName + "\\entities\\" + m_entities[i].m_name + "\\Idle").c_str(), std::ios::binary);
		{
			_index = 0;
			_file.seekg(0, _file.end);
			_length = Uint32(_file.tellg());
			_file.seekg(0, _file.beg);
			_data = new char[_length];
			_file.read(_data, _length);
			for(Uint16 j = 0; j < _length; j++)
				m_entities[i].m_idle += FileExt::readChar(_data, _index);
			delete[] _data;
		}
		_file.close();
	}

	std::cout << "Complete." << std::endl;

	return true;
}
