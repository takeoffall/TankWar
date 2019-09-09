#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "UIControls.h"
#include "Tank.h"

#define __PLAYER_1__ "player1"
#define __PLAYER_2__ "player2"
USING_NS_CC;
class Enemy;
class Bullet;
class MapLayer;

class GameLayer : public Layer
{
public:
	static GameLayer * createWithMap(const std::string& tmxFile);
	virtual bool init(const std::string& tmxFile);
	
	void test(const std::string& file);

	void addInventory();//�����Ʒ��
	void addBackButton();//��ӷ��ذ�ť
	
	//���˿��Լӵ��ߣ��������˺������ĵ��ߣ��Ժ���˵

	void initActionSet();
	void addMap();
	void addPlayer1(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addPlayer2(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);

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

	//ui
	Tank* player1;
	Tank* player2;
	
	Vector <BaseControls *> mycontrols;
	BaseControls* touchedControls;
};