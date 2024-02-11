package com.hepta.rxmodule;

import android.content.Context;


//import com.hepta.rmi.RmiServer;


public class LoadEntry {

    public static void Entry(Context context , String source){
        LoadSo(context,source);
//        try {
//            new RmiServer(context , source);
//        } catch (IOException e) {
//            throw new RuntimeException(e);
//        }
    }

    private static void LoadSo(Context context, String source) {

//        String abi= "arm64-v8a";
//        if(!android.os.Process.is64Bit()){
//            abi = "armeabi-v7a";
//        }
//        String str = source+"!/lib/"+abi+"/libpine.so";
//        System.load(str);
    }


}