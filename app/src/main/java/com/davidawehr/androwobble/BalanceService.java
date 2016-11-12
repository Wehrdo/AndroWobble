package com.davidawehr.androwobble;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

/**
 * Created by dawehr on 11/9/2016.
 */

public class BalanceService extends Service {
    private static final int BAL_SERVICE_NOTIF_ID = 12345;
    private static final String EXTRA_ACTION = "com.davidawehr.androwobble.what";
    private static final String EXTRA_ACTION_STOP = "stop";

    private static boolean balancing = false;
    private final IBinder mBinder = new BalanceBinder();

    @Override
    public void onDestroy() {
        super.onDestroy();
        // TODO: Stop balancing if balancing
        if (balancing) {
            Log.v("", "stopping balancing");
        }
    }

    private void beginBalancing() {
        Log.v("", "starting balancing");
    }

    public class BalanceBinder extends Binder {
        BalanceService getService() {
            // Return local instance of service so its methods can be called
            return BalanceService.this;
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null) {
            if (intent.hasExtra(EXTRA_ACTION)) {
                String what = intent.getStringExtra(EXTRA_ACTION);
                switch (what) {
                    case EXTRA_ACTION_STOP:
                        stopBalancing();
                        return START_NOT_STICKY;
                }
            }
        }

        if (!balancing) {
            // Create intent to stop service
            Intent stopBalanceIntent = new Intent(this, this.getClass());
            stopBalanceIntent.putExtra(EXTRA_ACTION, EXTRA_ACTION_STOP);
            // Package into PendingIntent for notification
            PendingIntent i = PendingIntent.getService(
                    this,
                    0,
                    stopBalanceIntent,
                    PendingIntent.FLAG_UPDATE_CURRENT
            );

            // TODO: Make clicking notification bring user to MainActivity
            // Add "Stop" action button in notification
            Notification.Action stopAct = new Notification.Action(R.drawable.stop_bal_notif_icon, "Stop", i);
            Notification notification = new Notification.Builder(this)
                    .setContentTitle("Balancing")
                    .setSmallIcon(R.drawable.balance_notif_icon)
                    .addAction(stopAct)
                    .build();

            // Start service as foreground
            startForeground(BAL_SERVICE_NOTIF_ID, notification);
            balancing = true;
         }

        return START_STICKY;
    }

    // Whether the service is balancing or not
    public boolean getBalancing() {
        return balancing;
    }

    public void stopBalancing() {
        balancing = false;
        // Removes the notification
        stopForeground(true);
        stopSelf();
    }
}
