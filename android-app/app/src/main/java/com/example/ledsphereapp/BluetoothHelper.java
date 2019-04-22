package com.example.ledsphereapp;

import android.app.Activity;
import android.bluetooth.BluetoothSocket;
import android.graphics.Bitmap;
import android.os.Handler;
import android.widget.TextView;
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

    Handler btIn;
    final int handlerState = 0;
    private StringBuilder recDataString = new StringBuilder();
    private ConnectedThread mConnectedThread;


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

    //send a bmp over bluetooth
    public void sendBmpFile(/*Bitmap bmp,*/ final String filename)
    {
        //send command char
        sendCommandBT((byte)0x54);

        //send the filename
        //use a handler to delay it
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                //action to do after the delay
                byte[] b = filename.getBytes();
                for(int i = 0; i < b.length; i++) {
                    sendCommandBT(b[i]);
                }
            }
        }, 100);





    }

    //resume listening when page becomes active again
    public void resumeListening(Handler connectedHandler) {

        btIn = connectedHandler;

        final GlobalVariables globalVars = (GlobalVariables) this.activity.getApplication();

        if(globalVars.btSocket != null) {
            mConnectedThread = new ConnectedThread(globalVars.btSocket);
            mConnectedThread.start();

        }
        else
        {
            msg("Connect bluetooth device for Vbatt");
        }
    }

    //stop listening when leaving an activity
    public void stopListening()  {
        if(mConnectedThread != null) {
            mConnectedThread.running = false;
        }
    }

    //create new class for connect thread
    private class ConnectedThread extends Thread {
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        volatile boolean running = true;

        //creation of the connect thread
        public ConnectedThread(BluetoothSocket socket) {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                //Create I/O streams for connection
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }


        public void run() {
            byte[] buffer = new byte[256];
            int bytes;

            // Keep looping to listen for received messages
            while (true) {
                try {
                    bytes = mmInStream.read(buffer);            //read bytes from input buffer
                    String readMessage = new String(buffer, 0, bytes);
                    // Send the obtained bytes to the UI Activity via handler
                    btIn.obtainMessage(handlerState, bytes, -1, readMessage).sendToTarget();
                } catch (IOException e) {
                    break;
                }
                if(!running) return;
            }
        }

        //write method
        public void write(String input) {
            byte[] msgBuffer = input.getBytes();           //converts entered String into bytes
            try {
                mmOutStream.write(msgBuffer);                //write bytes over BT connection via outstream
            } catch (IOException e) {
                //if you cannot write, close the application
                msg("Connection Failure");

            }
        }
    }



}
