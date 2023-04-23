#include <BLDC.h>

BLDCMotor::BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerQty, float _dt, RawSerial *_pc)
    : pwmU(_pwmU),
      pwmV(_pwmV),
      pwmW(_pwmW),
      polePairQty(_polerQty),
      velocityPID(0, 0, 0, _dt, _pc), // default gain
      pc(_pc),
      encoder(D11, D12, D13, D10),
      velocityLPF(0.005),
      available(NOTSET),
      debug(_pc != 0), targetVelocity(0) {}

void BLDCMotor::init() {
    pwmU.period_us(5);
    pwmV.period_us(5);
    pwmW.period_us(5);
    writePwm(0, 0, 0);

    velocityPID.setLimit(200);
    velocityPID.setGain(0, 0, 0);

    encoder.frequency(8e6);
    timer.start();

    supplyVoltage = 12;
    limitVoltage = 6;
    limitVelocity = 400;
    shAnglePrev = shAngle = 0;
    setAbsoluteZero();
    if (debug)
        pc->printf("\n\n\n\n\n\nBLDC init\n");
}

void BLDCMotor::setAbsoluteZero(int _shAngleZero) {
    if (_shAngleZero != NOTSET) {
        shAngleZero = _shAngleZero;
    } else {
        openLoopControl(0.1, 0);
        wait(0.5);
        shAngleZero = updateEncoder();

        if (debug) {
            pc->printf("\n\n\n\n\n");
            if (shAngleZero == -1) {
                pc->printf("- error: encoder not found");
            } else {
                pc->printf("- shAngleZero:%f\n", shAngleZero);
            }
        }
        Diagnose(); // 故障診断
        writePwm(0, 0, 0);
    }
}

void BLDCMotor::Diagnose() {
    if (debug) {
        pc->printf("- Diagnose the Motor Driver..\n");
    }

    int d = 0;
    float angleMin = TWO_PI;
    float angleMax = 0;
    // max search
    while (d < 360 * polePairQty) {
        updateEncoder();
        openLoopControl(0.1, Radians(d));
        d += 1;
        wait_us(200);
    }
    wait_ms(100);
    angleMax = shAngle;

    // min search
    while (d > 0) {
        updateEncoder();
        openLoopControl(0.1, Radians(d));
        d -= 1;
        wait_us(200);
    }
    wait_ms(100);
    angleMin = shAngle;
    available = (abs(angleMax - TWO_PI) < 0.1 && abs(angleMin) < 0.1); // check 1 tern
    if (debug)
        pc->printf("- EncoderMax:%f EncoderMin:%f\n", angleMax, angleMin);

    if (!available) {
        if (debug)
            pc->printf("- Error: polerPairQty isn't correct or Motor Driver is broken....\n");
    } else {
        if (debug)
            pc->printf("- This Motor Driber is available!!!\n");
    }
}

void BLDCMotor::setSupplyVoltage(uint8_t _supplyVoltage, uint8_t _limitVoltage) {
    supplyVoltage = _supplyVoltage;
    limitVoltage = _limitVoltage;
}

void BLDCMotor::setVelocityLimit(float _limit) {
    limitVelocity = _limit;
    velocityPID.setLimit(_limit);
}

void BLDCMotor::setPIDGain(float _p, float _i, float _d) {
    velocityPID.setGain(_p, _i, _d);
    if (debug)
        pc->printf("setGain P:%f I:%f D:%f\n", _p, _i, _d);
}

void BLDCMotor::setVelocity(float _velocity) {
    targetVelocity = _velocity;
    if (debug)
        pc->printf("setVelocity:%f\n", targetVelocity);
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
    //     pc->printf("%f %f %f\n", pwmA, pwmB, pwmC);
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
    if (debug)

        pc->printf("vel:%.4f sh:%f dt:%f\n", angularVelocity, shAngle, dt);
    return angularVelocity;
}

void BLDCMotor::setPhaseVoltage(float Uq, float _elAngle) {
    float T0, T1, T2;
    float Ta, Tb, Tc;
    float Ua, Ub, Uc;

    uint8_t sector;
    _elAngle = Degrees(_elAngle);

    Uq = abs(Uq);

    sector = Round(_elAngle / 60) + 1;
    T1 = SQRT3 * sin(sector * 60 - _elAngle) * Uq / supplyVoltage;
    T2 = SQRT3 * sin(_elAngle - (sector - 1.0) * 60) * Uq / supplyVoltage;
    T0 = 0 - T1 - T2;
    if (debug)
        pc->printf("elAngle:%f, sector:%d \n", _elAngle, sector);
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

    Ua = Ta * supplyVoltage;
    Ub = Tb * supplyVoltage;
    Uc = Tc * supplyVoltage;

    Ua = Constrain(Ua, 0, limitVoltage);
    Ub = Constrain(Ub, 0, limitVoltage);
    Uc = Constrain(Uc, 0, limitVoltage);

    if (debug)
        pc->printf("%f %f %f\r", Ua, Ub, Uc);

    if (debug)
        pc->printf("%f %f %f\r", Ta, Tb, Tc);

    writePwm(Ta, Tb, Tc);
}

void BLDCMotor::openLoopControl(float _A, float _elAngle) {
    float a = _A * sin(Degrees(_elAngle)) + 0.5;
    float b = _A * sin(Degrees(_elAngle) + 120) + 0.5;
    float c = _A * sin(Degrees(_elAngle) - 120) + 0.5;
    writePwm(a, b, c);
}

void BLDCMotor::drive() {
    updateEncoder();
    velocity = getAngularVelocity();
    velocityPID.appendError(targetVelocity - velocity);
    float Uq = velocityPID.getPID();
    setPhaseVoltage(Uq, elAngle);
    if (debug)
        pc->printf("velocity: %f\n", velocity);
}