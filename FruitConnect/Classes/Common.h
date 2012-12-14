/*
 * Common.h
 *
 *  Created on: Sep 29, 2012
 *      Author: liuyaxin
 */

#ifndef COMMON_H_
#define COMMON_H_

#define SAVE_STRING CCUserDefault::sharedUserDefault()->setStringForKey
#define SAVE_INTEGER CCUserDefault::sharedUserDefault()->setIntegerForKey
#define LOAD_STRING CCUserDefault::sharedUserDefault()->getStringForKey
#define LOAD_INTEGER CCUserDefault::sharedUserDefault()->getIntegerForKey
#define FLUSH_DATA CCUserDefault::sharedUserDefault()->flush()

#define CLASSIC_LEVEL_NO 100
#define CLASSIC_LEVEL_COLUMN_NO 5
#define CLASSIC_LEVEL_PAGE_NO 25

#define MUSHROOM_LEVEL_NO 100
#define MUSHROOM_LEVEL_COLUMN_NO 5
#define MUSHROOM_LEVEL_PAGE_NO 25

#define CLASSIC_MODE 1
#define MUSHROOM_MODE 2
#define ENDLESS_MODE 3

#define TILE_NO 10

enum SceneTag {
  SCENE_MENU, SCENE_LEVEL, SCENE_GAME, SCENE_SHOP  
};

enum LayerTag {
  LAYER_MENU, LAYER_LEVEL, LAYER_GAME, LAYER_SHOP 
};

#endif /* COMMON_H_ */
