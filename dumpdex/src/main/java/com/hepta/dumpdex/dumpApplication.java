package com.hepta.dumpdex;

import android.app.Application;

public class dumpApplication extends Application {


    static {
        System.loadLibrary("dump");
    }
    @Override
    public void onCreate() {
        super.onCreate();

//        LoadEntry.entry(this);
    }
}
