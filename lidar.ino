// This sketch code is based on the RPLIDAR driver library provided by RoboPeak
#include <RPLidar.h>
// You need to create an driver instance
RPLidar lidar;
#define RPLIDAR_MOTOR 3 // The PWM pin for control the speed of RPLIDAR's motor.
// This pin should connected with the RPLIDAR's MOTOCTRL signal
//

int count;

//
void setup()
{
    Serial.begin(9600);
    // bind the RPLIDAR driver to the arduino hardware serial
    lidar.begin(Serial2);
    // set pin modes
    pinMode(RPLIDAR_MOTOR, OUTPUT);
    //
}
void loop()
{

    if (IS_OK(lidar.waitPoint()))
    {
        float distance = lidar.getCurrentPoint().distance; // distance value in mm unit
        float angle = lidar.getCurrentPoint().angle;       // anglue value in degree
        bool startBit = lidar.getCurrentPoint().startBit;  // whether this point is belong to a new scan
        byte quality = lidar.getCurrentPoint().quality;    // quality of the current measurement
        // perform data processing here...
        count++;
        if (count == 1000)
        {
            count = 0;
            Serial.print(x);
            Serial.print('/');
            Serial.println(y);
        }

        // gauche
        if (angle < 315 && angle > 225)
        {
            if (quality == 15)
            {
                float x = distance * cos(radians(angle));
                float y = distance * sin(radians(angle));
                Serial.print(x);
                Serial.print('/');
                Serial.println(y);
            }
        }

        // gauche
        if (angle < 360 && angle > 315 || angle < 40 && angle > 0)
        {
            if (quality == 15)
            {
                float x = distance * cos(radians(angle));
                float y = distance * sin(radians(angle));
                Serial.print(x);
                Serial.print('/');
                Serial.println(y);
            }
        }
    }
    else
    {
        analogWrite(RPLIDAR_MOTOR, 0); // stop the rplidar motor
        // try to detect RPLIDAR...
        rplidar_response_device_info_t info;
        if (IS_OK(lidar.getDeviceInfo(info, 100)))
        {
            // detected...
            lidar.startScan();
            // start motor rotating at max allowed speed
            analogWrite(RPLIDAR_MOTOR, 255);
            delay(1000);
        }
    }
}
