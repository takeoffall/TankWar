#ifndef __MODAL_LAYER_H__
#define __MODAL_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class ModalLayer : public Layer
{
public:

	ModalLayer(const std::string& bgImg);
	static ModalLayer* create(const std::string& bgImg);
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	Sprite* bgSprite;
private:
	
	Button* confirm;
	Button* cancel;
};

#endif