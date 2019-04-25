package com.example.ledsphereapp;

import android.app.Application;
import android.bluetooth.BluetoothSocket;

public class GlobalVariables extends Application {
    int imageHeight = 130;
    int imageWidth = 200;

    BluetoothSocket btSocket = null;

    int hoopRPM = 33; // as a percentage of max
    int maxRPM = 350;
}
