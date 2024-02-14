package com.hepta.dumpdex;

import static com.hepta.dumpdex.LoadEntry.dumpdex;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        dumpdex(this,null);
    }
}