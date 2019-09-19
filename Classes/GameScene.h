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
	
	void testPlist(const std::string& file);//测试plist里面的图片

	void addBackButton();//添加返回按钮
	
	//敌人可以加道具，打死敌人后获得他的道具，以后再说

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

	void listenControlScaling();//控件行为
	void listenControlMoving();

public:
	int totalScore;//总分，双人游戏两人合用分数
	int level;//当前关卡
	Size vSize;//可视区域大小
	HelloWorld * tsm;//场景管理器指针
	std::string tmxFile;//外部传进来的地图文件
	MapLayer* m_map;


	Tank* player1;
	Tank* player2;
	
	Vector <BaseControls *> mycontrols;//便于统一管理
	BaseControls* touchedControls;//当前鼠标作用的控件
};