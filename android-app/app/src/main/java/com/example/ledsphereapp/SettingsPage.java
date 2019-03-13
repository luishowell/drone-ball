package com.example.ledsphereapp;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.TextView;

public class SettingsPage extends AppCompatActivity {


    EditText editImHeight;
    EditText editImWidth;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings_page);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        //init global variables
        final GlobalVariables globalVars = (GlobalVariables)getApplication();


        editImWidth = (EditText) findViewById(R.id.editImWidth);
        editImWidth.setText(Integer.toString(globalVars.imageWidth));
        editImWidth.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {

                boolean handled = false;
                if (actionId == EditorInfo.IME_ACTION_NEXT) {
                    //store the value in the relevant int value
                    String image_width_str = v.getText().toString();
                    globalVars.imageWidth = Integer.parseInt(image_width_str);
                }
                return handled;
            }
        });


        editImHeight = (EditText) findViewById(R.id.editImHeight);
        editImHeight.setText(Integer.toString(globalVars.imageHeight));
        editImHeight.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {

                boolean handled = false;
                if (actionId == EditorInfo.IME_ACTION_DONE) {
                    //store the value in the relevant int value
                    String image_height_str = v.getText().toString();
                    globalVars.imageHeight = Integer.parseInt(image_height_str);
                }
                return handled;
            }
        });
    }


}
