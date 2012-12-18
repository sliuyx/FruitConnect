//
//  CommonFunctions.h
//  FruitConnect
//
//  Created by 柳亚鑫 on 12-10-23.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef FruitConnect_CommonFunctions_h
#define FruitConnect_CommonFunctions_h
#include "Common.h"
#include "cocos2d.h"

USING_NS_CC;

int getNumberLength(int number);

int getGoldCount();

int getTipItemCount();

int getRerangeItemCount();

int getTimeItemCount();

void saveGoldCount(int value);

void saveTipItemCount(int value);

void saveRerangeItemCount(int value);

void saveTimeItemCount(int value);

bool isMusicOn();

void setMusicOn(bool value);

void playMusic(const char* music);

void callbackMenu();

void showMenuAd();

void showLevelAd();

void showGameAd();

void showGamePauseAd();

void showShopAd();

CCString* getPngPath(const char* path);

#endif
