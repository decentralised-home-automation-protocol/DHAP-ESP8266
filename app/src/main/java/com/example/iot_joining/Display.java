package com.example.iot_joining;

import android.content.Intent;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.ScrollView;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;

import me.aidengaripoli.dynamicdevicedisplay.IoTDevice;
import me.aidengaripoli.dynamicdevicedisplay.IotNetworkDiscovery;
import me.aidengaripoli.dynamicdevicedisplay.OnFragmentInteractionListener;
import me.aidengaripoli.dynamicdevicedisplay.UiGenerator;

public class Display extends AppCompatActivity implements OnFragmentInteractionListener {
    FragmentManager fragmentManager;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        fragmentManager = getSupportFragmentManager();
        ScrollView scrollView = new ScrollView(this);
        IoTDevice device = new IoTDevice("TV.xml", "TV");
        Intent intent = getIntent();
        String xml = intent.getStringExtra("xml");
        InputStream inputStream = new ByteArrayInputStream(xml.getBytes(Charset.forName("UTF-8")));

        device.setDisplayStream(inputStream);

        UiGenerator uiGenerator = new UiGenerator(fragmentManager, this);
        LinearLayout rootLayout = uiGenerator.generateUi(device);
        scrollView.addView(rootLayout);

        // add the root layout to the content view
        setContentView(scrollView);
    }

    @Override
    public void onFragmentMessage(String tag, String data) {
        Log.i("Fragment Message", tag + ": " + data);
    }
}
