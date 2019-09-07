#ifndef __UI_CONTROLS_H__
#define __UI_CONTROLS_H__

#include "cocos2d.h"
USING_NS_CC;

#include "Props.h"
#include "ui/CocosGUI.h"
using namespace ui;


class Inventory : public Sprite
{
public:
	static Inventory* create(const std::string& name);
	virtual bool init();
	void addItem(Props* p);
	void removeItem(const std::string& name, bool removeAll);
	void setSprite(const std::string& filename) {
		this->initWithFile(filename);
	}

	bool isTouched;
private:
	//数据结构
	class grid : public MenuItemSprite
	{
	public:
		grid()
		{
			isEmpty = true;
			setName("");
		}
		std::string scheduleString;
		bool isEmpty;
		std::string description;
		static grid* create(Node* normalSprite, Node* selectedSprite, const ccMenuCallback& callback)
		{
			grid *ret = new (std::nothrow) grid();
			ret->initWithNormalSprite(normalSprite, selectedSprite, nullptr, callback);
			ret->autorelease();
			return ret;
		}
	};
	Vector <grid *> grids;
	//组件
	grid *a, *b, *c, *d, *e, *f;
	Menu *itemsMenu;
	Label* label;
	//表现方法
	void printDialog();
	void clearLabel(grid* g);
};

class PlayerHP : public Sprite
{
public:
	static PlayerHP* create(const std::string& name, const std::string& source);//等以后有美术了可以用getBgSprite()等函数供外部接口修改
	virtual bool init(const std::string& source);

	std::string description;
	bool isTouched;
	//外部接口足以随心改变很多
	Sprite* getBgSprite() {
		return m_bg;
	}
	ProgressTimer* getProgressBar() {
		return m_progressBar;
	}
	Label* getLabel() {
		return m_label;
	}

private:
	//组件
	ProgressTimer* m_progressBar;
	Sprite* m_bg;
	Label* m_label;//描述

	EventListenerMouse* m_mouseEventListener;
	void listenScaling();
};

#endif