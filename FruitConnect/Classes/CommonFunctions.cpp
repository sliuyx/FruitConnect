//
//  CommonFunctions.cpp
//  FruitConnect
//
//  Created by 柳亚鑫 on 12-10-27.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//
#include "CommonFunctions.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

int getNumberLength(int number) {
    int count = 1;
    while (true) {
        number = number / 10;
        if (number == 0) {
            break;
        }
        count++;
    }
    return count; 
}

int getGoldCount() {
    return LOAD_INTEGER("gold_count", 100);
}

int getTipItemCount() {
    return LOAD_INTEGER("tip_item_count", 10);
}

int getRerangeItemCount() {
    return LOAD_INTEGER("rerange_item_count", 10);
}

int getTimeItemCount() {
    return LOAD_INTEGER("time_item_count", 10);
}

void saveGoldCount(int value) {
    SAVE_INTEGER("gold_count", value);
}

void saveTipItemCount(int value) {
    SAVE_INTEGER("tip_item_count", value);
}

void saveRerangeItemCount(int value) {
    SAVE_INTEGER("rerange_item_count", value);
}

void saveTimeItemCount(int value) {
    SAVE_INTEGER("time_item_count", value);
}

bool isMusicOn() {
    return (LOAD_INTEGER("music_on", 1) == 1);
}

void setMusicOn(bool value) {
    if (value) {
        SAVE_INTEGER("music_on", 1);
    } else {
        SAVE_INTEGER("music_on", 0);
    }
}

void playMusic(const char* music) {
    if (isMusicOn()) {
        SimpleAudioEngine::sharedEngine()->playEffect(music);
    }
}


