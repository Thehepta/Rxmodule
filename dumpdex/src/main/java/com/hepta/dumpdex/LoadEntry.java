package com.hepta.dumpdex;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexFile;
import top.custom.hepta.Pine;
import top.custom.hepta.callback.MethodHook;

public class LoadEntry {

    static String TAG = LoadEntry.class.getName();

    static boolean dump = false;

    public static native void dumpMethod(Member method);
    public static native ClassLoader[] getClassLoaderList();
    public static native ClassLoader[] getBaseDexClassLoaderList();

    public static native Object[] dumpDexByCookie(long[] cookie,String dumpDir);

    public static void entry(Context context){

        Log.e("Rzx","entry dumpdex");
        try {
            Class<?> MTGuard = context.getClassLoader().loadClass("android.app.Activity");
            Pine.hook(MTGuard.getDeclaredMethod("onCreate", Bundle.class), new MethodHook() {
                @Override
                public void beforeCall(Pine.CallFrame callFrame) throws Throwable {
                    Log.e(TAG,"onCreate beforeCall:"+callFrame.method.getName());

//                    dumpMethod(callFrame.method);
                }
                @Override
                public void afterCall(Pine.CallFrame callFrame) throws Throwable {

                    Log.e(TAG,"onCreate afterCall");

                }
            });
        } catch (NoSuchMethodException | ClassNotFoundException e) {
            throw new RuntimeException(e);
        }

//        dumpMethod();
    }

    public static void dumpdex(Context context , String dir) {


        File pathFile=new File(context.getFilesDir().getAbsolutePath()+"/dump");
        if(!pathFile.exists()){
            pathFile.mkdirs();
        }
        BaseDexClassLoader[] classLoaders = (BaseDexClassLoader[]) getBaseDexClassLoaderList();
        try {
            Log.e(TAG,"class xunhuan");
            //TODO:to get 'pathList' field and 'dexElements' field by reflection.
            //private final DexPathList pathList;
            Class<?> baseDexClassLoaderClass = Class.forName("dalvik.system.BaseDexClassLoader");
            Field pathListField = baseDexClassLoaderClass.getDeclaredField("pathList");

            //private Element[] dexElements;
            Class<?> dexPathListClass = Class.forName("dalvik.system.DexPathList");
            Class<?> Element = Class.forName("dalvik.system.DexPathList$Element");
            Field dexElementsField = dexPathListClass.getDeclaredField("dexElements");
            Field DexFile_mCookie = DexFile.class.getDeclaredField("mCookie");
            Field DexFile_mFileName = DexFile.class.getDeclaredField("mFileName");
            Field path_filed = Element.getDeclaredField("path");
            Field dexFile_filed = Element.getDeclaredField("dexFile");
            pathListField.setAccessible(true);
            DexFile_mCookie.setAccessible(true);
            DexFile_mFileName.setAccessible(true);
            dexElementsField.setAccessible(true);
            dexFile_filed.setAccessible(true);
            for (ClassLoader classLoader:classLoaders) {

                if (classLoader instanceof BaseDexClassLoader) {
                    Object BaseDexClassLoad_PathList = pathListField.get(classLoader);
                    Object[] DexPathList_dexElements = (Object[]) dexElementsField.get(BaseDexClassLoad_PathList);
                    int i = 0;
                    if (DexPathList_dexElements != null) {
                        for (Object dexElement : DexPathList_dexElements) {
                            DexFile dexFile = (DexFile) dexFile_filed.get(dexElement);
                            if (dexFile != null) {
                                //这个cookie 在android 13是一个智能指针，保存的是一个native 的 DexFile 指针
                                long[] cookie = (long[]) DexFile_mCookie.get(dexFile);
                                dumpDexByCookie(cookie, pathFile.getAbsolutePath());
                            }

                        }
                    }
                } else {
                    Log.e(TAG, "class not instanceof BaseDexClassLoader");
                }

            }
        } catch (Exception e) {
            e.printStackTrace();
        }


    }

    public static void dumpMethod(){
        ClassLoader[] classLoaders =  getClassLoaderList();
        for (ClassLoader classLoader:classLoaders) {
            try {
                Class LoadEntry_cls =  classLoader.loadClass("com.hepta.dumpdex.LoadEntry");
                Method Entry_mtd =  LoadEntry_cls.getDeclaredMethod("dumpdex",Context.class,String.class);
                dumpMethod(Entry_mtd);

            } catch (ClassNotFoundException e) {
                continue;
            } catch (NoSuchMethodException e) {
                throw new RuntimeException(e);
            }

        }
    }



}
