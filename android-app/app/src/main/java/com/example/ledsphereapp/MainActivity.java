package com.example.ledsphereapp;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.content.Intent;
import android.view.View;
import android.widget.TextView;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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

        GlobalVariables globalVars = (GlobalVariables)getApplication();

        Button button_draw = (Button) findViewById(R.id.button_draw);
        button_draw.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToDrawWindow();
            }

        });

        Button button_sd = (Button) findViewById(R.id.button_sd);
        button_sd.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToSDWindow();
            }

        });

        Button button_photo = (Button) findViewById(R.id.button_photo);
        button_photo.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToImageWindow();
            }

        });

        Button button_settings = (Button) findViewById(R.id.button_settings);
        button_settings.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToSettingsWindow();
            }

        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void goToDrawWindow() {
        Intent intent = new Intent(this, DrawPage.class);
        startActivity(intent);
    }

    private void goToSDWindow() {
        Intent intent = new Intent(this, SDCard.class);
        startActivity(intent);
    }

    private void goToImageWindow() {
        Intent intent = new Intent(this, ImageSelector.class);
        startActivity(intent);
    }

    private void goToSettingsWindow() {
        Intent intent = new Intent(this, SettingsPage.class);
        startActivity(intent);
    }
}
