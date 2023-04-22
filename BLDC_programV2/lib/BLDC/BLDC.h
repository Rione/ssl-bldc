#ifndef BLDC_H
#define BLDC_H

#include <mbed.h>
#include <FastPWM.h>
#include <PID.h>
#include <AS5048A.h>
#include <Math.h>

class BLDCMotor {
  private:
    FastPWM pwmU, pwmV, pwmW;
    uint8_t polerQty;
    PID velocityPID;
    RawSerial *pc;
    As5048Spi encoder;
    uint8_t supplyVoltage;
    uint8_t limitVoltage;
    bool debug;

    int elAngle;              // electrical angle 電気角度
    int shAngle, shAnglePrev; // shaft angle　機械角度

    Timer timer;

    float targetVelocity;
    float velocity;

  public:
    BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerQty, float _dt, RawSerial *_pc);
    void init();
    void setSupplyVoltage(uint8_t _supplyVoltage, uint8_t _limitVoltage);
    void setPIDGain(float _p, float _i, float _d);
    void setVelocity(float _velocity);
    void writePwm(float _pwmA, float _pwmB, float _pwmC);

    bool getEncoder();
    int getShaftAngle();
    int getElectricAngle();
    float getAnglerVelocity();

    void setPhaseVoltage(float _Uq, float _elAngle);

    void drive();
};

#endif