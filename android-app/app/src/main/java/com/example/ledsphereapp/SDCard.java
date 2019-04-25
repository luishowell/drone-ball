package com.example.ledsphereapp;

import android.os.Bundle;
import android.os.Handler;
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

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

public class SDCard extends AppCompatActivity {

    private static TextView txtContent;
    private Spinner selectFileSpinner;

    private static String fileList;

    //Bluetooth helper to send command
    BluetoothHelper btHelper = new BluetoothHelper(this);
    MyHandler btIn;
    public static final int handlerState = 0;
    private static StringBuilder recDataString = new StringBuilder();

    private Button readSDCardListButton;
    private Button updateSpinnerBtn;
    private Button submitSDSelectionButton;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sdcard);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);


        //setup the text view and spinner
        txtContent = (TextView) findViewById(R.id.SDCardList);
        selectFileSpinner = (Spinner) findViewById(R.id.selectFileSpinner);

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

        //button to update the spinner
        Button updateSpinnerBtn = (Button) findViewById(R.id.updateSpinBtn);
        updateSpinnerBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                addItemsToSpinner();
            }
        });


        btIn = new MyHandler(this);


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

    private static class MyHandler extends Handler  {
        private final WeakReference<SDCard> mActivity;

        public MyHandler(SDCard activity)  {
            mActivity = new WeakReference<SDCard>(activity);
        }

        @Override
        public void handleMessage(android.os.Message msg) {
            if (msg.what == handlerState) {										//if message is what we want
                String readMessage = (String) msg.obj;                                                                // msg.arg1 = bytes from connect thread
                recDataString.append(readMessage);      								//keep appending to string until ~
                int endOfLineIndex = recDataString.indexOf("~");                    // determine the end-of-line
                if (endOfLineIndex > 0) {                                           // make sure there data before ~
                    String dataInPrint = recDataString.substring(0, endOfLineIndex);    // extract string
                    int dataLength = dataInPrint.length();							//get length of data received

                    if (recDataString.charAt(0) == '!')								//if it starts with # we know it is what we are looking for
                    {
                        fileList = recDataString.substring(1, (endOfLineIndex-1));             //get sensor value from string between indices 1-5
                        txtContent.setText(fileList);	//update the textviews with sensor values
                        //addItemsToSpinner(sensor0);
                    }
                    recDataString.delete(0, recDataString.length()); 					//clear all string data
                }
            }
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        btHelper.resumeListening(btIn);

    }

    @Override
    public void onPause()
    {
        super.onPause();
        btHelper.stopListening();

    }

    // add items into spinner dynamically
    public void addItemsToSpinner() {

        List<String> list = new ArrayList<String>();

        //split the file by delimiter and load to the spinner
        if(fileList == null){
            msg("Error: File not found");
            return;
        }
        String [] arrOfStr = fileList.split("\n");
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


        String sel = String.valueOf(selectFileSpinner.getSelectedItem());
        //Print the selection
        msg("OnClickListener : " + "\nSpinner selection : "+ sel);

        //send the command byte
        btHelper.sendCommandBT((byte)0x43);

        //parse and send selected file after 100ms
        String selNum =  sel.split("\t")[0];
        int toSend = Integer.parseInt(selNum);
        btHelper.delayedSendBT((byte)toSend, 100);

        //msg(btHelper.allData);
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
        //btHelper.beginListenForData();



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
        //txtContent.setText(FileHelper.ReadFile(SDCard.this));
        //addItemsToSpinner();
    }

}
