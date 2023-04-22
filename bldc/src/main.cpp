/**
 *
 * Velocity motion control example
 * Steps:
 * 1) Configure the motor and magnetic sensor
 * 2) Run the code
 * 3) Set the target velocity (in radians per second) from serial terminal
 *
 *
 * By using the serial terminal set the velocity value you want to motor to obtain
 *
 */
#include <SimpleFOC.h>

// magnetic sensor instance - SPI
MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, 10);
// magnetic sensor instance - MagneticSensorI2C
// MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
// MagneticSensorAnalog sensor = MagneticSensorAnalog(A1, 14, 1020);

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(8);
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6);

// velocity set point variable
float target_velocity = 50;
// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char *cmd) { command.scalar(&target_velocity, cmd); }

void setup() {

    // initialise magnetic sensor hardware
    sensor.init();
    // link the motor to the sensor
    motor.linkSensor(&sensor);

    // driver config
    // power supply voltage [V]
    driver.voltage_power_supply = 15;
    driver.init();
    // link the motor and the driver
    motor.linkDriver(&driver);

    // set motion control loop to be used
    motor.controller = MotionControlType::velocity;

    // contoller configuration
    // default parameters in defaults.h

    // velocity PI controller parameters
    motor.PID_velocity.P = 0.2;
    motor.PID_velocity.I = 1;
    motor.PID_velocity.D = 0.000001;
    // default voltage_power_supply
    motor.voltage_limit = 5;
    // jerk control using voltage voltage ramp
    // default value is 300 volts per sec  ~ 0.3V per millisecond
    motor.PID_velocity.output_ramp = 1000;

    // velocity low pass filtering
    // default 5ms - try different values to see what is the best.
    // the lower the less filtered
    motor.LPF_velocity.Tf = 0.01f;

    // use monitoring with serial
    Serial.begin(2000000);
    // comment out if not needed
    motor.useMonitoring(Serial);

    // initialize motor
    motor.init();
    // align sensor and start FOC
    motor.initFOC();

    // add target command T
    command.add('T', doTarget, "target voltage");

    Serial.println(F("Motor ready."));
    Serial.println(F("Set the target velocity using serial terminal:"));
    _delay(1000);
}

void loop() {
    // main FOC algorithm function
    // the faster you run this function the better
    // Arduino UNO loop  ~1kHz
    // Bluepill loop ~10kHz
    motor.loopFOC();

    // Motion control function
    // velocity, position or voltage (defined in motor.controller)
    // this function can be run at much lower frequency than loopFOC() function
    // You can also use motor.move() and set the motor.target in the code
    motor.move(target_velocity);

    // function intended to be used with serial plotter to monitor motor variables
    // significantly slowing the execution down!!!!
    // motor.monitor();

    // user communicationT

    command.run();
}