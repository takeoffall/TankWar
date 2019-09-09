#ifndef __UI_CONTROLS_H__
#define __UI_CONTROLS_H__

#include "cocos2d.h"
USING_NS_CC;

#include "Props.h"
#include "ui/CocosGUI.h"
using namespace ui;

class BaseControls : public Sprite
{
public:
	bool isTouched;
	Node *keySr;//关键先生,就好比Vector内置一个vector用来同步
};


class Inventory : public BaseControls
{
public:
	static Inventory* create(const std::string& name);
	virtual bool init();
	void update(float dt);//check keySr.
	void addItem(Props* p);
	void addItem(Node *node);
	void addItem(const std::string& file);
	void addItem(void* data) {}
	void removeItem(const std::string& name, bool removeAll);
	void setSprite(const std::string& filename) {
		this->initWithFile(filename);
	}

private:
	//数据结构
	class grid : public MenuItemSprite
	{
	public:
		void addImage(Sprite* sprite)//外部接口
		{
			keySr = sprite;
			this->setNormalImage(sprite);
		}
		grid(Node *node)
		{
			keySr = nullptr;
			defaultImage = node;
			isEmpty = true;
			setName("");
		}
		static grid* create(Node* normalSprite, Node* selectedSprite, const ccMenuCallback& callback)
		{
			grid *ret = new (std::nothrow) grid(normalSprite);//把normalSprite当做默认图片(也就是格子)
			ret->initWithNormalSprite(normalSprite, selectedSprite, nullptr, callback);
			ret->autorelease();
			return ret;
		}
		
		void update(float dt)//每个格子自己检测
		{
			if (!keySr)
			{
				setNormalImage(defaultImage);
			}
		}
		Sprite* keySr;
		Node* defaultImage;

		std::string scheduleString;
		bool isEmpty;
		std::string description;
		
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

class PlayerHP : public BaseControls
{
public:
	static PlayerHP* create(const std::string& name, const std::string& source);//等以后有美术了可以用getBgSprite()等函数供外部接口修改
	virtual bool init(const std::string& source);
	void update(float dt) {}
	std::string description;
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