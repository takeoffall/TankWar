#ifndef __PROPS_H__
#define __PROPS_H__


#include "cocos2d.h"
USING_NS_CC;
class MapLayer;//以前注释
class PropController;

enum class PROP_TYPE
{
	ADD_BLOOD = 1,
	PROTECTED,
	START,//坦克初始化的样子?
	SPADE,//铲子
	MINE,//雷
	TIMER,//定住
	XINGXING,//子弹威力
};


class Props : public Sprite
{
public:
	Props()
	{
		isObtained = false;
	}
	static Props* createWithPropName(const std::string &sourceName, PROP_TYPE type, float ctime, float wtime = 10.0f);
	virtual bool init(PROP_TYPE type);
	void update(float dt);
	bool isDelete;
	MapLayer* map;//以前注释
	PropController* controller;
	void addController();
	bool isObtained;
	void disappear();
	void changeParent();
	const std::string getDescription() {
		return m_description;
	}
	PROP_TYPE getType() {
		return m_type;
	}
	float getContinuousTime() {
		return m_ctime;
	}
	
private:
	void checkTime();

	PROP_TYPE m_type;
	std::string m_description;//道具描述

	float m_ctime;//持续时间
	float m_wtime;//等待时间
};

#endif