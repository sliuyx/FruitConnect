//
//  ParticleWhiteSun.h
//  FruitConnect
//
//  Created by 柳亚鑫 on 12-11-3.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef FruitConnect_ParticleWhiteSun_h
#define FruitConnect_ParticleWhiteSun_h

#include "cocos2d.h"

NS_CC_BEGIN

class CC_DLL CCParticleWhiteSun : public CCParticleSystemQuad
{
public:
    CCParticleWhiteSun(){}
    virtual ~CCParticleWhiteSun(){}
    bool init(int count){ return initWithTotalParticles(count); }
    virtual bool initWithTotalParticles(unsigned int numberOfParticles);
    static CCParticleWhiteSun * node()
    {
        return create(350);
    }
    static CCParticleWhiteSun * create(int count)
    {
        CCParticleWhiteSun *pRet = new CCParticleWhiteSun();
        if (pRet->init(count))
        {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
};

NS_CC_END

#endif
