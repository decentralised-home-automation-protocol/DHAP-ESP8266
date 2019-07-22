package com.example.iot_joining;

import android.content.Intent;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ScrollView;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.charset.Charset;

import me.aidengaripoli.dynamicdevicedisplay.IoTDevice;
import me.aidengaripoli.dynamicdevicedisplay.IotNetworkDiscovery;
import me.aidengaripoli.dynamicdevicedisplay.OnFragmentInteractionListener;
import me.aidengaripoli.dynamicdevicedisplay.UiGenerator;

public class Display extends AppCompatActivity implements OnFragmentInteractionListener {
    private String TAG = "Display";

    FragmentManager fragmentManager;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        fragmentManager = getSupportFragmentManager();
        ScrollView scrollView = new ScrollView(this);
        IoTDevice device = new IoTDevice("TV.xml", "TV");
        Intent intent = getIntent();
        String packetData = intent.getStringExtra("xml");
        String xml = packetData.substring(4); //Remove the first 4 characters as they define the packets type.
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
        sendCommand(tag, data);
    }

    public void sendCommand(String tag, String data){
        Thread thread = new Thread(() -> {
            try {
                DatagramSocket ds;
                try {
                    ds = new DatagramSocket();
                    String str = "400|"+tag+": " + data;
                    InetAddress ip = InetAddress.getByName("192.168.1.102");

                    DatagramPacket requestPacket = new DatagramPacket(str.getBytes(), str.length(), ip, 8888);

                    Log.e(TAG, "run: Sending Request " + str);
                    ds.send(requestPacket);
                    ds.close();
                } catch (SocketException e) {
                    e.printStackTrace();
                } catch (UnknownHostException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        });

        thread.start();
    }
}
