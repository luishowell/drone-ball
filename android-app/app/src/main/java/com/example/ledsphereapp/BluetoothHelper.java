package com.example.ledsphereapp;

import android.app.Activity;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;

public class BluetoothHelper {

    public Activity activity;
    public BluetoothHelper(Activity act)
    {
        this.activity = act;
    }

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
}
