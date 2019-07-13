package com.example.iot_joining;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class IoTDevice extends AppCompatActivity {
    private String TAG = "IoTDevice";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_io_tdevice);

        Intent intent = getIntent();
        String ssid = intent.getStringExtra("SSID");

        TextView iot_SSID = findViewById(R.id.iot_SSID);
        String title = "Connected to: " + ssid;
        iot_SSID.setText(title);
    }

    public void JoinNetwork(View view){
        TextView ssid_view = findViewById(R.id.iot_ssid);
        TextView pass_view = findViewById(R.id.iot_password);

        sendWifiCredentials(ssid_view.getText().toString(), pass_view.getText().toString());
    }

    private void sendWifiCredentials(final String SSID, final String password) {
        Thread thread = new Thread(() -> {
            try {
                DatagramSocket ds;
                try {
                    ds = new DatagramSocket();
                    String str = SSID + ":" + password;
                    InetAddress ip = InetAddress.getByName("192.168.4.255");
//                    InetAddress address = InetAddress.getByName(Utils.getBroadcastAddress());


                    DatagramPacket dp = new DatagramPacket(str.getBytes(), str.length(), ip, 4210);

                    ds.setBroadcast(true);
                    Log.e(TAG, "run: Sending..." + str);
                    ds.send(dp);
                    Log.e(TAG, "run: Waiting to receive...");
                    ds.receive(dp);
                    Log.e(TAG, "run: Received");
                    String s = new String(dp.getData(), 0, dp.getLength());
                    Log.e(TAG, "run: " + s);
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
