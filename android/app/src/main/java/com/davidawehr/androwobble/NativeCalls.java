package com.davidawehr.androwobble;

/**
 * Created by dawehr on 11/5/2016.
 */

public class NativeCalls {
    public static native void beginBalancing();

    public static native void stopBalancing();

    public static native void setConstants(double p, double i, double d);

    public static native void destroyStream();
}
