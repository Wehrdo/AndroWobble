package com.davidawehr.androwobble;

import android.app.Notification;
import android.app.PendingIntent;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //NativeCalls.initSensors();
    }

    @Override
    public void onResume() {
        super.onResume();
        //NativeCalls.registerSensors();
    }

    @Override
    public void onPause() {
        super.onPause();
        //NativeCalls.unregisterSensors();
    }

    public void onStartBalanceBtn(View view) {
        Intent serviceIntent = new Intent(this, BalanceService.class);
        startService(serviceIntent);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
