#include "cocos2d.h"
#include "HelloWorldScene.h"

//#include "Enemy.h"
//#include "RelationEvent.h"
//#include "TankController.h"
#include "UIControls.h"
USING_NS_CC;

#include "Tank.h"
class Enemy;
class Bullet;
class MapLayer;

class GameLayer : public Layer
{
public:
	static GameLayer * createWithMap(const std::string& tmxFile);
	virtual bool init(const std::string& tmxFile);
	
	void addInventory();
	void addBackButton();//�����Ʒ��
	void addPlayerHP();//���Ѫ����ʾ
	void EX_AddProp(const std::string& name);//���ⲿ�ӿ����ui
	void EX_AddProp(Sprite* s);
	//���˿��Լӵ��ߣ��������˺������ĵ���
	void EX_RemoveProp(const std::string& name, bool removeAll);

	void addProps(const std::string &name, const std::string &posName);
	
	void initActionSet();
	void addMap();
	void addTank(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addEnemy(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed, const std::string &posName);

	void update(float delta);

public:
	Size vSize;
	HelloWorld * tsm;
	
	//RelateEvent *eventHandler;//�¼�������
	std::string tmxFile;
	MapLayer* m_map;

	//����
	Tank *tank;
	
	ProgressTimer* progress;
	Inventory* inventory;
	Vec2 currentPoint;
	int tankSpeed;

	Bullet* m_bullet;
	//Vector<Bullet *> m_bulletFromTank;
};