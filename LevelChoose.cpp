#include "LevelChoose.h"

bool LevelLayer::init()
{
	if (!Layer::init()) {return false;}
	
	display_levels();
	display_backbtn();
	return true;
}

void LevelLayer::display_backbtn()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();


	auto back = Button::create();
	back->setTitleText("Back");
	back->setTitleFontSize(40);
	back->setPosition(Vec2(visibleSize.width / 2, back->getContentSize().height));
	back->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			log("back");
			break;
		case Widget::TouchEventType::ENDED:
			tsm->goMenuScene();
			break;
		default:
			break;

		}
	});
	this->addChild(back);
}

void LevelLayer::display_levels()
{
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size winSize = Director::getInstance()->getWinSize();

	PageView *pageView = PageView::create();
	pageView->setTouchEnabled(true);
	pageView->setSize(Size(winSize.width, winSize.height));
	pageView->setPosition(Point(0, 0));

	//´´½¨20¸öpage
	for (int i = 1; i <= 20; i++) {
		Layout* layout = Layout::create();
		layout->setSize(Size(winSize.width, winSize.height));

		ImageView* imageView = ImageView::create(StringUtils::format("map%d.png", i));
		imageView->setScale9Enabled(true);
		//imageView->setContentSize(Size(600, 600));
		imageView->setPosition(Point(layout->getContentSize().width / 2.0f, layout->getContentSize().height / 2.0f));
		imageView->setTouchEnabled(true);
		imageView->addTouchEventListener([this, i](Ref* pSender, Widget::TouchEventType type) {
			switch (type)
			{
			case Widget::TouchEventType::BEGAN:
				log("touch page %d", i);
				break;
			case Widget::TouchEventType::ENDED:
				tsm->goGameSceneWithMap(i);
				break;
			default:
				break;

			}
		});
		layout->addChild(imageView);

		Text* label = Text::create(StringUtils::format("level %d", i), "fonts/Marker Felt.ttf", 24);
		label->setColor(Color3B(255, 0, 0));
		label->setPosition(Point(layout->getContentSize().width / 2.0f, layout->getContentSize().height / 2.0f));
		layout->addChild(label);

		pageView->addPage(layout);
		pageView->setSwallowTouches(false);
	}
	pageView->addEventListenerPageView(this, pagevieweventselector(LevelLayer::pageViewEvent));
	this->addChild(pageView);
}

void LevelLayer::pageViewEvent(Ref* pSender, PageViewEventType type) {

	switch (type) {
	case PAGEVIEW_EVENT_TURNING:
	{
		PageView* pageView = dynamic_cast<PageView*>(pSender);
		log("pageView->getCurPageIndex() = %d", pageView->getCurrentPageIndex() + 1);
	}
	break;

	default:
		break;
	}
}