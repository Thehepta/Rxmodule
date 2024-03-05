package com.hepta.dumpdex;

import com.hepta.dumpdex.LoadEntry;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button button = findViewById(R.id.text2);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                LoadEntry.entry(MainActivity.this,"","");
                LoadEntry.dumpdex();
                LoadEntry.dumpMethod();

            }
        });
    }
}