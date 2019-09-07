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
	void addPlayer1(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addPlayer2(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addPlayer1_HP(const std::string& bg, const std::string& source, Point pos);
	void addPlayer2_HP(const std::string& bg, const std::string& source, Point pos);
	void addTank(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addEnemy(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed, const std::string &posName);
	void genEnemyRandom();
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
	Tank* player1;
	Tank* player2;
	
	ProgressTimer* progress;
	Inventory* inventory;
	PlayerHP* php;
	PlayerHP* playerHP_1;
	PlayerHP* playerHP_2;
	Vec2 currentPoint;
	//����
	int tankSpeed;
	//����
	Bullet* m_bullet;
};