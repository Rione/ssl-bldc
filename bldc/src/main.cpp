#include <Arduino.h>
#include <SimpleFOC.h>

// magnetic sensor instance - SPI
// MagneticSensorSPI sensor = MagneticSensorSPI(AS5048_SPI, 10);
MagneticSensorSPI sensor = MagneticSensorSPI(AS5048_SPI, D10);
InlineCurrentSense current_sense = InlineCurrentSense(0.01, 50.0, A0, A2);
// magnetic sensor instance - MagneticSensorI2C
// MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
// MagneticSensorAnalog sensor = MagneticSensorAnalog(A1, 14, 1020);

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(8);
BLDCDriver3PWM driver = BLDCDriver3PWM(D9, D5, D6, D8);

// Stepper motor & driver instance
// StepperMotor motor = StepperMotor(50);
// StepperDriver4PWM driver = StepperDriver4PWM(9, 5, 10, 6,  8);

// velocity set point variable
float target_velocity = 0;
// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char *cmd) { command.scalar(&target_velocity, cmd); }

void setup() {

    // initialise magnetic sensor hardware
    sensor.init();
    // link the motor to the sensor
    motor.linkSensor(&sensor);

    // for SimpleFOCShield v2.01/v2.0.2
    // current_sense.gain_b *= -1;
    // current_sense.skip_align = true;
    // motor.torque_controller = TorqueControlType::dc_current;
    // current sens init

    // current_sense.init();
    // driver config
    // power supply voltage [V]

    driver.voltage_power_supply = 20;
    driver.pwm_frequency = 100000;
    Serial.println(driver.pwm_frequency);
    driver.init();
    // link the motor and the driver
    motor.linkDriver(&driver);

    // set motion control loop to be used
    motor.controller = MotionControlType::velocity;

    // contoller configuration
    // default parameters in defaults.h

    // velocity PI controller parameters
    motor.PID_velocity.P = 0.1f;
    motor.PID_velocity.I = 1.0;
    motor.PID_velocity.D = 0;
    // default voltage_power_supply
    motor.voltage_limit = 7;

    motor.current_limit = 2.0;
    // jerk control using voltage voltage ramp
    // default value is 300 volts per sec  ~ 0.3V per millisecond
    motor.PID_velocity.output_ramp = 1000;

    // velocity low pass filtering
    // default 5ms - try different values to see what is the best.
    // the lower the less filtered
    motor.LPF_velocity.Tf = 0.01f;
    motor.velocity_limit = 100;
    // use monitoring with serial
    Serial.begin(2000000);
    // comment out if not needed needed
    motor.useMonitoring(Serial);

    // motor.linkCurrentSense(&current_sense);
    // initialize motor
    motor.init();
    // align sensor and start FOC
    motor.initFOC();

    // add target command T
    command.add('T', doTarget, "target velocity");

    Serial.println(F("Motor ready."));
    Serial.println(F("Set the target velocity using serial terminal:"));
    _delay(1000);
}

float fadeAmout = 0.002;
void Fading() {
    target_velocity += fadeAmout;
    if (target_velocity <= -300.0 || target_velocity >= 300.0) {
        fadeAmout = -fadeAmout;
    }
}

void loop() {
    Fading();
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

    // user communication
    command.run();
    Serial.println(sensor.getVelocity());
}
// #include <SimpleFOC.h>
// MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, D10);
// // alternative constructor (chipselsect, bit_resolution, angle_read_register, )
// // MagneticSensorSPI sensor = MagneticSensorSPI(10, 14, 0x3FFF);

// void setup() {
//     // monitoring port
//     Serial.begin(115200);

//     // initialise magnetic sensor hardware
//     sensor.init();

//     Serial.println("Sensor ready");
//     _delay(1000);
// }

// void loop() {
//     // iterative function updating the sensor internal variables
//     // it is usually called in motor.loopFOC()
//     // this function reads the sensor hardware and
//     // has to be called before getAngle nad getVelocity
//     sensor.update();
//     // display the angle and the angular velocity to the terminal
//     Serial.print(sensor.getAngle());
//     Serial.print("\t");
//     Serial.println(sensor.getVelocity());
// }
