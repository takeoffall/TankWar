#include "LoadSourceUI.h"

bool LoadSourceUI::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	loadLabel = LabelTTF::create("Loading", "Arial", 20);//创建显示Loading: 的label
	loadLabel->setPosition(visibleSize.width / 2 - 30, visibleSize.height / 2 + 30);
	this->addChild(loadLabel);

	percentLabel = LabelTTF::create("0%", "Arial", 20);//创建显示百分比的label
	percentLabel->setPosition(visibleSize.width / 2 + 35, visibleSize.height / 2 + 30);
	this->addChild(percentLabel);

	auto loadBg = Sprite::create("slider_bar.png");//进程条的底图
	loadBg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(loadBg);

	loadProgress = ProgressTimer::create(Sprite::create("silder_progressBar.png"));//创建一个进程条
	loadProgress->setBarChangeRate(Vec2(1, 0));//设置进程条的变化速率
	loadProgress->setType(ProgressTimer::Type::BAR);//设置进程条的类型
	loadProgress->setMidpoint(Vec2(0, 1));//设置进度的运动方向
	loadProgress->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	loadProgress->setPercentage(0.0f);//设置初始值为0
	this->addChild(loadProgress, 2);
	//加载81张图片，每加载完一张就调用回调函数:loadingCallback(ps:原文使用的是清晰易懂方法，这里我自己进化了一下）
	for (int i = 7001; i < 7082; i++) {
		std::string imgName = StringUtils::format("%d.png", i);
		Director::getInstance()->getTextureCache()->addImageAsync(imgName, CC_CALLBACK_1(LoadSourceUI::loadingCallBack, this));
	}
	return true;
}

void LoadSourceUI::display()
{

}

void LoadSourceUI::loadingCallBack(Ref* pSender)
{
	++beenLoaded;//每进到这个函数一次，让beenLoaded + 1

	char buf_str[16];
	sprintf(buf_str, "%d%%", (int)(((float)beenLoaded / total) * 100));
	percentLabel->setString(buf_str);//更新percentLabel的值 percentLabel's value

	float newPercent = 100 - (total - beenLoaded) / ((float)total / 100);//计算进度条当前的百分比 

	//因为加载图片速度很快，所以就没有使用ProgressTo
	//或者ProgressFromTo这种动作来更新进度条
	loadProgress->setPercentage(newPercent);

	//图片加载完成后
	if (beenLoaded == total) {
		this->removeChild(loadProgress);//将添加的几个对象删除掉object
		this->removeChild(percentLabel);
		this->removeChild(loadLabel);
		//加载完既要跳转到gotoNewLayer,在这里可以创建新的Scene，新的Layer，或者其他什么乱七八糟的
		//this->goMenuLayer();
	}
}

void LoadSourceUI::update(float dt)
{

}
