#include "ModalLayer.h"

ModalLayer* ModalLayer::create(const std::string& bgImg)
{
	ModalLayer *modalLayer = new (std::nothrow) ModalLayer(bgImg);
	if (modalLayer && modalLayer->init())
	{
		modalLayer->autorelease();
		return modalLayer;
	}
	CC_SAFE_DELETE(modalLayer);
	return nullptr;
}

ModalLayer::ModalLayer(const std::string& bgImg)
{
	bgSprite = Sprite::create(bgImg);
}

bool ModalLayer::init()
{
	if (!Layer::init())
		return false;
	
	addChild(bgSprite, -1);

	confirm = Button::create("confirm.png", "confirm.png");
	confirm->setPosition(Vec2(confirm->getContentSize().width / 2, confirm->getContentSize().height / 2));
	bgSprite->addChild(confirm);
	confirm->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED)
		{
			this->getParent()->resume();
			auto c = this->getParent()->getChildren();
			c.eraseObject(this);
			for (auto &i : c)
			{
				i->resume();
			}
			this->removeFromParentAndCleanup(true);
		}
	});

	cancel = Button::create("cancel.png", "cancel.png");
	cancel->setPosition(Vec2(bgSprite->getContentSize().width - cancel->getContentSize().width / 2, cancel->getContentSize().height / 2));
	bgSprite->addChild(cancel);
	cancel->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED)
		{
			this->getParent()->resume();
			auto c = this->getParent()->getChildren();
			c.eraseObject(this);
			for (auto &i : c)
			{
				i->resume();
			}
			this->removeFromParentAndCleanup(true);
		}
	});

	
	
	return true;
}

void ModalLayer::onEnter()
{
	Layer::onEnter();
	this->setContentSize(this->getParent()->getContentSize());
	bgSprite->setPosition(getContentSize().width / 2, bgSprite->getContentSize().height / 2);

	this->getParent()->pause();
	auto a = this->getParent()->getChildren();
	a.eraseObject(this);
	for (auto &i : a)
	{
		i->pause();
	}
}

void ModalLayer::onExit()
{
	Layer::onExit();

	this->getParent()->resume();
	auto a = this->getParent()->getChildren();
	a.eraseObject(this);
	for (auto &i : a)
	{
		i->resume();
	}
}
