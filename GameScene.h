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
	
	void addInventory();//添加物品栏
	void addBackButton();//添加返回按钮
	void addPlayerHP();//猪脚血量显示，双人游戏时应该左右各一个
	void addPlayerHP(const std::string& name, const std::string& source);//关联一个猪脚
	//void EX_AddProp(const std::string& name);//外部接口以资源名添加物品到物品栏
	void EX_AddProp(Props* p);//外部接口以现成物品精灵添加物品到物品栏
	/*template <class T>;
	void EX_AddProp(Props* p);*/
	//敌人可以加道具，打死敌人后获得他的道具，以后再说
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

	//待定
	Tank *tank;
	
	ProgressTimer* progress;
	Inventory* inventory;
	PlayerHP* php;
	Vec2 currentPoint;
	//待定
	int tankSpeed;
	//待定
	Bullet* m_bullet;
};