#include "PID.h"

PID::PID(float _p, float _i, float _d, float _dt, RawSerial *_pc) : ticker() {
    pc = _pc;
    p = _p;
    i = _i;
    d = _d;
    dt = _dt;
    limit = 1000;
    setLimit(limit);
    lastError = 0;
    pc->printf("PID init\r");
    ticker.attach(callback(this, &PID::compute), _dt);
}

void PID::setLimit(float _limit) {
    limit = abs(_limit);
    min = -limit;
    max = limit;
}
void PID::setGain(float _p, float _i, float _d) {
    p = _p;
    i = _i;
    d = _d;
}

void PID::appendError(float _error) {
    error = _error;
}

void PID::reset() {
    integral = 0;
    error = 0;
    lastError = 0;
}

void PID::compute() {
    // please append error before compute
    integral += (abs(_output) > limit ? 0 : error * dt); // アンチワインドアップ

    pTerm = p * error;                    // 比例
    iTerm = i * integral;                 // 積分
    dTerm = d * (error - lastError) / dt; // 微分
    lastError = error;

    _output = (pTerm + iTerm + dTerm);
    output = Constrain(_output, max, min);
    // pc->printf("p:%f i:%f d:%f Output: %f _out:%f\n", pTerm, iTerm, dTerm, output, _output);
}

float PID::getOutput() {
    return output;
}
