//
//  ParticleWhiteSun.cpp
//  FruitConnect
//
//  Created by 柳亚鑫 on 12-11-3.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ParticleWhiteSun.h"

NS_CC_BEGIN

bool CCParticleWhiteSun::initWithTotalParticles(unsigned int numberOfParticles)
{
    if( CCParticleSystemQuad::initWithTotalParticles(numberOfParticles) )
    {
        // additive
        this->setBlendAdditive(true);
        
        // duration
        m_fDuration = kCCParticleDurationInfinity;
        
        // Gravity Mode
        m_nEmitterMode = kCCParticleModeGravity;
        
        // Gravity Mode: gravity
        modeA.gravity = ccp(0,0);
        
        // Gravity mode: radial acceleration
        modeA.radialAccel = 0;
        modeA.radialAccelVar = 0;
        
        // Gravity mode: speed of particles
        modeA.speed = 20;
        modeA.speedVar = 5;
        
        
        // angle
        m_fAngle = 90;
        m_fAngleVar = 360;
        
        // emitter position
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        this->setPosition(ccp(winSize.width/2, winSize.height/2));
        m_tPosVar = CCPointZero;
        
        // life of particles
        m_fLife = 1;
        m_fLifeVar = 0.5f;
        
        // size, in pixels
        m_fStartSize = 30.0f;
        m_fStartSizeVar = 10.0f;
        m_fEndSize = kCCParticleStartSizeEqualToEndSize;
        
        // emits per seconds
        m_fEmissionRate = m_uTotalParticles/m_fLife;
        
        // color of particles
        m_tStartColor.r = 0.89f;
        m_tStartColor.g = 0.56f;
        m_tStartColor.b = 0.36f;
        m_tStartColor.a = 1.0f;
        m_tStartColorVar.r = 0.0f;
        m_tStartColorVar.g = 0.0f;
        m_tStartColorVar.b = 0.0f;
        m_tStartColorVar.a = 0.0f;
        m_tEndColor.r = 0.0f;
        m_tEndColor.g = 0.0f;
        m_tEndColor.b = 0.0f;
        m_tEndColor.a = 0.84f;
        m_tEndColorVar.r = 0.0f;
        m_tEndColorVar.g = 0.0f;
        m_tEndColorVar.b = 0.0f;
        m_tEndColorVar.a = 0.0f;
        
        return true;
    }
    return false;
}

NS_CC_END