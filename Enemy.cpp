#include "Enemy.h"
#include "Bullet.h"
#include "MapLayer.h"

bool Enemy::init()
{
	collidedEnemy = nullptr;
	m_direction = DIRECTION::DOWN;
	this->setRotation(180.0f);

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
	m_moveSpeed = moveSpeed;
	m_shootSpeed = shootSpeed;
	HP = 100;
}

void Enemy::update(float dt)
{
	if (HP <= 0)
	{
		PlaySoundA("F:\\cocos_cpp\\comeback1\\Resources\\sounds\\eexplosion.wav", NULL, SND_FILENAME | SND_ASYNC);
		auto animation = AnimationCache::getInstance()->animationByName("enemyboom");
		auto action = Animate::create(animation);
		this->removeAllChildrenWithCleanup(true);
		gameLayer->m_map->tankSet.eraseObject(this);
		this->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {this->removeFromParentAndCleanup(true); }, NULL));
		unscheduleUpdate();
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
				HP -= 5;
				log("loss 20, %d", HP);
				collidedEnemy = nullptr;
			}
		}
		return true;
	}
	return false;
}

void Enemy::autoMove()
{
	//随机选择一个方向进行移动，如果碰撞则换一个方向继续移动
	struct timeval psv;
	gettimeofday(&psv, NULL);    // 计算时间种子   
	unsigned int tsrans = psv.tv_sec * 1000 + psv.tv_usec / 1000;    // 初始化随机数   
	srand(tsrans);

	this->schedule([&](float dt) {
		auto rect = this->getBoundingBox();
		auto flag = false;

		if (m_direction == DIRECTION::UP)
		{
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideObject(i, rect.getMaxY() + (int)m_moveSpeed))
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
				this->setPositionY(this->getPositionY() + (int)m_moveSpeed);
		}

		else if (m_direction == DIRECTION::DOWN)
		{
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideObject(i + 0.000003, rect.getMinY() - (int)m_moveSpeed))
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
				this->setPositionY(this->getPositionY() - (int)m_moveSpeed);
		}

		else if (m_direction == DIRECTION::LEFT)
		{
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideObject(rect.getMinX() - (int)m_moveSpeed, i))
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
				this->setPositionX(this->getPositionX() - (int)m_moveSpeed);
		}

		else if (m_direction == DIRECTION::RIGHT)
		{
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideObject(rect.getMaxX() + (int)m_moveSpeed, i))
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
				this->setPositionX(this->getPositionX() + (int)m_moveSpeed);
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
		auto m_bullet = this->shootOneBullet();//打出来的子弹

		gameLayer->m_map->addChild(m_bullet, -1, "bullet");
		m_bullet->setPosition(this->getPosition());//其实可以这样粗略处理子弹位置
		//auto tempDir = tank->m_direction;//获取按下键时的方向

		m_bullet->m_direction = this->m_direction;
		m_bullet->tankShootSpeed = (int)this->m_shootSpeed;
		//m_bullet->map = gameLayer->m_map;
		m_bullet->gameLayer = gameLayer;
		m_bullet->addController();
		//m_bullet->scheduleUpdate();
	}, 1.0f, kRepeatForever, 0.0f, "auto_fire");
	
}
