#ifndef BLDC_H
#define BLDC_H

#include <mbed.h>
#include "FastPWM.h"
#include "PID.h"
#include "AS5048A.h"
#include "Math.h"
#include "LPF.h"

#define NOTSET -9999
class BLDCMotor {
  private:
    FastPWM pwmU, pwmV, pwmW;
    uint8_t polePairQty;
    PID velocityPID;
    RawSerial *pc;
    As5048Spi encoder;
    uint8_t supplyVoltage;
    uint8_t limitVoltage;
    LPF velocityLPF;
    int8_t available;
    bool debug;

    float elAngle;                           // electrical angle
    float shAngle, shAnglePrev, shAngleZero; // shaft angle　

    Timer timer;

    float targetVelocity;
    float limitVelocity;
    float velocity;

  public:
    BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerPairQty, float _dt, RawSerial *_pc);
    void init();

    void setAbsoluteZero(int _shAngleZero = NOTSET);
    void Diagnose();

        void setSupplyVoltage(uint8_t _supplyVoltage, uint8_t _limitVoltage);
    void setPIDGain(float _p, float _i, float _d);

    void setVelocity(float _velocity);
    void setVelocityLimit(float _limit);

    void writePwm(float _pwmA, float _pwmB, float _pwmC);
    void setPWMFrequency(int _freq);

    float updateEncoder();
    int getShaftAngle();
    int getElectricAngle();
    float getAngularVelocity();

    void setPhaseVoltage(float _Uq, float _elAngle);
    void openLoopControl(float _A, float _elAngle);
    void drive();
};

#endif