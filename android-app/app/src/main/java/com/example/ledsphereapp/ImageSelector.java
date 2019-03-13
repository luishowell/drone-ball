package com.example.ledsphereapp;

import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.EditText;
import android.content.Intent;
import com.soundcloud.android.crop.Crop;

import android.widget.TextView;
import android.widget.Toast;
import android.view.Menu;
import android.view.MenuItem;


import java.io.File;

public class ImageSelector extends AppCompatActivity {

    private static final int PICK_IMAGE = 100;
    Uri imageUri;
    ImageView imageView2;

    float rotateAngle = 0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_selector);
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

        imageView2 = (ImageView) findViewById(R.id.imageView2);


        Button gallery_button = (Button) findViewById(R.id.gallery_button);
        gallery_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openGallery();
            }

        });

        Button button_send_image = (Button) findViewById(R.id.button_send_image);
        button_send_image.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Snackbar.make(v, "I'd send this now", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }

        });

        Button button_rotate = (Button) findViewById(R.id.button_rotate);
        button_rotate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                rotateImage();
            }

        });



    }

    private void openGallery() {
        //Intent gallery = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.INTERNAL_CONTENT_URI);
        //startActivityForResult(gallery, PICK_IMAGE);
        imageView2.setImageDrawable(null);
        Crop.pickImage((this));
    }

    @Override
    protected void  onActivityResult(int requestCode, int resultCode, Intent data){
        super.onActivityResult(requestCode, resultCode, data);
        //if(resultCode == RESULT_OK && requestCode == PICK_IMAGE){
        if(requestCode == Crop.REQUEST_PICK && resultCode == RESULT_OK){
            beginCrop(data.getData());
        } else if (requestCode == Crop.REQUEST_CROP){
            handleCrop(resultCode, data);
        } else if(resultCode == RESULT_OK && requestCode == PICK_IMAGE){
            imageUri = data.getData();
            imageView2.setImageURI(imageUri);
        }
    }

    private void beginCrop(Uri source){
        Uri outputUri = Uri.fromFile(new File(getCacheDir(), "cropped"));
        final GlobalVariables globalVars = (GlobalVariables)getApplication();
        new Crop(source).output(outputUri).withFixedSize(globalVars.imageWidth,globalVars.imageHeight).start(this);
    }

    private void handleCrop(int resultCode, Intent result) {
        if (resultCode == RESULT_OK) {
            imageView2.setImageURI(Crop.getOutput(result));
        } else if (resultCode == Crop.RESULT_ERROR) {
            Toast.makeText(this, Crop.getError(result).getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void rotateImage() {
        rotateAngle = (rotateAngle + 90) % 360;
        imageView2.setRotation(rotateAngle);
    }

}
