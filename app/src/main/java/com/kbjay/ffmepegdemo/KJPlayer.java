package com.kbjay.ffmepegdemo;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class KJPlayer implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("kjplayer-lib");
    }

    SurfaceHolder surfaceHolder;

    public KJPlayer(SurfaceView surfaceView) {
        if (surfaceHolder != null) {
            this.surfaceHolder.removeCallback(this);
        }

        this.surfaceHolder = surfaceView.getHolder();
        this.surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        this.surfaceHolder = holder;
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public void play(String path) {
        nativePlayVideo(path, surfaceHolder.getSurface());
    }

    public native void nativePlayVideo(String path, Surface surface);
}
