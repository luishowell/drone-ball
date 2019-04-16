package com.example.ledsphereapp;

import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;

import java.io.OutputStream;
import java.util.Set;
import java.util.ArrayList;
import android.widget.Toast;
import android.widget.ArrayAdapter;
import android.widget.AdapterView;
import android.widget.TextView;
import android.content.Intent;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;
import java.io.IOException;
import java.util.UUID;
import android.app.ProgressDialog;
import android.widget.ListView;

public class SettingsPage extends AppCompatActivity {


    EditText editImHeight;
    EditText editImWidth;

    //Bluetooth helper to send command
    BluetoothHelper btHelper = new BluetoothHelper(this);

    Button btnPaired;
    Button btnSendTest;
    ListView DeviceList;

    private BluetoothAdapter myBluetooth = null;
    private Set<BluetoothDevice> pairedDevices;
    String address = null;

    private ProgressDialog progress;
    //BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

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


        //Bluetooth test stuff
        btnPaired = (Button)findViewById(R.id.btPaired);
        btnSendTest = (Button)findViewById(R.id.btnBluetoothSendTest);
        DeviceList = (ListView) findViewById(R.id.deviceList);

        //setup the bluetooth
        myBluetooth = BluetoothAdapter.getDefaultAdapter();
        if(myBluetooth == null)
        {
            //Show a mensag. that thedevice has no bluetooth adapter
            Toast.makeText(getApplicationContext(), "Bluetooth Device Not Available", Toast.LENGTH_LONG).show();
            //finish apk
            finish();
        }
        else
        {
            if (myBluetooth.isEnabled())
            { }
            else
            {
                //Ask to the user turn the bluetooth on
                Intent turnBTon = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(turnBTon,1);
            }
        }

        //create a listener on the paired devices list
        btnPaired.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                pairedDevicesList();
            }
        });


        //create a listener on send button
        btnSendTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                byte toSend = (byte)0xFF;
                btHelper.sendCommandBT(toSend);
            }
        });
    }


    private void pairedDevicesList()
    {
        pairedDevices = myBluetooth.getBondedDevices();
        ArrayList list = new ArrayList();

        Toast.makeText(getApplicationContext(), "pairedDeviceButtonPressed.", Toast.LENGTH_LONG).show();
        if (pairedDevices.size()>0)
        {
            Toast.makeText(getApplicationContext(), "Paired Bluetooth Devices Found.", Toast.LENGTH_LONG).show();
            for(BluetoothDevice bt : pairedDevices)
            {
                list.add(bt.getName() + "\n" + bt.getAddress()); //Get the device's name and the address
            }
        }
        else
        {
            Toast.makeText(getApplicationContext(), "No Paired Bluetooth Devices Found.", Toast.LENGTH_LONG).show();
        }

        final ArrayAdapter adapter = new ArrayAdapter(this,android.R.layout.simple_list_item_1, list);
        DeviceList.setAdapter(adapter);
        DeviceList.setOnItemClickListener(myListClickListener); //Method called when the device from the list is clicked

    }


    private AdapterView.OnItemClickListener myListClickListener = new AdapterView.OnItemClickListener()
    {
        public void onItemClick (AdapterView av, View v, int arg2, long arg3)
        {
            // Get the device MAC address, the last 17 chars in the View
            String info = ((TextView) v).getText().toString();
            address = info.substring(info.length() - 17);
            new ConnectBT().execute(); //Call the class to connect
        }
    };


    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }

    private class ConnectBT extends AsyncTask<Void, Void, Void>  // UI thread
    {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute()
        {
            progress = ProgressDialog.show(SettingsPage.this, "Connecting...", "Please wait!!!");  //show a progress dialog
        }

        @Override
        protected Void doInBackground(Void... devices) //while the progress dialog is shown, the connection is done in background
        {
            //init global variables
            final GlobalVariables globalVars = (GlobalVariables)getApplication();

            try
            {

                if (globalVars.btSocket == null || !isBtConnected)
                {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                    globalVars.btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    globalVars.btSocket.connect();//start connection
                }
            }
            catch (IOException e)
            {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }
        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);
            //init global variables
            final GlobalVariables globalVars = (GlobalVariables)getApplication();

            if (!ConnectSuccess)
            {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            }
            else
            {
                msg("Connected.");
                isBtConnected = true;
            }
            progress.dismiss();
        }
    }


}
