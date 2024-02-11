package com.hepta.rxmodule;



import android.os.Bundle;
import com.google.android.material.snackbar.Snackbar;
import androidx.appcompat.app.AppCompatActivity;

import android.util.Log;
import android.view.View;


//import com.hepta.andjvmenv.JvmEnv;
import com.hepta.rxmodule.databinding.ActivityMainBinding;

import java.lang.reflect.Method;


public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;
//    private RmiServer rmiServer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

//        Thread con =  new Thread("Connect") {
//            public void run () {
//                try {
//                    new RmiServer(getApplicationContext(),getApplicationInfo().sourceDir);
//                } catch (IOException e) {
//                    throw new RuntimeException(e);
//                }
////                        new ChatRmiClient();
////                    player.registerName("text");
//            }
//        };
//        con.start();
//        try {
//            con.join();
//        } catch (InterruptedException e) {
//            throw new RuntimeException(e);
//        }


        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setSupportActionBar(binding.toolbar);

        binding.fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG).setAnchorView(R.id.fab).setAction("Action", null).show();
                new Thread("Connect") {
                    public void run () {
//                        try {
//                            rmiServer = new RmiServer(getApplicationContext(),getApplicationInfo().sourceDir);
//
//                        } catch (IOException e) {
//                            throw new RuntimeException(e);
//                        }
//                        new ChatRmiClient();

//                    player.registerName("text");
                    }
                }.start();

            }
        });
        binding.hook.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                hooktest();

                Log.e("rzx","hook text");
            }
        });

        binding.loadjvmti.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                JvmEnv.getClassLoaderList();
            }
        });
    }

    public void hooktest(){

        String []args={"java.lang.String","java.lang.String"};
        try {
            Class<?> byte_cls = getClassLoader().loadClass("B");
            Class<?> cls = getClassLoader().loadClass("android.util.Log");
            Class<?>[] types = new Class[args.length];
            for(int i=0;i<args.length;i++){
                Class<?> arg_cls = getClassLoader().loadClass(args[i]);
                types[i] =arg_cls;
            }

            Method method = cls.getMethod("e",types);
            Log.e("rzx",method.getName());
        } catch (ClassNotFoundException | NoSuchMethodException e) {
            throw new RuntimeException(e);
        }




    }

}