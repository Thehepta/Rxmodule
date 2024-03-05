package com.hepta.textmodule;

import android.content.Context;
import android.util.Log;

//import top.canyie.pine.Pine;
//import top.canyie.pine.callback.MethodHook;
import top.custom.hepta.Pine;
import top.custom.hepta.callback.MethodHook;

public class LoadEntry {


    public static void Entry(Context context , String source,String argument){
        PreLoadNativeSO(context,source);
        hookAppText(context);
    }

    private static void hookAppText(Context context) {
        Log.e("Rzx","hookAppText entry");
        try {
            Class<?> MTGuard = context.getClassLoader().loadClass("com.hepta.textapp.MainActivity");
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

    private static void PreLoadNativeSO(Context context, String source) {
        try {
            String abi= "arm64-v8a";
            if(!android.os.Process.is64Bit()){
                abi = "armeabi-v7a";
            }
            String str = source+"!/lib/"+abi+"/libpine.so";
            System.load(str);
        }catch (Exception e){
            Log.e("LoadEntry","LoadSo error");
        }

    }
}
