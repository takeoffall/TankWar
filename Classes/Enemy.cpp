#include "Enemy.h"
#include "Bullet.h"
#include "MapLayer.h"

#include "AudioEngine.h"
using namespace experimental;

bool Enemy::init()
{
	/*collidedEnemy = nullptr;
	m_direction = DIRECTION::DOWN;*/
	this->setRotation(180.0f);

	//initBorn();
	addHPUI();
	autoMove();
	autoFire();
	scheduleUpdate();
	return true;
}

Enemy* Enemy::create(const std::string& filename, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	Enemy *enemy = new (std::nothrow) Enemy(moveSpeed, shootSpeed);
	if (enemy && enemy->initWithSpriteFrameName(filename) && enemy->init())
	{
		enemy->autorelease();
		return enemy;
	}
	CC_SAFE_DELETE(enemy);
	return nullptr;
}

Enemy::Enemy(MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	//file_name = file;
	this->moveSpeed = (int)moveSpeed;
	this->shootSpeed = (int)shootSpeed;
	HP = 100;
	isPause = false;
	collidedEnemy = nullptr;
	m_direction = DIRECTION::DOWN;
}

void Enemy::update(float dt)
{
	if (HP <= 0)
	{
		AudioEngine::play2d("sounds/explosion-enemy.mp3");

		auto animation = AnimationCache::getInstance()->getAnimation("enemyboom");
		auto action = Animate::create(animation);
		//this->removeAllChildrenWithCleanup(true);
		this->runAction(Sequence::create(action, CallFunc::create([&]() {
			
			gameLayer->totalScore += 10;
			gameLayer->m_map->genRandomProp();
			gameLayer->genEnemyRandom();
			gameLayer->m_map->tankSet.eraseObject(this);
		}), CCRemoveSelf::create(), NULL));

		unscheduleUpdate();
	}
}

void Enemy::initBorn()
{
	//this->setVisible(false);
	auto animation = AnimationCache::getInstance()->getAnimation("enemyborn");
	auto action = Animate::create(animation);
	//this->runAction(Sequence::create(action, CallFunc::create([this]() {this->initWithSpriteFrameName(file_name); }), NULL));
}

void Enemy::waitForDie(int damage)
{
	HP -= damage;
	if (HP > 0)
	{
		bloodBar->setPercentage(HP);
	}
	else
	{
		AudioEngine::play2d("sounds/fexplosion.mp3");
		gameLayer->m_map->tankSet.eraseObject(this);
		gameLayer->totalScore += 10;
		gameLayer->m_map->genRandomProp();
		gameLayer->genEnemyRandom();

		_actionManager->resumeTarget(this);
		auto animation = AnimationCache::getInstance()->getAnimation("enemyboom");
		auto action = Animate::create(animation);
		this->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {this->removeFromParentAndCleanup(true); }, NULL));
	}
}

void Enemy::addHPUI()
{
	auto bloodbg = Sprite::create("blood_bar_bg.png");
	bloodbg->setPosition(Vec2(this->getContentSize().width/2, this->getContentSize().height));
	this->addChild(bloodbg);

	bloodBar = ProgressTimer::create(Sprite::create("blood_bar.png"));
	bloodBar->setType(ProgressTimer::Type::BAR);
	bloodBar->setBarChangeRate(Point(1, 0));
	bloodBar->setMidpoint(Point(0, 0));
	bloodBar->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height));
	this->addChild(bloodBar);

	schedule([&](float dt) {
		if (this->HP >= 0 && this->HP <= 100)
			bloodBar->setPercentage(this->HP);
	}, 0.0f, kRepeatForever, 0.0f, "HP");

}

bool Enemy::isCollideObject(float x, float y)
{
	auto a = this->isCollideMap(x, y);
	auto b = this->isCollideTank(x, y);
	if (a || b)
	{
		if (b)
		{
			if (collidedEnemy != nullptr && collidedEnemy->isProtectedUP)
			{
				dropBlood(1);
				//log("loss 20, %d", HP);
				collidedEnemy = nullptr;
			}
		}
		return true;
	}
	return false;
}

