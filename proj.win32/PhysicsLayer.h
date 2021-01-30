#ifndef __MYARKANOIDCLONE_PHYSICSLAYER_H__
#define __MYARKANOIDCLONE_PHYSICSLAYER_H__

#include "cocos2d.h"

class PhysicsLayer : public cocos2d::CCLayer
{

	 private:
        PhysicsWorld* m_world;

    // add following codes
    void setPhyWorld(PhysicsWorld* world){m_world = world;}

}

#endif // __MYARKANOIDCLONE_PHYSICSLAYER_H__