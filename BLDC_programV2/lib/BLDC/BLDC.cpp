#include <BLDC.h>

BLDCMotor::BLDCMotor(PinName _pwmU, PinName _pwmV, PinName _pwmW, uint8_t _polerQty, float _dt, RawSerial *_pc)
    : pwmU(_pwmU),
      pwmV(_pwmV),
      pwmW(_pwmW),
      polerQty(_polerQty),
      velocityPID(0, 0, 0, _dt, _pc), // default gain
      pc(_pc),
      encoder(D11, D12, D13, D10),
      supplyVoltage(12),
      limitVoltage(6),
      debug(_pc != 0), targetVelocity(0) {}

void BLDCMotor::init() {
    pwmU.period_us(5);
    pwmV.period_us(5);
    pwmW.period_us(5);

    velocityPID.setLimit(200);
    velocityPID.setGain(0, 0, 0);

    encoder.frequency(8000000);

    timer.start();
    if (debug)
        pc->printf("BLDC init\r");
}

void BLDCMotor::setSupplyVoltage(uint8_t _supplyVoltage, uint8_t _limitVoltage) {
    supplyVoltage = _supplyVoltage;
    limitVoltage = _limitVoltage;
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

void BLDCMotor::writePwm(float _pwmA, float _pwmB, float _pwmC) {
    pwmU.write(_pwmA);
    pwmV.write(_pwmB);
    pwmW.write(_pwmC);
    if (debug)
        pc->printf("%f %f %f\r", _pwmA, _pwmB, _pwmC);
}

bool BLDCMotor::getEncoder() {
    int _angle = *encoder.read_angle();
    if (encoder.parity_check(_angle)) {
        shAngle = As5048Spi::degrees(_angle) / 100;
        elAngle = shAngle * polerQty;
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
    shAnglePrev = shAngle;
    shAngle = getEncoder();
    float dt = timer.read();
    timer.reset();
    float vel = (shAnglePrev - shAngle) / dt;

    if (debug)
        pc->printf("shAngle: %d elAngle: %d\r", shAngle, elAngle);
    return vel;
}

void BLDCMotor::setPhaseVoltage(float Uq, float _elAngle) {
    float T0, T1, T2;
    float Ta, Tb, Tc;
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
    float Ua, Ub, Uc;
    Ua = Ta * supplyVoltage;
    Ub = Tb * supplyVoltage;
    Uc = Tc * supplyVoltage;

    writePwm(Ua, Ub, Uc);
}

void BLDCMotor::drive() {
    getEncoder();
    velocity = getAnglerVelocity();
    velocityPID.appendError(targetVelocity - velocity);
    float Uq = velocityPID.getPID();
    setPhaseVoltage(Uq, elAngle);
}