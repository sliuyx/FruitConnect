#include "AppDelegate.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#include "HelloWorldScene.h"
#include "Common.h"
#include "CommonFunctions.h"
#include "ShopLayer.h"

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

extern "C"
{

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JniHelper::setJavaVM(vm);

    return JNI_VERSION_1_4;
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    if (!CCDirector::sharedDirector()->getOpenGLView())
    {
        CCEGLView *view = CCEGLView::sharedOpenGLView();
        view->setFrameSize(w, h);

        AppDelegate *pAppDelegate = new AppDelegate();
        CCApplication::sharedApplication()->run();
    }
    else
    {
        ccDrawInit();
        ccGLInvalidateStateCache();
        
        CCShaderCache::sharedShaderCache()->reloadDefaultShaders();
        CCTextureCache::reloadAllTextures();
        CCNotificationCenter::sharedNotificationCenter()->postNotification(EVNET_COME_TO_FOREGROUND, NULL);
        CCDirector::sharedDirector()->setGLDefaultValues(); 
    }
}

void refreshOwnedItems()
{
  CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
  if (scene != NULL && scene->getTag() == SCENE_SHOP) {
      ShopLayer* shopLayer = (ShopLayer*) scene->getChildByTag(LAYER_SHOP);
      if (shopLayer != NULL) {
          shopLayer->refreshOwnedItems();
      }
  }
}

void Java_com_xxstudio_llk_Cocos2dHelper_updatePoint(JNIEnv* env, jobject thiz, jint pointTotal)
{
  saveGoldCount(pointTotal);
  FLUSH_DATA;
  refreshOwnedItems();
}

void Java_com_xxstudio_llk_Cocos2dHelper_spendPointSuccess(JNIEnv* env, jobject thiz, jint itemType, jint itemCount, jint pointTotal)
{
  saveGoldCount(pointTotal);
  if (itemType == TIP) {
      saveTipItemCount(getTipItemCount()+itemCount);
  } else if (itemType == RERANGE) {
      saveRerangeItemCount(getRerangeItemCount()+itemCount);
  } else if (itemType == TIME) {
      saveTimeItemCount(getTimeItemCount()+itemCount);
  } else if (itemType == MIXED) {
      saveTipItemCount(getTipItemCount()+itemCount);
      saveRerangeItemCount(getRerangeItemCount()+itemCount);
      saveTimeItemCount(getTimeItemCount()+itemCount);
  }
  FLUSH_DATA;
  refreshOwnedItems();
}

}
