#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "UIControls.h"
#include "Tank.h"

USING_NS_CC;
class Enemy;
class Bullet;
class MapLayer;

class GameLayer : public Scene
{
public:
	static GameLayer * createWithMap(const std::string& tmxFile);
	virtual bool init(const std::string& tmxFile);
	
	void testPlist(const std::string& file);//����plist�����ͼƬ

	void addBackButton();//��ӷ��ذ�ť
	
	//���˿��Լӵ��ߣ��������˺������ĵ��ߣ��Ժ���˵

	void initActionSet();
	void addMap();
	void addPlayer1(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addPlayer2(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);

	void addTank(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed);
	void addEnemy(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed, const std::string &posName);
	void genEnemyRandom();
	void update(float dt);
	void goNextLevel();
	void checkGameResult();

	void listenControlScaling();//�ؼ���Ϊ
	void listenControlMoving();

public:
	int totalScore;//�ܷ֣�˫����Ϸ���˺��÷���
	int level;//��ǰ�ؿ�
	Size vSize;//���������С
	HelloWorld * tsm;//����������ָ��
	std::string tmxFile;//�ⲿ�������ĵ�ͼ�ļ�
	MapLayer* m_map;


	Tank* player1;
	Tank* player2;
	
	Vector <BaseControls *> mycontrols;//����ͳһ����
	BaseControls* touchedControls;//��ǰ������õĿؼ�
};