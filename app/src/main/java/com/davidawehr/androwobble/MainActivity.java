package com.davidawehr.androwobble;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import com.davidawehr.androwobble.BalanceService.BalanceBinder;

public class MainActivity extends AppCompatActivity {
    BalanceService balService;
    boolean bound = false;
    Button toggleBalBtn;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        toggleBalBtn = (Button) findViewById(R.id.start_service_btn);
        //NativeCalls.initSensors();
    }

    @Override
    public void onStart() {
        super.onStart();
        // Bind to the balancing service on start.
        Intent bindInt = new Intent(this, BalanceService.class);
        bindService(bindInt, mConnection, Context.BIND_AUTO_CREATE);
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

    @Override
    public void onStop() {
        super.onStop();
        // Unbind when we stop. Because the service is also created with startService(),
        // the service will not necessarily terminate. If it is not balancing, and we quit the app,
        // then the service will terminate
        if (bound) {
            unbindService(mConnection);
            bound = false;
        }
    }

    // The start/stop balancing button was clicked
    public void onToggleBalanceBtn(View view) {
        // Start balancing if the service doesn't exist (It should exist because we bind to it)
        // or if it is not currently balancing
        if (balService == null || !balService.getBalancing()) {
            Intent serviceIntent = new Intent(this, BalanceService.class);
            startService(serviceIntent);
            toggleBalBtn.setText(R.string.stop_bal_service);
        }
        // Otherwise, if the service exists but we are balancing, then stop balancing
        else if (balService != null) {
            balService.stopBalancing();
            toggleBalBtn.setText(R.string.start_bal_service);
        }
        else {
            throw new RuntimeException("This shouldn't happen? onToggleBalanceBtn");
        }
    }

    // Service connection manager
    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            // Just bound to service
            BalanceBinder binder = (BalanceBinder) service;
            balService = binder.getService();
            bound = true;
            if (balService.getBalancing()) {
                toggleBalBtn.setText(R.string.stop_bal_service);
            }
            else {
                toggleBalBtn.setText(R.string.start_bal_service);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            bound = false;
        }
    };
}
