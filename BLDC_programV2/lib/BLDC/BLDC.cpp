#include <BLDC.h>

BLDCMotor::BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerQty, float _dt, Serial *_pc)
    : pwmU(_pwmU),
      pwmV(_pwmV),
      pwmW(_pwmW),
      polePairQty(_polerQty),
      velocityPID(0.0001, 0, 0, _dt, _pc), // default gain
      pc(_pc),
      encoder(D11, D12, D13, D10),
      velocityLPF(0.01),

      supplyVoltage(12),
      limitVoltage(6),
      elAngle(0),
      shAngle(0),
      shAnglePrev(0),
      shAngleZero(0),
      targetVelocity(0),
      limitVelocity(300),
      velocity(0),
      available(NOTSET),
      debug(_pc != 0) {}

void BLDCMotor::init() {
    pwmU.period_us(5);
    pwmV.period_us(5);
    pwmW.period_us(5);
    writePwm(0, 0, 0);

    velocityPID.setLimit(8); // 2.3
    encoder.frequency(8e6);
    timer.start();
    setAbsoluteZero();
    Diagnose(); // 故障診断
    available = true;
    if (debug)
        pc->printf("\n\n\n\n\n\nBLDC init\n");
}

void BLDCMotor::setAbsoluteZero(int _shAngleZero) {
    if (_shAngleZero != NOTSET) {
        shAngleZero = _shAngleZero;
    } else {
        openLoopControl(1.5, HALF_PI);
        // wait(0.5);
        wait_us(500000);
        shAngleZero = updateEncoder();

        if (debug) {
            pc->printf("\n\n\n\n\n");
            if (shAngleZero == -1) {
                pc->printf("- error: encoder not found");
            } else {
                pc->printf("- shAngleZero:%f", shAngleZero);
            }
        }
        writePwm(0, 0, 0);
    }
}

void BLDCMotor::Diagnose() {
    available = NOTSET;
    if (debug) {
        pc->printf("- Diagnose the Motor Driver..\n");
    }
    // if (abs(gapRadians(shAngle, Radians(deg))) > 0.1) {
    //     if (debug)
    //         pc->printf("- Error: Motor Driver is broken....\n\n\n\n\n");
    //     return;
    // }
    // 一旦諦め
    //  bool a, b, c, d = false;
    //  while (deg < 360 * polePairQty) {
    //      openLoopControl(1, Radians(deg));
    //      deg += 1;
    //      updateEncoder();
    //      if (abs(gapRadians(shAngle, HALF_PI)) < 0.1 && !a) {
    //          a = true;
    //          pc->printf("%d %d %d %d\n", a, b, c, d);
    //      };
    //      if (abs(gapRadians(shAngle, PI)) < 0.1 && !b) {
    //          b = true;
    //          pc->printf("%d %d %d %d\n", a, b, c, d);
    //      };
    //      if (abs(gapRadians(shAngle, 3 * HALF_PI)) < 0.1 && !c) {
    //          c = true;
    //          pc->printf("%d %d %d %d\n", a, b, c, d);
    //      };
    //      if (abs(gapRadians(shAngle, TWO_PI)) < 0.1 && !d) {
    //          d = true;
    //          pc->printf("%d %d %d %d\n", a, b, c, d);
    //      }
    //  }

    // available = a && b && c && d;
    available = 1;

    if (!available) {
        if (debug)
            pc->printf("- Error: polerPairQty isn't correct or Motor Driver is broken....\n\n\n\n\n");
    } else {
        if (debug)
            pc->printf("- This Motor Driber is available!!!\n\n\n\n\n");
    }
    writePwm(0, 0, 0);
    // wait(1);
    wait_us(1000000);
}

void BLDCMotor::setSupplyVoltage(float _supplyVoltage, float _limitVoltage) {
    supplyVoltage = _supplyVoltage;
    limitVoltage = _limitVoltage;
}

void BLDCMotor::setVelocityLimit(float _limit) {
    limitVelocity = _limit;
}

void BLDCMotor::setPIDGain(float _p, float _i, float _d) {
    velocityPID.setGain(_p, _i, _d);
    if (debug)
        pc->printf("setGain P:%f I:%f D:%f\n", _p, _i, _d);
}

void BLDCMotor::setVelocity(float _velocity) {
    targetVelocity = Constrain(_velocity, -limitVelocity, limitVelocity);
    // if (debug)
    //     pc->printf("setVelocity:%f\n", targetVelocity);
}

float BLDCMotor::getTargetVelocity() {
    return targetVelocity;
}

void BLDCMotor::writePwm(float pwmA, float pwmB, float pwmC) {
    if (available == false) return;
    pwmA = Constrain(pwmA, 0, 1.0);
    pwmB = Constrain(pwmB, 0, 1.0);
    pwmC = Constrain(pwmC, 0, 1.0);
    pwmU.write(pwmA);
    pwmV.write(pwmB);
    pwmW.write(pwmC);
    // if (debug)
    //     pc->printf("%f %f %f\n", pwmA * 1000, pwmB * 1000, pwmC * 1000);
}

void BLDCMotor::setPWMFrequency(int _freq) {
    pwmU.period_us(10e6 / _freq);
    pwmV.period_us(10e6 / _freq);
    pwmW.period_us(10e6 / _freq);
}

