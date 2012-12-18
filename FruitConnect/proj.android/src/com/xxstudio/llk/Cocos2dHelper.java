package com.xxstudio.llk;

import com.waps.AppConnect;
import com.waps.UpdatePointsNotifier;
import com.xxstudio.fruitconnect.FruitConnect;

public class Cocos2dHelper {

    private static FruitConnect context;
    
    public static void setContext(FruitConnect ctx) {
        context = ctx;
    }
    
    public static void showOffers() {
        if (context != null) {
            AppConnect.getInstance(context).showOffers(context);
        }
    }
    
    public static void showMore() {
        if (context != null) {
            AppConnect.getInstance(context).showMore(context);
        }
    }
    
    public static void showMenu() {
        if (context != null) {
            context.showMenu();
        }
    }
    
    public static void showMenuSceneAd() {
        if (context != null) {
            context.showTopAdView();
            context.hideBottomAdView();
            context.hideBottomMiniView();
        }
    }
    
    public static void showLevelSceneAd() {
        if (context != null) {
            context.showBottomAdView();
            context.hideTopAdView();
            context.hideBottomMiniView();
        }
    }
    
    public static void showGameSceneAd() {
        if (context != null) {
            context.showBottomMiniView();
            context.hideTopAdView();
            context.hideBottomAdView();
        }
    }
    
    public static void showGamePauseAd() {
        if (context != null) {
            context.showBottomAdView();
            context.hideTopAdView();
            context.hideBottomMiniView();
        }
    }
    
    public static void showShopSceneAd() {
        if (context != null) {
            context.hideTopAdView();
            context.hideBottomAdView();
            context.hideBottomMiniView();
        }
    }
    
    public static void spendPoints(final int itemType, final int itemCount, int point) {
        if (context != null) {
            AppConnect.getInstance(context).spendPoints(point,
                    new SpendPointNotifier(itemType, itemCount));
            context.showLoadingDialog();
        }
    }
    
    public static native void updatePoint(int pointTotal);
    
    public static native void spendPointSuccess(int itemType, int itemCount, int pointTotal);
    
    private static class SpendPointNotifier implements UpdatePointsNotifier {
        
        private int itemType;
        private int itemCount;
        
        public SpendPointNotifier(int itemType, int itemCount) {
            this.itemType = itemType;
            this.itemCount = itemCount;
        }

        @Override
        public void getUpdatePoints(String arg0, int arg1) {
            context.hideLoadingDialog();
            spendPointSuccess(itemType, itemCount, arg1);
        }

        @Override
        public void getUpdatePointsFailed(String arg0) {
            context.hideLoadingDialog();
            context.showToast(arg0);
        }
        
    }
    
}
