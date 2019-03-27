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

import java.util.ArrayList;
import java.util.List;

public class SDCard extends AppCompatActivity {

    private TextView txtContent;
    private Spinner selectFileSpinner;

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
                txtContent.setText(FileHelper.ReadFile(SDCard.this)); //read the file and save
                addItemsToSpinner();
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

        //place holder for now
        Toast.makeText(getApplicationContext(),
                "OnClickListener : " +
                        "\nSpinner selection : "+ String.valueOf(selectFileSpinner.getSelectedItem()),
                Toast.LENGTH_SHORT).show();
    }

}
