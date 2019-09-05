#ifndef _MAPLAYER_H_
#define _MAPLAYER_H_

#include "cocos2d.h"
USING_NS_CC;
#include "Tank.h"
class Props;

class MapLayer : public TMXTiledMap
{
public:
	MapLayer() {}
	MapLayer(const std::string& tmxFile);
	static MapLayer* create(const std::string& tmxFile);
	virtual bool init(const std::string& tmxFile);

	Point getObjPos(const std::string& groupName, const std::string& objName);
	Size getObjSize(const std::string& groupName, const std::string& objName);

	bool isCollision(Point p1, Point p2);
	bool isCollision(Point p);
	bool isPointOutMap(Point p);
	Point tileCoordForPosition(Point position);
	bool isSteel(Point p);
	bool isGrass(Point p);

	void update(float dt);
	Vector <Tank *> tankSet;
	Vector <Props *> propSet;
	TMXLayer *layer1;

	void addProps(const std::string &name, const std::string &posName, PROP_TYPE type, float ctime, float wtime = 10.0f);

private:
	Size mapSize;
	Size tileSize;
};

#endif