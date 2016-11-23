//
// Created by dawehr on 11/22/2016.
//

#ifndef ANDROID_PID_H
#define ANDROID_PID_H

double pidUpdate(double currAngle, double desiredAngle);
void updateConstants(double _p, double _i, double _d);

#endif //ANDROID_PID_H
