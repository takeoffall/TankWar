#include "cocos2d.h"

USING_NS_CC;

class BaseModel : public Node
{
public:
	virtual bool init() = 0;
};