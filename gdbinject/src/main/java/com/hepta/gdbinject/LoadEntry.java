package com.hepta.gdbinject;

import android.content.Context;
import android.util.Log;

public class LoadEntry {


    public static void Entry(Context context , String source){
        PreLoadNativeSO(context,source);
    }


    private static void PreLoadNativeSO(Context context, String source) {
        try {
            String abi= "arm64-v8a";
            if(!android.os.Process.is64Bit()){
                abi = "armeabi-v7a";
            }
            String str = source+"!/lib/"+abi+"/libentry.so";
            System.load(str);
        }catch (Exception e){
            Log.e("LoadEntry","LoadSo error");
        }

    }


}
