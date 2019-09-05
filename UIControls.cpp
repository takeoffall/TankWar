#include "UIControls.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

Inventory* Inventory::create(const std::string& name)
{
	Inventory *sprite = new (std::nothrow) Inventory();
	if (sprite && sprite->initWithFile(name) && sprite->init())
	{
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool Inventory::init()
{
	//物品说明
	label = Label::createWithTTF("物品说明", "fonts/SIMYOU.TTF", 12);
	label->setAnchorPoint(Vec2(0.5, 0.5));
	label->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height * 0.25));
	
	label->setDimensions(this->getContentSize().width, this->getContentSize().height * 0.25);
	label->setColor(Color3B::RED);
	this->addChild(label);
	//RichText::create();
	//物品栏
	isTouched = false;
	auto grid_a = Sprite::create("grid.png");
	auto grid_b = Sprite::create("grid.png");
	auto grid_c = Sprite::create("grid.png");
	auto grid_d = Sprite::create("grid.png");
	auto grid_e = Sprite::create("grid.png");
	auto grid_f = Sprite::create("grid.png");

	 a = grid::create(grid_a, Node::create(), [this](Ref *pSender) {
		if (a->isEmpty){
			label->setString("空");
		}else{
			//auto strings = FileUtils::getInstance()->getValueMapFromFile("hhh.xml");
			label->setString(a->description);
			printDialog();
			clearLabel(a);
		}
		

	});
	 b = grid::create(grid_b, Node::create(), [this](Ref *pSender) {
		 if (b->isEmpty){
			 label->setString("空");
		 }else{
			 label->setString(b->description);
			 printDialog();
			 clearLabel(b);
		 }
	 });
	 c = grid::create(grid_c, Node::create(), [this](Ref *pSender) {
		 if (c->isEmpty){
			 label->setString("空");
		 }else{
			 label->setString(c->description);
			 printDialog();
			 clearLabel(c);
		 }
	 });
	 d = grid::create(grid_d, Node::create(), [this](Ref *pSender) {
		 if (d->isEmpty){
			 label->setString("空");
		 }else{
			 label->setString(d->description);
			 printDialog();
			 clearLabel(d);
		 }
	 });
	 e = grid::create(grid_e, Node::create(), [this](Ref *pSender) {
		 if (e->isEmpty){
			 label->setString("空");
		 }else{
			 label->setString(e->description);
			 printDialog();
			 clearLabel(e);
		 }
	 });
	 f = grid::create(grid_f, Node::create(), [this](Ref *pSender) {
		 if (f->isEmpty){
			 label->setString("空");
		 }else{
			 label->setString(f->description);
			 printDialog();
			 clearLabel(f);
		 }
	 });
	 a->scheduleString = "aa";
	 b->scheduleString = "bb";
	 c->scheduleString = "cc";
	 d->scheduleString = "dd";
	 e->scheduleString = "ee";
	 f->scheduleString = "ff";

	grids.pushBack(a);
	grids.pushBack(b);
	grids.pushBack(c);
	grids.pushBack(d);
	grids.pushBack(e);
	grids.pushBack(f);

	itemsMenu = Menu::create(a, b, c, d, e, f, NULL);
	itemsMenu->setContentSize(Size(this->getContentSize().width, this->getContentSize().height * 0.75));
	itemsMenu->alignItemsInColumns(3, 3, NULL);
	//itemsMenu->alignItemsInRows(2, 2, 2, NULL);
	itemsMenu->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height - itemsMenu->getContentSize().height / 2));
	this->addChild(itemsMenu);
	//itemsMenu->setColor(Color3B(255, 0, 0));
	return true;
}

void Inventory::addItem(Props* p)
{
	for (auto &i : grids)
	{
		if (i->isEmpty)
		{
			p->setPosition(Point::ZERO);//閹煎鍠嶇花锟犲箣閹存縿鍋ㄥ鑸电濡炲倿姊?2019-9-5 morning
			i->isEmpty = false;
			i->setNormalImage(p);
			i->setName(p->getName());
			i->description = p->getDescription();
			//label->setString(p->getDescription());
			log("des: %s", i->description);
			break;
		}
	}
}

void Inventory::addItem(Sprite* s)
{
	for (auto &i : grids)
	{
		if (i->isEmpty)
		{
			s->setPosition(Point::ZERO);
			i->isEmpty = false;
			i->setNormalImage(s);
			i->setName(s->getName());
			break;
		}
	}
}

void Inventory::addItem(const std::string &name)
{
	auto s = Sprite::createWithSpriteFrameName(name);
	//auto newItem = MenuItemSprite::create(a, a, [](Ref* pSender) {});
	for (auto &i : grids)
	{
		if (i->isEmpty)
		{
			i->isEmpty = false;
			i->setNormalImage(s);
			i->setName(name);
			break;
		}
	}
}

void Inventory::removeItem(const std::string& name, bool removeAll)
{
	auto s = Sprite::create("grid.png");
	if (!removeAll)
	{
		for (auto &i : grids)
		{
			if (i->getName() == name)
			{
				i->setName("");
				i->isEmpty = true;
				i->setNormalImage(s);
				break;
			}
		}
	}
	else
	{
		for (auto &i : grids)
		{
			if (i->getName() == name)
			{
				i->setName("");
				i->isEmpty = true;
				i->setNormalImage(s);
			}
		}
	}
}

void Inventory::printDialog()
{
	int index = 0;
	while (label->getLetter(index) != nullptr)
	{
		label->getLetter(index)->setVisible(false);
		index++;
	}
	index = 0;
	while (label->getLetter(index) != nullptr)
	{
		label->getLetter(index)->runAction(
			Sequence::create(
				DelayTime::create(index * 0.1f),
				Show::create(), nullptr)
		);
		index++;
	}
}

void Inventory::clearLabel(grid* g)
{
	auto a = grids;
	a.eraseObject(g);
	for (auto &i : grids)
	{
		unschedule(i->scheduleString);
	}
	schedule([this, g](float dt) {
		if (g->isEmpty) {
			label->setString("空");
		}
	}, 0.0f, g->scheduleString);
}