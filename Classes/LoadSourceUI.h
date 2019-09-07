#ifndef _LOAD_SOURCE_UI_H_
#define _LOAD_SOURCE_UI_H_

#include "BaseUI.h"
#include "cocos2d.h"
USING_NS_CC;

class LoadSourceUI : public BaseUI
{
public:
	CREATE_FUNC(LoadSourceUI);
	virtual bool init();

	void display();
	void update(float dt);
	void loadingCallBack(Ref* pSender);

private:
	ProgressTimer* loadProgress;//进度条
	LabelTTF* percentLabel;//加载进度label
	LabelTTF* loadLabel;//显示loading: 的label
	

	int total;
	int beenLoaded;
};

#endif