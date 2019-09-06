#include "cocos2d.h"
#include "HelloWorldScene.h"
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
	
	void addInventory();//�����Ʒ��
	void addBackButton();//��ӷ��ذ�ť
	void addPlayerHP();//���Ѫ����ʾ��˫����ϷʱӦ�����Ҹ�һ��
	void addPlayerHP(const std::string& name, const std::string& source);//����һ�����
	//void EX_AddProp(const std::string& name);//�ⲿ�ӿ�����Դ�������Ʒ����Ʒ��
	void EX_AddProp(Props* p);//�ⲿ�ӿ����ֳ���Ʒ���������Ʒ����Ʒ��
	/*template <class T>;
	void EX_AddProp(Props* p);*/
	//���˿��Լӵ��ߣ��������˺������ĵ��ߣ��Ժ���˵
	void EX_RemoveProp(const std::string& name, bool removeAll);

	void initActionSet();
	void addMap();
	void addTank(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addEnemy(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed, const std::string &posName);

	void update(float delta);
	void goNextLevel();
	void checkGameResult();

	bool scaleLock;
	bool moveLock;
	void listenControlScaling();//�ؼ���Ϊ
	void listenControlMoving();

public:
	int totalScore;//�ܷ֣�˫����Ϸ���˺��÷���
	int level;//��ǰ�ؿ�
	Size vSize;//���������С
	HelloWorld * tsm;//����������ָ��
	
	//RelateEvent *eventHandler;//�¼�������
	std::string tmxFile;
	MapLayer* m_map;

	//����
	Tank *tank;
	
	ProgressTimer* progress;
	Inventory* inventory;
	PlayerHP* php;
	Vec2 currentPoint;
	//����
	int tankSpeed;
	//����
	Bullet* m_bullet;
};