float BLDCMotor::updateEncoder() {
    int _angle = *encoder.read_angle();
    if (encoder.parity_check(_angle)) {
        shAnglePrev = shAngle;
        _angle *= ENCODER_DIR;
        float __angle = (float)(As5048Spi::radian(_angle)) / 10000;
        shAngle = gapRadians(shAngleZero, __angle);
        elAngle = normalizeRadians(shAngle * polePairQty);
        // if (debug)
        //     pc->printf("shAngleZero:%f :shAngle: %f elAngle: %f\n", shAngleZero, shAngle, elAngle);
        return __angle;
    } else {
        if (debug)
            pc->printf("ENCODER: Parity check failed.\n");
        return -1;
    }
}

int BLDCMotor::getShaftAngle() {
    return shAngle;
}

int BLDCMotor::getElectricAngle() {
    return elAngle;
}

float BLDCMotor::getAngularVelocity() {
    float dt = timer.read();
    timer.reset();
    float angleDiff = shAnglePrev - shAngle;
    if (angleDiff > PI) {
        angleDiff -= 2 * PI;
    } else if (angleDiff < -PI) {
        angleDiff += 2 * PI;
    }
    double angularVelocity = angleDiff / dt;
    if (abs(angleDiff) > PI && angularVelocity != 0) {
        angularVelocity += (angularVelocity > 0) ? -2 * PI / dt : 2 * PI / dt;
    }
    angularVelocity = velocityLPF.update(angularVelocity);
    shAnglePrev = shAngle;
    // if (debug)
    //     pc->printf("vel:%.4f sh:%f dt:%f\n", angularVelocity, shAngle, dt);
    return angularVelocity;
}

void BLDCMotor::setPhaseVoltage(float Uq, float Ud, float _elAngle) {
    float T0, T1, T2;
    float Ta, Tb, Tc;
    float Ua, Ub, Uc;

    uint8_t sector;

    float Uout;
    // from SimpleFOC
    // a bit of optitmisation
    if (Ud) { // only if Ud and Uq set
        // _sqrt is an approx of sqrt (3-4% error)
        Uout = sqrt(Ud * Ud + Uq * Uq) / limitVoltage;
        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        _elAngle = normalizeRadians(_elAngle + atan2(Uq, Ud));
    } else { // only Uq available - no need for atan2 and sqrt
        Uout = Uq / limitVoltage;
        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        _elAngle = normalizeRadians(_elAngle + HALF_PI);
    }
    _elAngle = normalizeRadians(_elAngle);

    Uq = abs(Uq);

    sector = (int)(_elAngle / PI_3) + 1;
    T1 = SQRT3 * sin(sector * PI_3 - _elAngle) * Uout;
    T2 = SQRT3 * sin(_elAngle - (sector - 1.0) * PI_3) * Uout;
    T0 = 0 - T1 - T2;
    // if (debug)
    //     pc->printf("elAngle:%f, sector:%d \n", _elAngle, sector);
    if (sector == 1) {
        Ta = T1 + T2 + T0 / 2;
        Tb = T2 + T0 / 2;
        Tc = T0 / 2;
    } else if (sector == 2) {
        Ta = T1 + T0 / 2;
        Tb = T1 + T2 + T0 / 2;
        Tc = T0 / 2;
    } else if (sector == 3) {
        Ta = T0 / 2;
        Tb = T1 + T2 + T0 / 2;
        Tc = T2 + T0 / 2;
    } else if (sector == 4) {
        Ta = T0 / 2;
        Tb = T1 + T0 / 2;
        Tc = T1 + T2 + T0 / 2;
    } else if (sector == 5) {
        Ta = T2 + T0 / 2;
        Tb = T0 / 2;
        Tc = T1 + T2 + T0 / 2;
    } else if (sector == 6) {
        Ta = T1 + T2 + T0 / 2;
        Tb = T0 / 2;
        Tc = T1 + T0 / 2;
    } else {
        Ta = 0;
        Tb = 0;
        Tc = 0;
    }

    Ua = Constrain(Ta * supplyVoltage, -limitVoltage, limitVoltage) + supplyVoltage / 2;
    Ub = Constrain(Tb * supplyVoltage, -limitVoltage, limitVoltage) + supplyVoltage / 2;
    Uc = Constrain(Tc * supplyVoltage, -limitVoltage, limitVoltage) + supplyVoltage / 2;

    // if (debug)
    //     pc->printf("%f %f %f\n", Ua, Ub, Uc);

    Ua /= supplyVoltage;
    Ub /= supplyVoltage;
    Uc /= supplyVoltage;

    writePwm(Ua, Ub, Uc);
}

void BLDCMotor::openLoopControl(float _Uq, float _elAngle) {
    // float a = _Uq * sin(_elAngle) + 0.5;
    // float b = _Uq * sin(_elAngle + PI_3) + 0.5;
    // float c = _Uq * sin(_elAngle - PI_3) + 0.5;
    // writePwm(a, b, c);
    setPhaseVoltage(_Uq, 0, _elAngle);
    wait_us(200);
}
int deg = 0;
void BLDCMotor::drive() {
    updateEncoder();
    velocity = getAngularVelocity();
    velocityPID.appendError(targetVelocity - velocity);
    float Uq = velocityPID.getPID();
    float Ud = 0;
    setPhaseVoltage(Uq, Ud, elAngle);
    // pc->printf("V:%.3f\tUq:%.3f \n", velocity, Uq);
}