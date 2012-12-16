/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package com.xxstudio.llk;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxEditText;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxRenderer;

import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.kuguo.banner.view.BannerNormalView;
import com.kuguo.kuzai.KuzaiAdsManager;
import com.umeng.analytics.MobclickAgent;
import com.umeng.fb.NotificationType;
import com.umeng.fb.UMFeedbackService;
import com.umeng.update.UmengUpdateAgent;
import com.waps.AdView;
import com.waps.AppConnect;
import com.waps.MiniAdView;
import com.waps.UpdatePointsNotifier;

public class FruitConnect extends Cocos2dxActivity implements UpdatePointsNotifier{

    ProgressDialog progress;
    Handler handler = new Handler();
    
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		Cocos2dHelper.setContext(this);
		MobclickAgent.onError(this);
		MobclickAgent.updateOnlineConfig(this);
		UMFeedbackService.enableNewReplyNotification(this, NotificationType.AlertDialog);
		UmengUpdateAgent.update(this);
		KuzaiAdsManager.getInstance().setDefaultAppType(this, KuzaiAdsManager.GAMETYPE);
		KuzaiAdsManager.getInstance().showKuguoSprite(this, 0);
		AppConnect.getInstance(this);
		if (detectOpenGLES20()) {
			// get the packageName,it's used to set the resource path
			String packageName = getApplication().getPackageName();
			super.setPackageName(packageName);
			
            // FrameLayout
            ViewGroup.LayoutParams framelayout_params =
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT,
                                           ViewGroup.LayoutParams.FILL_PARENT);
            FrameLayout framelayout = new FrameLayout(this);
            framelayout.setLayoutParams(framelayout_params);

            // Cocos2dxEditText layout
            ViewGroup.LayoutParams edittext_layout_params =
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT,
                                           ViewGroup.LayoutParams.WRAP_CONTENT);
            Cocos2dxEditText edittext = new Cocos2dxEditText(this);
            edittext.setLayoutParams(edittext_layout_params);

            // ...add to FrameLayout
            framelayout.addView(edittext);

            // Cocos2dxGLSurfaceView
	        mGLView = new Cocos2dxGLSurfaceView(this);

            // ...add to FrameLayout
            framelayout.addView(mGLView);

	        mGLView.setEGLContextClientVersion(2);
	        mGLView.setCocos2dxRenderer(new Cocos2dxRenderer());
            mGLView.setTextField(edittext);

            LinearLayout layout = new LinearLayout(this);
            layout.setOrientation(LinearLayout.VERTICAL);
            BannerNormalView banner = new BannerNormalView(this);
            layout.addView(banner);
            framelayout.addView(layout);
            
            LinearLayout wapLayout = new LinearLayout(this);
            wapLayout.setOrientation(LinearLayout.VERTICAL);
            wapLayout.setGravity(Gravity.BOTTOM);
            framelayout.addView(wapLayout);
            
//            LinearLayout miniLayout = new LinearLayout(this);
//            miniLayout.setOrientation(LinearLayout.VERTICAL);
//            miniLayout.setGravity(Gravity.CENTER);
//            framelayout.addView(miniLayout);
            // Set framelayout as the content view
			setContentView(framelayout);
			new AdView(this, wapLayout).DisplayAd();
//			new MiniAdView(this, miniLayout).DisplayAd();
		}
		else {
			Log.d("activity", "don't support gles2.0");
			finish();
		}	
	}
	
	public void showLoadingDialog() {
	    handler.post(new Runnable() {
            @Override
            public void run() {
                if (progress == null || !progress.isShowing()) {
                    progress = ProgressDialog.show(FruitConnect.this, "", getString(R.string.dialog_submitting));
                }
            }
        });
	}
	
	public void hideLoadingDialog() {
	    handler.post(new Runnable() {
            @Override
            public void run() {
                if (progress != null && progress.isShowing()) {
                    progress.dismiss();
                }
            }
        });
	}
	
	public void showToast(final String str) {
	    handler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(FruitConnect.this, str, Toast.LENGTH_SHORT).show();
            }
        });
	}
	
	public void showMenu() {
	    handler.post(new Runnable() {
            @Override
            public void run() {
                openOptionsMenu();
            }
        });
	}
	
	 @Override
	 protected void onPause() {
	     super.onPause();
	     mGLView.onPause();
	     MobclickAgent.onPause(this);
	 }
	 
	 @Override
	 protected void onDestroy() {
	     super.onDestroy();
	     AppConnect.getInstance(this).finalize();
	 }

	 @Override
	 protected void onResume() {
	     super.onResume();
	     mGLView.onResume();
	     MobclickAgent.onResume(this);
	     AppConnect.getInstance(this).getPoints(this);
	 }
	 
	 @Override
	 public boolean onCreateOptionsMenu(Menu menu) {
	     MenuInflater inflater = getMenuInflater();
	     inflater.inflate(R.menu.main_menu, menu);
	     return true;
	 }
	 
	 @Override
	 public boolean onOptionsItemSelected(MenuItem item) {
	     if (item.getItemId() == R.id.menu_feedback) {
	         UMFeedbackService.openUmengFeedbackSDK(this);
	     }
	     return true;
	 }
	 
	 private boolean detectOpenGLES20() 
	 {
	     ActivityManager am =
	            (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
	     ConfigurationInfo info = am.getDeviceConfigurationInfo();
	     return (info.reqGlEsVersion >= 0x20000);
	 }
	
     static {
         System.loadLibrary("game");
     }

    @Override
    public void getUpdatePoints(String arg0, int arg1) {
        Cocos2dHelper.updatePoint(arg1);
    }

    @Override
    public void getUpdatePointsFailed(String arg0) {
    }
}
