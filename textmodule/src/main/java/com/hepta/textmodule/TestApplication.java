package com.hepta.textmodule;

import android.app.Application;
import android.content.Context;
import android.util.Log;

//import top.canyie.pine.Pine;
//import top.canyie.pine.callback.MethodHook;

import top.custom.hepta.Pine;
import top.custom.hepta.callback.MethodHook;

public class TestApplication extends Application {


    static {
        System.loadLibrary("pine");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        hookAppText(this);
    }


    private static void hookAppText(Context context) {
        try {
            Class<?> MTGuard = context.getClassLoader().loadClass("com.hepta.textmodule.MainActivity");
            Pine.hook(MTGuard.getDeclaredMethod("getTextHook"), new MethodHook() {
                @Override
                public void beforeCall(Pine.CallFrame callFrame) throws Throwable {

                }
                @Override
                public void afterCall(Pine.CallFrame callFrame) throws Throwable {
                    String result = (String) callFrame.getResult();
//                    String libnative = (String) callFrame.args[1];
                    Log.e("LoadEntry","result:"+result);
                    callFrame.setResult("rxposed text successful");
                }
            });
        } catch (NoSuchMethodException | ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }


}
