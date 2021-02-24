#include "Arduino.h"
#include "Wheel.h"

Wheel::Wheel(
    unsigned int motorNum, 
    unsigned int encPinA, 
    unsigned int encPinB, 
    bool isClockwise, 
    unsigned int intervalMillis)
{
    // 1900 - stability limit for motor 1
    // kP = 1140; kI = 3040; kD = 107; // position PID

    // kP = 50; kI = 800; kD = 5; // angular velocity PID (crazy on heap to zero)
    kP = 50; kI = 500; kD = 0; // angular velocity PID

    this->motor = new Motor(motorNum);
    this->encoder = new Encoder(encPinA, encPinB, isClockwise);
    this->pid = new PID(&pidFeedback, &pidOutput, &pidSetpoint, kP, kI, kD, DIRECT);
    pid->SetMode(AUTOMATIC);
    pid->SetOutputLimits(-255, 255);
    // pid->SetSampleTime(intervalMillis); // in millis
    // pid->SetTunings(1, 2, 3);
    // pid->SetControllerDirection(DIRECT);

    interval = intervalMillis;
    currentMillis = 0;
    previousMillis = 0;
}
Wheel::~Wheel()
{
    delete motor;
    delete encoder;
    delete pid;
}

void Wheel::setValue(int value)
{
    motor->setValue(value);
}

void Wheel::reachVelocity(double desiredVelocity, double dt)
{
    // currentMillis = millis();
    // unsigned long diff = currentMillis - previousMillis;
    // if (diff > interval)
    // {
    //     previousMillis = currentMillis;

        double revolutions = (double)encoder->getTicks() / TICKS_PER_REV;
        resetEncoder();
        // double linearDistance = PI * DIAMETER * revolutions;
        currentAngularVelocity = revolutions / dt;
        pidFeedback = currentAngularVelocity;
        pidSetpoint = desiredVelocity;
        pid->Compute();
        motor->setValue(pidOutput);
    // }
}

void Wheel::triggerA() { encoder->triggerA(); }
void Wheel::triggerB() { encoder->triggerB(); }
void Wheel::resetEncoder() { encoder->reset(); }

double Wheel::getPidOutput() { return pidOutput; }
double Wheel::getCurrentVelocity() { return currentAngularVelocity; }
long Wheel::getTicks() { return encoder->getTicks(); }
int Wheel::getEncPinA() { return encoder->getPinA(); }
int Wheel::getEncPinB() { return encoder->getPinB(); }