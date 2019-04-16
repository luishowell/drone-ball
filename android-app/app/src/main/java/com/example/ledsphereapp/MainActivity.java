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
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;

import static java.lang.Boolean.TRUE;

public class MainActivity extends AppCompatActivity {

    private Boolean switchState;

    private Switch masterSwitch;
    private Button button_draw;
    private Button button_sd;
    private Button button_photo;
    private Button button_settings;

    //Bluetooth helper to send command
    BluetoothHelper btHelper = new BluetoothHelper(this);

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

        //setup the button that takes you to the draw image window
        button_draw = (Button) findViewById(R.id.button_draw);
        button_draw.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToDrawWindow();
            }

        });

        //setup the button that takes you to the sd card page
        button_sd = (Button) findViewById(R.id.button_sd);
        button_sd.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToSDWindow();
            }

        });

        //setup the button that takes you to photo selector page
        button_photo = (Button) findViewById(R.id.button_photo);
        button_photo.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToImageWindow();
            }

        });

        //setup the button that takes you to settings page
        button_settings = (Button) findViewById(R.id.button_settings);
        button_settings.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                goToSettingsWindow();
            }

        });

        //setup the master toggle switch
        masterSwitch = (Switch) findViewById(R.id.MasterControl);
        masterSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                switchEvent();
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

    private void switchEvent() {
        // check current state of a Switch (true or false).
        switchState = masterSwitch.isChecked();

        byte commandByte = 0;

        if(switchState == TRUE) {
            msg("Switch on");
            commandByte = 0x10;
        } else {
            msg("Switch off");
            commandByte = 0x21;
        }
        //send the byte
        btHelper.sendCommandBT(commandByte);
    }

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }


    //send command over bluetooth
    private void sendCommandBT(byte toSend)
    {

        //init global variables
        final GlobalVariables globalVars = (GlobalVariables)getApplication();

        if (globalVars.btSocket!=null)
        {
            try
            {
                OutputStream mmOutputStream = globalVars.btSocket.getOutputStream();
                mmOutputStream.write(toSend);
            }
            catch (IOException e)
            {
                msg("Error");
            }
        }else{
            msg("no bluetooth device connected");
        }
    }
}
