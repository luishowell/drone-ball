package com.example.ledsphereapp;

import android.app.Activity;
import android.os.Handler;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class BluetoothHelper {

    public Activity activity;
    public BluetoothHelper(Activity act)
    {
        this.activity = act;
    }

    public boolean readEnded = false;
    public String allData = "";

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(this.activity.getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }


    //send command over bluetooth
    public void sendCommandBT(byte toSend)
    {

        //init global variables
        final GlobalVariables globalVars = (GlobalVariables)this.activity.getApplication();

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

    //send command over bluetooth after some delay
    public void delayedSendBT(final byte toSend, long delayMS)
    {
        //use a handler to do the delay
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                //action to do after the delay
                sendCommandBT(toSend);
            }
        }, delayMS);
    }

    InputStream mmInputStream;
    Thread workerThread;
    byte[] readBuffer;
    int readBufferPosition;
    int counter;
    volatile boolean stopWorker;

    void beginListenForData()
    {

        readEnded = false;

        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character

        //init global variables
        final GlobalVariables globalVars = (GlobalVariables)this.activity.getApplication();

        if (globalVars.btSocket!=null)
        {
            try
            {
                mmInputStream = globalVars.btSocket.getInputStream();
            }
            catch (IOException e)
            {
                msg("Error");
            }
        }else{
            msg("no bluetooth device connected");
        }



        stopWorker = false;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        workerThread = new Thread(new Runnable()
        {
            public void run()
            {
                while(!Thread.currentThread().isInterrupted() && !stopWorker)
                {
                    try
                    {
                        int bytesAvailable = mmInputStream.available();
                        if(bytesAvailable > 0)
                        {
                            byte[] packetBytes = new byte[bytesAvailable];
                            mmInputStream.read(packetBytes);
                            for(int i=0;i<bytesAvailable;i++)
                            {
                                byte b = packetBytes[i];
                                if(b == delimiter)
                                {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "US-ASCII");
                                    readBufferPosition = 0;

                                    handler.post(new Runnable()
                                    {
                                        public void run()
                                        {
                                            msg(data);
                                            allData += data;
                                            readEnded = true;
                                            //stopWorker = true;
                                        }
                                    });
                                }
                                else
                                {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                            //stopWorker = true;
                        }
                    }
                    catch (IOException ex)
                    {
                        msg("stopping worker");
                        stopWorker = true;
                    }
                }
            }
        });

        workerThread.start();
    }

    void endListenForData() throws IOException
    {
        mmInputStream.close();
        readEnded = false;
    }

    void readData()
    {
        //init global variables
        final GlobalVariables globalVars = (GlobalVariables)this.activity.getApplication();

        //check the bluetooth is connected and that
        if (globalVars.btSocket!=null)
        {
            try
            {
                mmInputStream = globalVars.btSocket.getInputStream();
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