void Enemy::autoMove()
{
	//���ѡ��һ����������ƶ��������ײ��һ����������ƶ�
	struct timeval psv;
	gettimeofday(&psv, NULL);    // ����ʱ������   
	unsigned int tsrans = psv.tv_sec * 1000 + psv.tv_usec / 1000;    // ��ʼ�������   
	srand(tsrans);

	this->schedule([&](float dt) {
		auto rect = this->getBoundingBox();
		auto flag = false;

		if (m_direction == DIRECTION::UP)
		{
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideObject(i, rect.getMaxY() + moveSpeed))
				{
					while (!isCollideObject(i, rect.getMaxY() + 1))
					{
						this->setPositionY(this->getPositionY() + 1);
						rect = this->getBoundingBox();
					}
					flag = true;
					adjustDirection();
					break;
				}
			}
			if (!flag)
				this->setPositionY(this->getPositionY() + moveSpeed);
		}

		else if (m_direction == DIRECTION::DOWN)
		{
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideObject(i + 0.000003, rect.getMinY() - moveSpeed))
				{
					while (!isCollideObject(i, rect.getMinY() - 1))
					{
						this->setPositionY(this->getPositionY() - 1);
						rect = this->getBoundingBox();
					}
					flag = true;
					adjustDirection();
					break;
				}
			}
			if (!flag)
				this->setPositionY(this->getPositionY() - moveSpeed);
		}

		else if (m_direction == DIRECTION::LEFT)
		{
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideObject(rect.getMinX() - moveSpeed, i))
				{
					while (!isCollideObject(rect.getMinX() - 1, i))
					{
						this->setPositionX(this->getPositionX() - 1);
						rect = this->getBoundingBox();
					}
					flag = true;
					adjustDirection();
					break;
				}
			}
			if (!flag)
				this->setPositionX(this->getPositionX() - moveSpeed);
		}

		else if (m_direction == DIRECTION::RIGHT)
		{
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideObject(rect.getMaxX() + moveSpeed, i))
				{
					while (!isCollideObject(rect.getMaxX() + 1, i))
					{
						this->setPositionX(this->getPositionX() + 1);
						rect = this->getBoundingBox();
					}
					flag = true;
					adjustDirection();
					break;
				}
			}
			if (!flag)
				this->setPositionX(this->getPositionX() + moveSpeed);
		}

	}, 0.0f, kRepeatForever, 0.0f, "auto_move");
	
}

void Enemy::adjustDirection()
{
	int a = CCRANDOM_0_1() * 4;
	if (a >= 0 && a < 1) {
		this->setRotation(-90.0f);
		this->m_direction = DIRECTION::LEFT;
	}
	else if (a >= 1 && a < 2) {
		this->setRotation(0.0f);
		this->m_direction = DIRECTION::UP;
	}
	else if (a >= 2 && a < 3) {
		this->setRotation(90.0f);
		this->m_direction = DIRECTION::RIGHT;
	}
	else {
		this->setRotation(180.0f);
		this->m_direction = DIRECTION::DOWN;
	}
}

void Enemy::autoFire()
{
	schedule([&](float dt) {
		auto m_bullet = this->shootOneBullet();//��������ӵ�

		gameLayer->m_map->addChild(m_bullet, -1, "bullet");
		m_bullet->setPosition(this->getPosition());//��ʵ�����������Դ����ӵ�λ��
		//auto tempDir = tank->m_direction;//��ȡ���¼�ʱ�ķ���

		m_bullet->m_direction = this->m_direction;
		m_bullet->tankShootSpeed = this->shootSpeed;
		//m_bullet->map = gameLayer->m_map;
		m_bullet->gameLayer = gameLayer;
		m_bullet->addController();
		//m_bullet->scheduleUpdate();
	}, 1.0f, kRepeatForever, 0.0f, "auto_fire");
	
}
