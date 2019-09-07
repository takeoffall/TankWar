#include "GameRules.h"
#include "MapLayer.h"

//bool DynamicObject::isCollideObject()
//{
//	if (objectAhead && getSpeed() - m_frontObject->getSpeedForDirection(m_direction) > 0)//定义什么是碰撞
//	{
//		return true;
//		//碰撞之后怎么做
//		//afterCollideDoing();
//	}
//}

bool DynamicObject::isCollideObject()
{
	//1.MY_OBJECT ahead
	for (auto &i : map->cv)
	{
		if (getObjectInFront() == i->getName())
		{
			return true;
		}
	}
	//2.map block ahead
	if (mapBlockAhead())
	{
		return true;
	}
	return false;
}

bool DynamicObject::mapBlockAhead()
{
	auto rect = this->getBoundingBox();
	if (m_direction == MYDIRECTION::UP)
	{
		for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
		{
			if (map->isCollision(map->tileCoordForPosition(Point(i, rect.getMaxY() + 1))))
			{
				return true;
			}
		}	
	}

	else if (m_direction == MYDIRECTION::DOWN)
	{
		for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
		{
			if (map->isCollision(map->tileCoordForPosition(Point(i + 0.000003, rect.getMinY() - 1))))
			{
				return true;
			}
		}
	}

	else if (m_direction == MYDIRECTION::LEFT)
	{
		for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
		{
			if (map->isCollision(map->tileCoordForPosition(Point(rect.getMinX() - 1, i))))
			{
				return true;
			}
		}	
	}

	else if (m_direction == MYDIRECTION::RIGHT)
	{
		for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
		{
			if (map->isCollision(map->tileCoordForPosition(Point(rect.getMaxX() + 1, i))))
			{
				return true;
			}
		}
	}
	return false;
}

std::string DynamicObject::getObjectInFront()
{
	auto rect = this->getBoundingBox();

	if (m_direction == MYDIRECTION::UP)
	{
		for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
		{
			m_frontObject = getObjectForPosition(i, rect.getMaxY() + 1);
			if (m_frontObject != nullptr)
			{
				return m_frontObject->getName();
			}
		}
		return NONE_STRING;
	}

	else if (m_direction == MYDIRECTION::DOWN)
	{
		for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
		{
			m_frontObject = getObjectForPosition(i + 0.000003, rect.getMinY() - 1);
			if (m_frontObject != nullptr)
			{
				return m_frontObject->getName();
			}
		}
		return NONE_STRING;
	}

	else if (m_direction == MYDIRECTION::LEFT)
	{
		for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
		{
			m_frontObject = getObjectForPosition(rect.getMinX() - 1, i);
			if (m_frontObject != nullptr)
			{
				return m_frontObject->getName();
			}
		}
		return NONE_STRING;
	}

	else if (m_direction == MYDIRECTION::RIGHT)
	{
		for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
		{
			m_frontObject = getObjectForPosition(rect.getMaxX() + 1, i);
			if (m_frontObject != nullptr)
			{
				return m_frontObject->getName();
			}
		}
		return NONE_STRING;
	}
}

MY_OBJECT* DynamicObject::getObjectForPosition(float x, float y)
{
	map->cv.eraseObject(this);
	for (auto &i : map->cv)
	{
		auto rect = i->getBoundingBox();
		if (x >= rect.getMinX() && x <= rect.getMaxX() && y >= rect.getMinY() && y <= rect.getMaxY())
		{
			return i;
		}
	}
	return nullptr;
}

int MY_OBJECT::getSpeed()
{
	switch (m_direction)
	{
	case MYDIRECTION::UP:
		return m_moveUpSpeed;
		break;
	case MYDIRECTION::LEFT:
		return m_moveLeftSpeed;
		break;
	case MYDIRECTION::RIGHT:
		return m_moveRightSpeed;
		break;
	case MYDIRECTION::DOWN:
		return m_moveDownSpeed;
		break;
	default:
		log("direction type error: %d", m_direction);
		break;
	}
}

int MY_OBJECT::getSpeedForDirection(MYDIRECTION direction)
{
	switch (direction)
	{
	case MYDIRECTION::UP:
		return m_moveUpSpeed;
		break;
	case MYDIRECTION::LEFT:
		return m_moveLeftSpeed;
		break;
	case MYDIRECTION::RIGHT:
		return m_moveRightSpeed;
		break;
	case MYDIRECTION::DOWN:
		return m_moveDownSpeed;
		break;
	default:
		log("direction type error: %d", m_direction);
		break;
	}
}