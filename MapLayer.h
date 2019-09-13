#ifndef _MAPLAYER_H_
#define _MAPLAYER_H_

#include "cocos2d.h"
#include "Tank.h"

class Props;
USING_NS_CC;
//#include "Enemy.h"


class MapLayer : public cocos2d::TMXTiledMap
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
	bool childOutMap(Rect rect);//
	//void correctChildrenPosition();//
	Point tileCoordForPosition(Point position);
	bool isSteel(Point p);
	bool isGrass(Point p);

	void update(float dt);
	Vector <Tank *> tankSet;
	//Vector <Enemy *> enemySet;
	Vector <Props *> propSet;
	cocos2d::TMXLayer *layer1;
	cocos2d::TMXLayer *layer2;
	
	/*void addTank();
	void addEnemy();*/
	void addProps(const std::string &name, const std::string &posName, PROP_TYPE type, float ctime, float wtime = 10.0f);

	void genRandomProp();
	//void genRandomEnemy() {}

	Size mapSize;
	Size tileSize;
	int widthTiles;
	int heightTiles;
	bool isKingProtected;

private:
	
};

#endif