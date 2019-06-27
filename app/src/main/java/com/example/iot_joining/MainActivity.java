package com.example.iot_joining;

import android.content.Context;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import java.util.List;

public class MainActivity extends AppCompatActivity {
    private String TAG = "results";

    LinearLayout foundNetworks;

    WifiManager wifiManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        foundNetworks = findViewById(R.id.found_networks);

        wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
    }

    public void scanWiFI(View view) {
        foundNetworks.removeAllViews();
        wifiManager.startScan();
        List<ScanResult> results = wifiManager.getScanResults();
        for (final ScanResult r : results) {

            Button networkButton = new Button(getApplicationContext());
            networkButton.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    //Join wifi network.
                    Log.e(TAG, "onClick: " + r.SSID);
                    wifiManager.setWifiEnabled(true);

                    WifiConfiguration wifiConfig = new WifiConfiguration();
                    wifiConfig.SSID = String.format("\"%s\"", r.SSID);
                    wifiConfig.preSharedKey = String.format("\"%s\"", "thereisnospoon");

                    WifiManager wifiManager = (WifiManager)getSystemService(WIFI_SERVICE);
                    int netId = wifiManager.addNetwork(wifiConfig);
                    wifiManager.disconnect();
                    wifiManager.enableNetwork(netId, true);
                    wifiManager.reconnect();
                }
            });

            networkButton.setText(r.SSID);
            foundNetworks.addView(networkButton);
        }
    }
}