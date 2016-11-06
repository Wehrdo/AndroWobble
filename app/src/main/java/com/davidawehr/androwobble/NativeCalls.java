package com.davidawehr.androwobble;

/**
 * Created by dawehr on 11/5/2016.
 */

public class NativeCalls {

    // Must be called before registering sensors
    public static native void initSensors();

    // Initializes sensors to begin receiving data
    public static native void registerSensors();

    // Unregisters sensors. Should be called in onPause()
    public static native void unregisterSensors();
}
