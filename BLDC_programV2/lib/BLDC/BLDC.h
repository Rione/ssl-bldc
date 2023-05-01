#ifndef BLDC_H
#define BLDC_H

#include <mbed.h>
#include "FastPWM.h"
#include "PID.h"
#include "AS5048A.h"
#include "Math.h"
#include "LPF.h"

#define NOTSET -9999
#define ENCODER_DIR -1
class BLDCMotor {
  private:
    FastPWM pwmU, pwmV, pwmW;
    uint8_t polePairQty;
    PID velocityPID;
    Serial *pc;
    As5048Spi encoder;
    LPF velocityLPF;
    Timer timer;

    float supplyVoltage;
    float limitVoltage; // min = -limitVoltage, max = limitVoltage

    float elAngle;                           // electrical angle
    float shAngle, shAnglePrev, shAngleZero; // shaft angle　

    float targetVelocity;
    float limitVelocity;
    float velocity;

    int16_t available;
    bool debug;

  public:
    BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerPairQty, float _dt, Serial *_pc);
    void init();

    void setAbsoluteZero(int _shAngleZero = NOTSET);
    void Diagnose();

    void setSupplyVoltage(float _supplyVoltage, float _limitVoltage);
    void setPIDGain(float _p, float _i, float _d);

    void setVelocity(float _velocity);
    void setVelocityLimit(float _limit);
    float getTargetVelocity();

    void writePwm(float _pwmA, float _pwmB, float _pwmC);
    void setPWMFrequency(int _freq);

    float updateEncoder();
    int getShaftAngle();
    int getElectricAngle();
    float getAngularVelocity();

    void setPhaseVoltage(float _Uq, float Ud, float _elAngle);
    void openLoopControl(float _A, float _elAngle);
    void drive();
};

#endif