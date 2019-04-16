package com.example.ledsphereapp;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class SDCard extends AppCompatActivity {

    private TextView txtContent;
    private Spinner selectFileSpinner;

    //Bluetooth helper to send command
    BluetoothHelper btHelper = new BluetoothHelper(this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sdcard);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);


        //setup the text view
        txtContent = (TextView) findViewById(R.id.SDCardList);

        //setup the button to read the SDCardList
        Button readSDCardListButton = (Button) findViewById(R.id.buttonReadSDCard);
        readSDCardListButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getSDCardList();
            }

        });

        //setup the button to submit choice
        Button submitSDSelectionButton = (Button) findViewById(R.id.submitSDSelectionButton);
        submitSDSelectionButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                submitSelection();
            }

        });




        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }

    // add items into spinner dynamically
    public void addItemsToSpinner() {

        selectFileSpinner = (Spinner) findViewById(R.id.selectFileSpinner);
        List<String> list = new ArrayList<String>();

        //split the file by delimiter and load to the spinner
        String str = FileHelper.ReadFile(SDCard.this);
        if(str == null){
            msg("Error: File not found");
            return;
        }
        String [] arrOfStr = str.split("\n");
        for(String a:arrOfStr) {
            list.add(a);
        }

        ArrayAdapter<String> dataAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, list);
        dataAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        selectFileSpinner.setAdapter(dataAdapter);
    }

    //submit the selection to the sphere
    public void submitSelection() {

        /*
        String sel = String.valueOf(selectFileSpinner.getSelectedItem());
        //Print the selection
        msg("OnClickListener : " + "\nSpinner selection : "+ sel);

        //send the command byte
        btHelper.sendCommandBT((byte)0x43);

        //parse and send selected file
        String selNum =  sel.split("\t")[0];
        int toSend = Integer.parseInt(selNum);
        btHelper.sendCommandBT((byte)toSend);
        */
        msg(btHelper.allData);
    }

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }

    //get the file list from the SD card
    private void getSDCardList()
    {
        //request file list from micro
        btHelper.sendCommandBT((byte)0x32);

        //wait for the file back
        btHelper.beginListenForData();



        //stop listening for data
        //try
        //{
        //    btHelper.endListenForData();
        //}
        //catch (IOException ex)
        //{
        //    msg("error closing input stream");
        //}

        //read the file and display on spinner
        txtContent.setText(FileHelper.ReadFile(SDCard.this));
        addItemsToSpinner();
    }

}
