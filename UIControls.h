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
	bool isTouched;
	virtual bool init();
	void addItem(Props* p);
	void addItem(Sprite* s);
	void addItem(const std::string &name);
	void removeItem(const std::string& name, bool removeAll);
	
private:

	class grid : public MenuItemSprite
	{
	public:
		grid()
		{
			isEmpty = true;
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
	void printDialog();
	void clearLabel(grid* g);

	//Vector <String > c_stringSet;
	grid *a, *b, *c, *d, *e, *f;
	Menu *itemsMenu;
	Vector <grid *> grids;
	Label* label;
};

#endif