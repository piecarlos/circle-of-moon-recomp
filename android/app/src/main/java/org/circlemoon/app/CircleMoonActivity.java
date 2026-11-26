package org.circlemoon.app;

import android.app.UiModeManager;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

public class CircleMoonActivity extends SDLActivity {
    private PowerManager.WakeLock screenWakeLock;

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "main"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        applyPhoneOrientationPolicy();
        keepScreenAwakeAndFocused();
        acquireScreenWakeLock();
    }

    @Override
    protected void onResume() {
        super.onResume();
        keepScreenAwakeAndFocused();
        acquireScreenWakeLock();
    }

    @Override
    protected void onPause() {
        releaseScreenWakeLock();
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        releaseScreenWakeLock();
        super.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            keepScreenAwakeAndFocused();
            acquireScreenWakeLock();
        }
    }

    private void keepScreenAwakeAndFocused() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (mSurface != null) {
            mSurface.setKeepScreenOn(true);
            mSurface.setFocusable(true);
            mSurface.setFocusableInTouchMode(true);
            mSurface.requestFocus();
        }
        enableImmersiveFullscreen();
    }

    @SuppressWarnings("deprecation")
    private void acquireScreenWakeLock() {
        if (screenWakeLock == null) {
            PowerManager pm = (PowerManager) getApplicationContext().getSystemService(Context.POWER_SERVICE);
            if (pm != null) {
                screenWakeLock = pm.newWakeLock(
                    PowerManager.SCREEN_DIM_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE,
                    "CircleMoon:KeepScreenOn"
                );
                screenWakeLock.setReferenceCounted(false);
            }
        }
        if (screenWakeLock != null && !screenWakeLock.isHeld()) {
            screenWakeLock.acquire();
        }
    }

    private void releaseScreenWakeLock() {
        if (screenWakeLock != null && screenWakeLock.isHeld()) {
            screenWakeLock.release();
        }
    }

    private boolean isDesktopOrMultiWindow() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N && isInMultiWindowMode()) {
            return true;
        }
        UiModeManager uiMode = (UiModeManager) getSystemService(Context.UI_MODE_SERVICE);
        return uiMode != null && uiMode.getCurrentModeType() == Configuration.UI_MODE_TYPE_DESK;
    }

    private void applyPhoneOrientationPolicy() {
        if (!isDesktopOrMultiWindow()) {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        }
    }

    @SuppressWarnings("deprecation")
    private void enableImmersiveFullscreen() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            getWindow().setDecorFitsSystemWindows(false);
            WindowInsetsController controller = getWindow().getInsetsController();
            if (controller != null) {
                controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                controller.setSystemBarsBehavior(
                    WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
                );
            }
        } else {
            getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
            );
        }
    }
}
