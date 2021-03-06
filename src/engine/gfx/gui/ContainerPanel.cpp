#include "engine\gfx\gui\ContainerPanel.h"

ContainerPanel::ContainerPanel(std::string p_compName, std::string p_title, Vector2<Sint32> p_pos, Vector2<Sint32> p_size, Sint8 p_colorTheme, bool p_visible, Sint32 p_texture, Sint8 p_textureStyle)
{
	m_compName = p_compName;
	m_title = p_title;
	m_pos = p_pos;
	m_size = p_size;
	m_colorTheme = m_colorThemes[p_colorTheme];
	m_texture = p_texture;
	m_textureStyle = p_textureStyle;
	m_visible = p_visible;

	m_borderThickness = 1;

	m_scroll = {0, 0};
	m_minScroll = m_maxScroll = {0, 0};

	m_draggable = false; //currently disabled
	m_scrollX = m_scrollY = false;
}

Component* ContainerPanel::addComponent(Component* p_component, Sint8 p_alignment)
{
	Component* _comp = Container::addComponent(p_component, p_alignment);

	m_minScroll.x = min(m_minScroll.x, m_contentArea.x);
	m_minScroll.y = min(m_minScroll.y, m_contentArea.y - (m_title != "" ? 20 : 0));

	m_maxScroll.x = max(m_maxScroll.x, m_contentArea.z - m_size.x);
	m_maxScroll.y = max(m_maxScroll.y, m_contentArea.w - m_size.y + 4);
	if(m_maxScroll.y > 10000)
		std::cout << "Something went wrong! Tell Nick: " << m_maxScroll.y << ", " << m_contentArea.w << std::endl;

	m_scroll = m_minScroll;

	if((m_minScroll.y != m_maxScroll.y) != m_scrollY)
		setScrollY(!m_scrollY);

	return _comp;
}

void ContainerPanel::calcSize(Vector2<Sint32> p_offset)
{
	if(p_offset.getLength() == 0)
		m_minScroll = m_maxScroll = Vector2<Sint32>();
	m_contentArea = Vector4<Sint32>();
	for (Uint16 i = 0; i < m_componentList.size(); i++)
	{
		Comp _comp = m_componentList[i];
		Component* _component = m_componentList[i].m_component;
		switch (_comp.m_alignment)
		{
		case PANEL_ALIGN_CENTER:
			_component->setPosition(_component->getPosition() + Vector2<Sint32>(p_offset.x / 2, 0));
			break;
		case PANEL_ALIGN_RIGHT:
			_component->setPosition(_component->getPosition() + Vector2<Sint32>(p_offset.x, 0));
			break;
		case PANEL_ALIGN_TOP:
			_component->setPosition(_component->getPosition() + Vector2<Sint32>(p_offset.x / 2, 0));
			break;
		case PANEL_ALIGN_BOTTOM:
			_component->setPosition(_component->getPosition() + Vector2<Sint32>(p_offset.x / 2, 0));
			break;
		case PANEL_ALIGN_TOP_RIGHT:
			_component->setPosition(_component->getPosition() + Vector2<Sint32>(p_offset.x, 0));
			break;
		case PANEL_ALIGN_BOTTOM_RIGHT:
			_component->setPosition(_component->getPosition() + Vector2<Sint32>(p_offset.x, 0));
			break;
		default:
			break;
		}
		if(_component->isVisible())
			m_contentArea = Vector4<Sint32>(min(_component->getRealPosition().x, m_contentArea.x), min(_component->getRealPosition().y, m_contentArea.y), max(_component->getRealPosition().x + _component->getRealSize().x, m_contentArea.z), max(_component->getRealPosition().y + _component->getRealSize().y + 8, m_contentArea.w));
	}

	m_minScroll.x = min(m_minScroll.x, m_contentArea.x);
	m_minScroll.y = min(m_minScroll.y, m_contentArea.y - (m_title != "" ? 20 : 0));

	m_maxScroll.x = max(m_maxScroll.x, m_contentArea.z - m_size.x);
	m_maxScroll.y = max(m_maxScroll.y, m_contentArea.w - m_size.y + 4);

	if((m_minScroll.y != m_maxScroll.y) != m_scrollY)
		setScrollY(!m_scrollY);
}
//TODO: Set whether scroll bar across bottom exists
void ContainerPanel::setScrollX(bool p_state)
{

}
//Set whether scroll bar across right exists
void ContainerPanel::setScrollY(bool p_state)
{
	if(p_state != m_scrollY)
	{
		m_scrollY = p_state;
		if(p_state)
			m_size.x -= 10;
		else
			m_size.x += 10;
		calcSize({p_state ? -10 : 10, 0});
	}
}

