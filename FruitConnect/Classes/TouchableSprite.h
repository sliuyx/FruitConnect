/*
 * TouchableSprite.h
 *
 *  Created on: Oct 6, 2012
 *      Author: liuyaxin
 */

#ifndef TOUCHABLESPRITE_H_
#define TOUCHABLESPRITE_H_
#include "cocos2d.h"

USING_NS_CC;

class TouchableSprite : public CCSprite, public CCTargetedTouchDelegate {
public:
  TouchableSprite();
  virtual ~TouchableSprite();

  static TouchableSprite* touchSpriteWithFile(const char* file);

  bool initWithFile(const char* file);

  virtual void onEnter();
  virtual void onExit();

  CCRect rect();
  bool containsTouchLocation(CCTouch *touch);

  virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
  virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
  virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);

};

#endif /* TOUCHABLESPRITE_H_ */
