#include "LoadSourceUI.h"

bool LoadSourceUI::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	loadLabel = LabelTTF::create("Loading", "Arial", 20);//������ʾLoading: ��label
	loadLabel->setPosition(visibleSize.width / 2 - 30, visibleSize.height / 2 + 30);
	this->addChild(loadLabel);

	percentLabel = LabelTTF::create("0%", "Arial", 20);//������ʾ�ٷֱȵ�label
	percentLabel->setPosition(visibleSize.width / 2 + 35, visibleSize.height / 2 + 30);
	this->addChild(percentLabel);

	auto loadBg = Sprite::create("slider_bar.png");//�������ĵ�ͼ
	loadBg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(loadBg);

	loadProgress = ProgressTimer::create(Sprite::create("silder_progressBar.png"));//����һ��������
	loadProgress->setBarChangeRate(Vec2(1, 0));//���ý������ı仯����
	loadProgress->setType(ProgressTimer::Type::BAR);//���ý�����������
	loadProgress->setMidpoint(Vec2(0, 1));//���ý��ȵ��˶�����
	loadProgress->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	loadProgress->setPercentage(0.0f);//���ó�ʼֵΪ0
	this->addChild(loadProgress, 2);
	//����81��ͼƬ��ÿ������һ�ž͵��ûص�����:loadingCallback(ps:ԭ��ʹ�õ��������׶��������������Լ�������һ�£�
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
	++beenLoaded;//ÿ�����������һ�Σ���beenLoaded + 1

	char buf_str[16];
	sprintf(buf_str, "%d%%", (int)(((float)beenLoaded / total) * 100));
	percentLabel->setString(buf_str);//����percentLabel��ֵ percentLabel's value

	float newPercent = 100 - (total - beenLoaded) / ((float)total / 100);//�����������ǰ�İٷֱ� 

	//��Ϊ����ͼƬ�ٶȺܿ죬���Ծ�û��ʹ��ProgressTo
	//����ProgressFromTo���ֶ��������½�����
	loadProgress->setPercentage(newPercent);

	//ͼƬ������ɺ�
	if (beenLoaded == total) {
		this->removeChild(loadProgress);//����ӵļ�������ɾ����object
		this->removeChild(percentLabel);
		this->removeChild(loadLabel);
		//�������Ҫ��ת��gotoNewLayer,��������Դ����µ�Scene���µ�Layer����������ʲô���߰����
		//this->goMenuLayer();
	}
}

void LoadSourceUI::update(float dt)
{

}
