#ifndef __PROPS_H__
#define __PROPS_H__


#include "cocos2d.h"
USING_NS_CC;
//class MapLayer;

enum class PROP_TYPE
{
	ADD_BLOOD = 1,
	PROTECTED,
	START,//̹�˳�ʼ��������?
	SPADE,//����
	MINE,//��
	TIMER,//��ס
	XINGXING,//�ӵ�����
};


class Props : public Sprite
{
public:
	static Props* createWithPropName(const std::string &sourceName, PROP_TYPE type, float ctime, float wtime = 10.0f);
	virtual bool init(PROP_TYPE type);
	void update(float dt);
	//MapLayer* map;

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
	std::string m_description;//��������

	float m_ctime;//����ʱ��
	float m_wtime;//�ȴ�ʱ��
};

#endif