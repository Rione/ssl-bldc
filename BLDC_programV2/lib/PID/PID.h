#ifndef PID_H
#define PID_H
#include <mbed.h>
#include <Math.h>
class PID {
  private:
    /* data */
    float p, i, d; // PID gain
    float dt;      // interval of PID loop [s]
    Ticker ticker;
    Serial *pc;
    float lastError;
    float error;

    float output, _output;
    float limit;
    float min, max;

    float pTerm;
    float iTerm, integral;
    float dTerm;

    void compute();

  public:
    PID(float _p, float _i, float _d, float _dt, Serial *_pc);
    void setLimit(float limit);
    void setGain(float _p, float _i, float _d);
    void reset();
    void appendError(float _error);
    float getPID();
};

#endif