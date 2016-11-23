#include <cmath>
#include "pid.h"
#define INTEGRAL_THRESHHOLD 0.1

static double p = 0;
static double i = 0;
static double d = 0;

static double proportional = 0;
static double integral = 0;
static double derivative = 0;

static double lastAngle = 0;

double pidUpdate(double currAngle, double desiredAngle) {
	double error = desiredAngle - currAngle;
	double integralError = 0;

	if(fabs(error) < INTEGRAL_THRESHHOLD) {
		integralError = integralError + error;
	} else {
		integralError = 0;
	}

	proportional = error * p;
	integral = integralError * i;
	derivative = (currAngle - lastAngle) * d;

	lastAngle = currAngle;

	return (proportional + integral + derivative);
} 

void updateConstants(double _p, double _i, double _d) {
	p = _p;
	i = _i;
	d = _d;
}