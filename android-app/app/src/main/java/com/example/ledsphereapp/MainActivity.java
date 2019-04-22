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
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.Button;
import android.widget.TextView;
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

    //rpm stuff
    private SeekBar rpmSeek;
    private Button rpmButton;
    private TextView rpmView;


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

        final GlobalVariables globalVars = (GlobalVariables)getApplication();

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


        //rpm stuff
        rpmSeek = (SeekBar) findViewById(R.id.rpmSeekBar);
        rpmButton = (Button) findViewById(R.id.rpmSendButton);
        rpmView = (TextView) findViewById(R.id.rpmView);

        //set defaults
        rpmSeek.setProgress(globalVars.hoopRPM);
        int rpm = (globalVars.hoopRPM * globalVars.maxRPM)/ 100;
        rpmView.setText("" + rpm + " RPM");

        //listener for progress bar changes
        rpmSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int rpm = (progress * globalVars.maxRPM)/ 100;
                rpmView.setText("" + rpm + " RPM");
                globalVars.hoopRPM = progress; //percentage of max
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        //listener for send rpm button
        rpmButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendRPM();
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

    private void sendRPM()  {

        //send the command byte
        byte commandByte = 0x65;
        btHelper.sendCommandBT(commandByte);

        //get the rpm values
        final GlobalVariables globalVars = (GlobalVariables)getApplication();

        //send the rpm value after 100ms
        btHelper.delayedSendBT((byte)globalVars.hoopRPM, 100);
    }

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }



}
