#include "MapLayer.h"
#include "Props.h"

#include "AudioEngine.h"
using namespace experimental;

MapLayer::MapLayer(const std::string& tmxFile)
{
	initWithTMXFile(tmxFile);
	ignoreAnchorPointForPosition(false);
	setAnchorPoint(Vec2(0.5, 0.5));

	layer1 = this->getLayer("bg1");
	mapSize = this->getContentSize();
	tileSize = this->getTileSize();
	this->autorelease();
}

MapLayer* MapLayer::create(const std::string& tmxFile)
{
	MapLayer *pRet = new(std::nothrow) MapLayer;
	if (pRet && pRet->init(tmxFile))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool MapLayer::init(const std::string& tmxFile)
{
	/*if (!TMXTiledMap::init())
		return false;*/
	initWithTMXFile(tmxFile);
	layer2 = this->getLayer("bg2");
	layer2->setVisible(false);
	isKingProtected = false;

	ignoreAnchorPointForPosition(false);
	setAnchorPoint(Vec2(0.5, 0.5));

	layer1 = this->getLayer("bg1");
	mapSize = this->getContentSize();
	tileSize = this->getTileSize();
	widthTiles = getMapSize().width;
	heightTiles = getMapSize().height;

	addProps("props-protect.png", "t1", PROP_TYPE::PROTECTED, 10.0f);
	addProps("props-protect.png", "t9", PROP_TYPE::PROTECTED, 10.0f);
	addProps("props-protect.png", "t3", PROP_TYPE::PROTECTED, 10.0f);
	addProps("props-spade.png", "t2", PROP_TYPE::SPADE, 5.0f);
	addProps("props-spade.png", "t4", PROP_TYPE::SPADE, 5.0f);
	//scheduleUpdate();
	return true;
}

void MapLayer::genRandomProp()
{
	auto message = Dictionary::createWithContentsOfFile("new_props.xml");    //读取xml文件，文件在Resources目录下
	auto dic = (__Dictionary *)message->randomObject();
	auto filename = dic->valueForKey("file_name")->getCString();
	auto _type = dic->valueForKey("type")->intValue();
	auto type = (PROP_TYPE)_type;
	auto ctime = dic->valueForKey("ctime")->floatValue();
	auto wtime = dic->valueForKey("wtime")->floatValue();
	auto allPos = (__Dictionary *)dic->objectForKey("pos");
	auto pos = ((__String *)allPos->randomObject())->getCString();
	addProps(filename, pos, type, ctime, wtime);
	
	//log("ctime: %f, wtime: %f", ctime, wtime);
	//2.直接资源名与内容映射
	
}

void MapLayer::addProps(const std::string &name, const std::string &posName, PROP_TYPE type, float ctime, float wtime)
{
	auto pos = getObjPos("objects", posName);
	auto objSize = getObjSize("objects", posName);

	auto s = Props::createWithPropName(name, type, ctime, wtime);
	s->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
	addChild(s);
	//AudioEngine::play2d("sounds/bonus-appear.mp3");
	propSet.pushBack(s);
	s->map = this;//
	s->addController();
}

Point MapLayer::getObjPos(const std::string& groupName, const std::string& objName)
{
	auto objGroup = this->getObjectGroup(groupName);
	auto obj = objGroup->getObject(objName);
	auto x = obj.at("x").asFloat();
	auto y = obj.at("y").asFloat();
	return Point(x, y);
}
Size MapLayer::getObjSize(const std::string& groupName, const std::string& objName)
{
	auto objGroup = this->getObjectGroup(groupName);
	auto obj = objGroup->getObject(objName);
	auto width = obj.at("width").asFloat();
	auto height = obj.at("height").asFloat();
	return Size(width, height);
}

bool MapLayer::isCollision(Point p1, Point p2)
{
	auto m = layer1->getTileGIDAt(p1);
	auto n = layer1->getTileGIDAt(p2);
	if (m == 0 && n == 0)
		return false;
	if ((m==0 || (m >= 8 && m <= 11) || (m >= 36 && m <= 39) || (m >= 64 && m <= 67) || (m >= 92 && m <= 95)) &&
		(n==0 || (n >= 8 && n <= 11) || (n >= 36 && n <= 39) || (n >= 64 && n <= 67) || (n >= 92 && n <= 95)))
		return false;
	//log("p1: %d, p2: %d", layer1->getTileGIDAt(p1), layer1->getTileGIDAt(p2));
	return true;
}

bool MapLayer::isPointOutMap(Point p)
{
	if (p.x >= 1 && p.x <= getMapSize().width - 2 && p.y >= 1 && p.y <= getMapSize().height - 2)
		return false;
	return true;
}

bool MapLayer::childOutMap(Rect rect)
{
	if (rect.getMinX() >= tileSize.width && rect.getMaxX() <= getContentSize().width - tileSize.width &&
		rect.getMinY() >= tileSize.height && rect.getMaxY() <= getContentSize().height - tileSize.height)
		return false;
	return true;
}

bool MapLayer::isCollision(Point p)
{
	if (isPointOutMap(p))
		return true;
	this->layer1->getTileAt(p);
	auto a = layer1->getTileGIDAt(p);
	if (a == 0 || (a>=8 && a<=11)||(a>=36 && a<=39)||(a>=64 && a<=67)||(a>=92 && a<=95)||
		(a>=12 && a<=19)||(a>=40 && a<=47)||(a>=68 && a<=75)||(a>=96 && a<=103))
		return false;
	return true;
}

Point MapLayer::tileCoordForPosition(Point position)
{
	int x = position.x / tileSize.width;
	int y = (this->getMapSize().height * tileSize.height - position.y) / tileSize.height;
	return Point(x, y);
}

bool MapLayer::isSteel(Point p)
{
	auto a = layer1->getTileGIDAt(p);
	if ((a >= 4 && a <= 7) || (a >= 32 && a <= 35) || (a >= 60 && a <= 63) || (a >= 88 && a <= 91))
		return true;
	return false;
}

bool MapLayer::isGrass(Point p)//简单检测坦克中心
{
	auto m = layer1->getTileGIDAt(p);
	if ((m >= 8 && m <= 11) || (m >= 36 && m <= 39) || (m >= 64 && m <= 67) || (m >= 92 && m <= 95))
		return true;
	return false;
}

void MapLayer::update(float dt)
{
	//correct children position in order to prevent they get out of map
	for (auto &i : getChildren())
	{
		auto rect = i->getBoundingBox();
		if (rect.getMinX() < tileSize.width)
			i->setPositionX(tileSize.width + i->getContentSize().width / 2);
		else if (rect.getMaxX() > mapSize.width - tileSize.width)
			i->setPositionX(mapSize.width - tileSize.width - i->getContentSize().width / 2);

		if (rect.getMinY() < tileSize.height)
			i->setPositionY(tileSize.height + i->getContentSize().height / 2);

		else if (rect.getMaxY() > mapSize.height - tileSize.height)
			i->setPositionY(mapSize.height - tileSize.height - i->getContentSize().height / 2);
			
	}
}