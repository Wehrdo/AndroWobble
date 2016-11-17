#include <cmath>

#define INTEGRAL_THRESHHOLD

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

	if(abs(error) < INTEGRAL_THRESHHOLD) {
		integralError = integralError + error;
	} else {
		integralError = 0;
	}

	proportional = error * p;
	integral = integralError * i;
	derivative = (lastAngle - currAngle) * d;

	lastAngle = currAngle;

	return (proportional + integral + derivative);
} 