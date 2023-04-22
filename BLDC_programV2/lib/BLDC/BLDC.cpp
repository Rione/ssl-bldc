#include <BLDC.h>

BLDCMotor::BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerQty, float _dt, RawSerial *_pc)
    : pwmU(_pwmU),
      pwmV(_pwmV),
      pwmW(_pwmW),
      polerPairQty(_polerQty),
      velocityPID(0, 0, 0, _dt, _pc), // default gain
      pc(_pc),
      encoder(D11, D12, D13, D10),
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
        pc->printf("BLDC init\n");
}

void BLDCMotor::setAbsoluteZero(int _shAngleZero) {
    if (_shAngleZero != NOTSET) {
        shAngleZero = _shAngleZero;
    } else {
        writePwm(0.5, 0, 0);
        wait(0.5);
        shAngleZero = updateEncoder();
        writePwm(0, 0, 0);
    }
    if (debug)
        pc->printf("shaftAngleZero:%d\n", shAngleZero);
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
    pwmA = Constrain(pwmA, 0, 1.0);
    pwmB = Constrain(pwmB, 0, 1.0);
    pwmC = Constrain(pwmC, 0, 1.0);
    pwmU.write(pwmA);
    pwmU.write(pwmB);
    pwmU.write(pwmC);
}

void BLDCMotor::setPWMFrequency(int _freq) {
    pwmU.period_us(10e6 / _freq);
    pwmV.period_us(10e6 / _freq);
    pwmW.period_us(10e6 / _freq);
}

bool BLDCMotor::updateEncoder() {
    int _angle = *encoder.read_angle();
    if (encoder.parity_check(_angle)) {
        shAnglePrev = shAngle;
        _angle = As5048Spi::degrees(_angle) / 100;
        shAngle = gapDegrees(shAngleZero, _angle);
        elAngle = shAngle * polerPairQty;
        return true;
    } else {
        if (debug)
            pc->printf("ENCODER: Parity check failed.\n");
        return false;
    }
}

int BLDCMotor::getShaftAngle() {
    return shAngle;
}

int BLDCMotor::getElectricAngle() {
    return elAngle;
}

float BLDCMotor::getAnglerVelocity() {
    float dt = timer.read();
    timer.reset();
    float vel = gapDegrees(targetVelocity, velocity) / dt;

    if (debug)
        pc->printf("shAngle: %d elAngle: %d\n", shAngle, elAngle);
    return vel;
}

void BLDCMotor::setPhaseVoltage(float Uq, float _elAngle) {
    float T0, T1, T2;
    float Ta, Tb, Tc;
    float Ua, Ub, Uc;

    uint8_t sector;
    _elAngle = normalizeDegrees(_elAngle);

    Uq = abs(Uq);

    sector = Round(_elAngle / 60) + 1;
    T1 = SQRT3 * sin(sector * 60 - _elAngle) * Uq / supplyVoltage;
    T2 = SQRT3 * sin(_elAngle - (sector - 1.0) * 60) * Uq / supplyVoltage;
    T0 = 0 - T1 - T2;
    if (debug)
        pc->printf("sector:%d \n", sector);
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

void BLDCMotor::drive() {
    updateEncoder();
    velocity = getAnglerVelocity();
    velocityPID.appendError(targetVelocity - velocity);
    float Uq = velocityPID.getPID();
    setPhaseVoltage(Uq, elAngle);
    if (debug)
        pc->printf("velocity: %f\n", velocity);
}