void ContainerPanel::input(Sint8& p_interactFlags, Sint8* p_keyStates, Sint8* p_mouseStates, Vector2<Sint32> p_mousePos)
{
	if(p_mousePos.x - m_pos.x >= 0 && p_mousePos.x - m_pos.x <= m_size.x + (m_scrollY ? 10 : 0) && 
		p_mousePos.y - m_pos.y >= 0 && p_mousePos.y - m_pos.y <= m_size.y)
		Container::input(p_interactFlags, p_keyStates, p_mouseStates, p_mousePos + m_scroll);
	else
	{
		bool _1 = (p_interactFlags & 1) == 0, _4 = (p_interactFlags & 4) == 0;
		if(_1)
			p_interactFlags += 1;
		if(_4)
			p_interactFlags += 4;
		Container::input(p_interactFlags, p_keyStates, p_mouseStates, p_mousePos + m_scroll);
		if(_1)
			p_interactFlags -= 1;
		if(_4)
			p_interactFlags -= 4;
	}

	m_moveToFront = (m_lHeld != 0);

	// Scroll window
	if((p_interactFlags & 4) == 0 &&
		p_mousePos.x - m_pos.x >= 0 && p_mousePos.x - m_pos.x <= m_size.x + 10 &&
		p_mousePos.y - m_pos.y >= 0 && p_mousePos.y - m_pos.y <= m_size.y)
	{
		m_scroll.y -= Globals::getInstance().m_mouseScroll * 8;
		p_interactFlags += 4;
	}
	if(m_rHeld != 0)
	{
		if(m_minScroll.x != 0 || m_maxScroll.x != 0)
			m_scroll.x += p_mousePos.x - m_mousePos.x;
		if(m_minScroll.y != 0 || m_maxScroll.y != 0)
			m_scroll.y += p_mousePos.y - m_mousePos.y;
	}

	if(m_scroll.x < m_minScroll.x)
		m_scroll.x = m_minScroll.x;
	if(m_scroll.y < m_minScroll.y)
		m_scroll.y = m_minScroll.y;

	if(m_scroll.x > m_maxScroll.x)
		m_scroll.x = m_maxScroll.x;
	if(m_scroll.y > m_maxScroll.y)
		m_scroll.y = m_maxScroll.y;

	if((p_interactFlags & 1) == 0 &&
		p_mousePos.x - m_pos.x >= 0 && p_mousePos.x - m_pos.x <= m_size.x &&
		p_mousePos.y - m_pos.y >= 0 && p_mousePos.y - m_pos.y <= m_size.y)
		p_interactFlags += 1;

	// Drag window -- disabled
	if(((p_interactFlags & 1) == 0 || m_lHeld) && m_draggable)
	{
		if(m_lHeld != 0)
			m_pos = m_pos + (p_mousePos - m_mousePos);
		if(p_mousePos.x - m_pos.x >= 0 && p_mousePos.x - m_pos.x <= m_size.x && 
			p_mousePos.y - m_pos.y >= 0 && p_mousePos.y - m_pos.y <= m_size.y)
		{
			if(p_mouseStates[0] == 1)
				m_lHeld = 2;
			else if(p_mouseStates[0] == 0 || p_mouseStates[0] == 3)
			{
				m_lHeld = 0;
				if(m_pos.x < 0)
					m_pos.x = 0;
				else if(m_pos.x > Sint32(Globals::getInstance().m_screenSize.x - m_size.x))
					m_pos.x = Sint32(Globals::getInstance().m_screenSize.x - m_size.x);
				if(m_pos.y > 0)
					m_pos.y = 0;
				else if(m_pos.y < -Sint32(Globals::getInstance().m_screenSize.y - m_size.y) + 24)
					m_pos.y = -Sint32(Globals::getInstance().m_screenSize.y - m_size.y) + 24;
			}
		}
		else
		{
			if(p_mouseStates[0] == 0 || p_mouseStates[0] == 3)
			{
				m_lHeld = 0;
				if(m_pos.x < 0)
					m_pos.x = 0;
				else if(m_pos.x > Sint32(Globals::getInstance().m_screenSize.x - m_size.x))
					m_pos.x = Sint32(Globals::getInstance().m_screenSize.x - m_size.x);
				if(m_pos.y > 0)
					m_pos.y = 0;
				else if(m_pos.y < -Sint32(Globals::getInstance().m_screenSize.x - m_size.x) + 24)
					m_pos.y = -Sint32(Globals::getInstance().m_screenSize.x - m_size.x) + 24;
			}
		}
	}
	if(m_scrollY && 
		p_mousePos.x - m_pos.x >= m_size.x && p_mousePos.x - m_pos.x <= m_size.x + 10 && 
		p_mousePos.y - m_pos.y >= 0 && p_mousePos.y - m_pos.y <= m_size.y)
	{
		if(m_scrollY)
		{
			if(p_mouseStates[0] == 1)
				m_rHeld = 2;
			else if(p_mouseStates[0] == 0 || p_mouseStates[0] == 3)
				m_rHeld = 0;
		}
		if((p_interactFlags & 1) == 0)
			p_interactFlags += 1;
	}
	else
	{
		if(p_mouseStates[0] == 0 || p_mouseStates[0] == 3)
			m_rHeld = 0;
		else
			if(m_rHeld && (p_interactFlags & 1) == 0)
				p_interactFlags += 1;
	}
	m_mousePos = p_mousePos;
}
void ContainerPanel::update(GLfloat p_updateTime)
{
	Container::update(p_updateTime);
}
void ContainerPanel::render()
{
	if(m_visible)
	{
		if(m_texture != 0)
			Component::renderFill();
		else
			Component::render();
		if(m_title != "")
		{
			m_colorTheme.m_back.useColor();
			glBegin(GL_LINES);
			{
				glVertex2f(GLfloat(m_pos.x), GLfloat(m_pos.y + 24));
				glVertex2f(GLfloat(m_pos.x + m_size.x), GLfloat(m_pos.y + 24));
			}
			glEnd();

			m_colorTheme.m_text.useColor();
			Font::getInstance().setAlignment(ALIGN_CENTER);
			Font::getInstance().print(m_title, m_pos.x + m_size.x / 2, m_pos.y + 4);
		}
		MScissor::getInstance().push(Rect(GLfloat(m_pos.x + 1), GLfloat(m_pos.y + (m_title != "" ? 20 : 0)), GLfloat(m_size.x - 2), GLfloat(m_size.y - 1 - (m_title != "" ? 20 : 0))));
		glPushMatrix();
		{
			glTranslatef(-GLfloat(m_scroll.x), -GLfloat(m_scroll.y), 0);
			Container::render();
		}
		glPopMatrix();
		MScissor::getInstance().pop();
		glPushMatrix();
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			Vector2<Sint32> _scrollDist = m_maxScroll - m_minScroll;
			glTranslatef(GLfloat(m_pos.x), GLfloat(m_pos.y), 0);
			if(m_scrollX)
			{
				m_colorTheme.m_back.useColor();
				glBegin(GL_QUADS);
				{
					glVertex2f(0, GLfloat(m_size.y));
					glVertex2f(0, GLfloat(m_size.y + 10));
					glVertex2f(GLfloat(m_size.x), GLfloat(m_size.y + 10));
					glVertex2f(GLfloat(m_size.x), GLfloat(m_size.y));
				}
				glEnd();
				m_colorTheme.m_fore.useColor();
			}
			if(m_scrollY)
			{
				m_colorTheme.m_back.useColor();
				glBegin(GL_QUADS);
				{
					glVertex2f(GLfloat(m_size.x), 0);
					glVertex2f(GLfloat(m_size.x + 10), 0);
					glVertex2f(GLfloat(m_size.x + 10), GLfloat(m_size.y));
					glVertex2f(GLfloat(m_size.x), GLfloat(m_size.y));
				}
				glEnd();
				m_colorTheme.m_fore.useColor();
				glBegin(GL_QUADS);
				{
					//((m_size.y / GLfloat(_scrollDist.y + m_size.y)) * m_size.y) - 1
					glVertex2f(GLfloat(m_size.x + 1), (GLfloat(m_scroll.y - m_minScroll.y) / _scrollDist.y) * (m_size.y * _scrollDist.y) / (m_size.y + _scrollDist.y) + 1);
					glVertex2f(GLfloat(m_size.x + 9), (GLfloat(m_scroll.y - m_minScroll.y) / _scrollDist.y) * (m_size.y * _scrollDist.y) / (m_size.y + _scrollDist.y) + 1);
					glVertex2f(GLfloat(m_size.x + 9), (GLfloat(m_scroll.y - m_minScroll.y) / _scrollDist.y) * (m_size.y * _scrollDist.y) / (m_size.y + _scrollDist.y) + ((m_size.y * m_size.y) / (m_size.y + _scrollDist.y)) - 1);
					glVertex2f(GLfloat(m_size.x + 1), (GLfloat(m_scroll.y - m_minScroll.y) / _scrollDist.y) * (m_size.y * _scrollDist.y) / (m_size.y + _scrollDist.y) + ((m_size.y * m_size.y) / (m_size.y + _scrollDist.y)) - 1);
				}
				glEnd();
			}
		}
		glPopMatrix();
	}
}