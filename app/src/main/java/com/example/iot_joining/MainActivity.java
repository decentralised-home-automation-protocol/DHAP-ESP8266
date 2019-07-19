package com.example.iot_joining;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private String TAG = "MainActivity";
    private LinearLayout foundNetworks;
    private WifiManager wifiManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        foundNetworks = findViewById(R.id.found_networks);

        wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
    }

    public void scanWiFI(final View view) {
        foundNetworks.removeAllViews();
        wifiManager.startScan();
        List<ScanResult> results = wifiManager.getScanResults();
        for (final ScanResult result : results) {

            Button networkButton = new Button(getApplicationContext());
            networkButton.setOnClickListener(v -> passwordDialog(result.SSID));

            networkButton.setText(result.SSID);
            foundNetworks.addView(networkButton);
        }
    }

    private void passwordDialog(final String SSID) {
        LayoutInflater inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        final View formElementsView = inflater.inflate(R.layout.password_dialog, null, false);
        final EditText passwordEditText = formElementsView.findViewById(R.id.passwordEditText);

        new AlertDialog.Builder(MainActivity.this).setView(formElementsView)
                .setTitle("Enter Password")
                .setPositiveButton("OK", (dialog, id) -> {
                    dialog.cancel();
                    joinNetwork(SSID, passwordEditText.getText().toString());
                }).show();
    }

    private void joinNetwork(String SSID, String password) {
        Log.e(TAG, "joinNetwork: " + SSID + " password: " + password);
        wifiManager.setWifiEnabled(true);

        WifiConfiguration wifiConfig = new WifiConfiguration();
        wifiConfig.SSID = String.format("\"%s\"", SSID);
        wifiConfig.preSharedKey = String.format("\"%s\"", password);

        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        int netId = wifiManager.addNetwork(wifiConfig);
        wifiManager.disconnect();
        wifiManager.enableNetwork(netId, true);
        wifiManager.reconnect();

        ConnectivityManager connManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

        while (!mWifi.isConnected()) {
            mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        }
        Log.e(TAG, "run: connected");

        Intent intent = new Intent(this, IoTDevice.class);
        intent.putExtra("SSID", SSID);
        startActivity(intent);
    }

    public void GetUi(View view){
        Thread thread = new Thread(() -> {
            try {
                DatagramSocket ds;
                try {
                    ds = new DatagramSocket();
                    String str = "200";
                    InetAddress ip = InetAddress.getByName("192.168.1.102");

                    byte[] buf = new byte[4000];

                    DatagramPacket dp = new DatagramPacket(buf, buf.length, ip, 8888);
                    DatagramPacket requestPacket = new DatagramPacket(str.getBytes(), str.length(), ip, 8888);

                    Log.e(TAG, "run: Sending..." + str);
                    ds.send(requestPacket);
                    Log.e(TAG, "run: Waiting to receive...");
                    ds.receive(dp);
                    Log.e(TAG, "run: Received");
                    String s = new String(dp.getData(), 0, dp.getLength());
                    Log.e(TAG, "run: " + s);
                    ds.close();


                    Intent intent = new Intent(this, Display.class);
                    intent.putExtra("xml", s);

                    startActivity(intent);
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

    public void Discovery(View view){
        Thread thread = new Thread(() -> {
            try {
                DatagramSocket ds;
                try {
                    ds = new DatagramSocket();
                    String str = "300";
                    InetAddress ip = InetAddress.getByName("192.168.1.255");

                    byte[] buf = new byte[100];

                    DatagramPacket dp = new DatagramPacket(buf, buf.length, ip, 8888);
                    DatagramPacket requestPacket = new DatagramPacket(str.getBytes(), str.length(), ip, 8888);

                    Log.e(TAG, "run: Sending..." + str);
                    ds.send(requestPacket);
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

    public void Status(View view){
        Thread thread = new Thread(() -> {
            try {
                DatagramSocket ds;
                try {
                    ds = new DatagramSocket();
                    String str = "500:10000,2000,T";
                    InetAddress ip = InetAddress.getByName("192.168.1.255");

                    byte[] buf = new byte[100];

                    DatagramPacket dp = new DatagramPacket(buf, buf.length, ip, 8888);
                    DatagramPacket requestPacket = new DatagramPacket(str.getBytes(), str.length(), ip, 8888);

                    Log.e(TAG, "run: Sending... Status request " + str);
                    ds.send(requestPacket);
                    Log.e(TAG, "run: Waiting to receive...");
                    boolean leaseExpired = false;
                    while(!leaseExpired){
                        ds.receive(dp);
                        Log.e(TAG, "run: Received");
                        String s = new String(dp.getData(), 0, dp.getLength());
                        Log.e(TAG, "run: " + s);
                        if(dp.getData()[0] == 'L'){
                            leaseExpired = true;
                        }
                    }
                    Log.e(TAG, "done");

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