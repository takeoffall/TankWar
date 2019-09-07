#include "BaseModel.h"
#include "BaseUI.h"
#include "cocos2d.h"
USING_NS_CC;


class BaseController : public Node
{								
public:
	virtual bool init() = 0;

	BaseModel *baseModel;
	BaseUI *baseUI;
};