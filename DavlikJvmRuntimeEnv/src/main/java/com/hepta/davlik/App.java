package com.hepta.davlik;

import static com.hepta.davlik.JvmRuntime.nativeLoadJvmTI;

import android.app.Application;

public class App extends Application {

    static {
        System.loadLibrary("jvmRuntime");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        nativeLoadJvmTI();

    }
}
