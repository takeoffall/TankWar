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

	void addInventory();//添加物品栏
	void addBackButton();//添加返回按钮
	
	//敌人可以加道具，打死敌人后获得他的道具，以后再说

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
	void listenControlScaling();//控件行为
	void listenControlMoving();

public:
	int totalScore;//总分，双人游戏两人合用分数
	int level;//当前关卡
	Size vSize;//可视区域大小
	HelloWorld * tsm;//场景管理器指针
	
	//RelateEvent *eventHandler;//事件处理器
	std::string tmxFile;
	MapLayer* m_map;

	//ui
	Tank* player1;
	Tank* player2;
	
	Vector <BaseControls *> mycontrols;
	BaseControls* touchedControls;
};