// Adapted from following work :
//              App to phrase IMU data supplied via HID from a PS4 controller, and estimate the orientation of the controller
//              Written by Jack Halliday (jack.halliday12@imperial.ac.uk) for Igloo Vision LTD (http://www.igloovision.com/).


#pragma once
#include <External\HIDAPI\hidapi.h>
#include "Common/DataStructures/BitArray.h"

#define VENDOR_ID 1356		// The vendor & product ID of HID device to be used:
#define PRODUCT_ID 1476		//		(Currently Setup for PS4 controller)

#define ACC_FACTOR 9.8f / 8100.f		// No' of m/s2 per arb unit of acceleration

#define GYRO_FACTOR 1.f / 1024.f		// No of rad/s per arb unit of angular velocity
#define GYRO_ERROR 0.001f			// Estimated error on gyroscope measurement in rad/s

#define TIME_FACTOR 0.00125f / 188.f	// No' of secs per arb unit of time 

#define DS4_INPUT_REPORT_LENGTH 64
#define DS4_OUTPUT_REPORT_LENGTH 32

#define DEADZONE_SIZE 0.08f

// Indices for controller connected via USB
#define GX_INDEX 14		
#define GY_INDEX 16
#define GZ_INDEX 18
#define DX_INDEX 20
#define DY_INDEX 22
#define DZ_INDEX 24
#define FACE_BUTTONS_INDEX 5
#define OTHER_BUTTONS_INDEX 6
#define LEFT_ANALOG_X_INDEX 1
#define LEFT_ANALOG_Y_INDEX 2
#define RIGHT_ANALOG_X_INDEX 3
#define RIGHT_ANALOG_Y_INDEX 4

#define TIME_INDEX 11 //Index of the timestamp byte

#define SQUARE_BUTTON_MASK 0x10
#define CROSS_BUTTON_MASK 0x20
#define CIRCLE_BUTTON_MASK 0x40
#define TRIANGLE_BUTTON_MASK 0x80

#define L1_BUTTON_MASK 0x01
#define L2_BUTTON_MASK 0x04
#define L3_BUTTON_MASK 0x40
#define R1_BUTTON_MASK 0x02
#define R2_BUTTON_MASK 0x08
#define R3_BUTTON_MASK 0x80
#define OPTIONS_BUTTON_MASK 0x20
#define SHARE_BUTTON_MASK 0x10

#define LEFT_RUMBLE_WRITE_INDEX 5
#define RIGHT_RUMBLE_WRITE_INDEX 4
#define R_LED_WRITE_INDEX 6
#define G_LED_WRITE_INDEX 7
#define B_LED_WRITE_INDEX 8

//****************************   ALGO' SETTING    ****************************//
/*
//  We use Seb Madgwick's IMU sensor fusion algo' to couple the gro' and accelerometer
// sensor readings from the controller. For more detail, refer to Seb's website:
// http://www.x-io.co.uk/open-source-ahrs-with-x-imu/.
//
// My understanding of what Beta does is to dictate how much attention is paid to the
// accelerometer data in the sensor fusion process. This define specifies it's default
// value - it can also be changed via a set method within the class.
*/
#define BETA 0.5

enum ConnectionMode { USB, BLUETOOTH };

class Dualshock4
{
public:

    ~Dualshock4()
    {
        Close();
    }

    bool Setup();
    void Update();
    void Close();

    bool GetDS4Found() const { return m_hidDevice; }

    void SetLeftLFRumble(float zeroToOne) { m_writeBuffer[GetIndex(LEFT_RUMBLE_WRITE_INDEX)] = (blaU8)(zeroToOne * (float)0xFF); }
    void SetRightHFRumble(float zeroToOne) { m_writeBuffer[GetIndex(RIGHT_RUMBLE_WRITE_INDEX)] = (blaU8)(zeroToOne * (float)0xFF); }

    void SetLightBarColor(float r, float g, float b);

private:

    void ReportInputsToManager();

    void SetBeta(float beta) { m_beta = beta; }

    int GetRawGyroX() { return ReadInt16LE(m_reportBuffer, GetIndex(GX_INDEX)); }
    int GetRawGyroY() { return ReadInt16LE(m_reportBuffer, GetIndex(GY_INDEX)); }
    int GetRawGyroZ() { return ReadInt16LE(m_reportBuffer, GetIndex(GZ_INDEX)); }

    int GetRawAccelX() { return ReadInt16LE(m_reportBuffer, GetIndex(DX_INDEX)); }
    int GetRawAccelY() { return ReadInt16LE(m_reportBuffer, GetIndex(DY_INDEX)); }
    int GetRawAccelZ() { return ReadInt16LE(m_reportBuffer, GetIndex(DZ_INDEX)); }

    int   GetRawTimestamp() { return ReadInt16LE(m_reportBuffer, GetIndex(TIME_INDEX)); }
    float GetTimestamp() { return float(GetRawTimestamp() * TIME_FACTOR); }
    float GetTime() { return m_time; }

    float GetGyroX() { return (float)GetRawGyroX() * (float)GYRO_FACTOR; }
    float GetGyroY() { return (float)GetRawGyroY() * (float)GYRO_FACTOR; }
    float GetGyroZ() { return (float)GetRawGyroZ() * (float)GYRO_FACTOR; }

    float GetAccelX() { return (float)GetRawAccelX() * (float)ACC_FACTOR; }
    float GetAccelY() { return (float)GetRawAccelY() * (float)ACC_FACTOR; }
    float GetAccelZ() { return (float)GetRawAccelZ() * (float)ACC_FACTOR; }

    bool GetSquareButton() { return m_reportBuffer[GetIndex(FACE_BUTTONS_INDEX)] & SQUARE_BUTTON_MASK; }
    bool GetCircleButton() { return m_reportBuffer[GetIndex(FACE_BUTTONS_INDEX)] & CIRCLE_BUTTON_MASK; }
    bool GetTriangleButton() { return m_reportBuffer[GetIndex(FACE_BUTTONS_INDEX)] & TRIANGLE_BUTTON_MASK; }
    bool GetCrossButton() { return m_reportBuffer[GetIndex(FACE_BUTTONS_INDEX)] & CROSS_BUTTON_MASK; }

    bool GetL1Button() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & L1_BUTTON_MASK; }
    bool GetL2Button() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & L2_BUTTON_MASK; }
    bool GetL3Button() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & L3_BUTTON_MASK; }

    bool GetR1Button() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & R1_BUTTON_MASK; }
    bool GetR2Button() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & R2_BUTTON_MASK; }
    bool GetR3Button() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & R3_BUTTON_MASK; }

    bool GetOptionsButton() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & OPTIONS_BUTTON_MASK; }
    bool GetShareButton() { return m_reportBuffer[GetIndex(OTHER_BUTTONS_INDEX)] & SHARE_BUTTON_MASK; }

    void GetLeftJoystick(float& x, float& y)
    {
        x = m_reportBuffer[GetIndex(LEFT_ANALOG_X_INDEX)] / 128.f - 1.0f;
        y = -m_reportBuffer[GetIndex(LEFT_ANALOG_Y_INDEX)] / 128.f + 1.0f;

        x = abs(x) < DEADZONE_SIZE ? 0.f : x;
        y = abs(y) < DEADZONE_SIZE ? 0.f : y;
    }

    void GetRightJoystick(float& x, float& y)
    {
        x = m_reportBuffer[GetIndex(RIGHT_ANALOG_X_INDEX)] / 128.f - 1.0f;
        y = -m_reportBuffer[GetIndex(RIGHT_ANALOG_Y_INDEX)] / 128.f + 1.0f;

        x = abs(x) < DEADZONE_SIZE ? 0.f : x;
        y = abs(y) < DEADZONE_SIZE ? 0.f : y;
    }

    void GetDPad(int& x, int& y)
    {
        x = 0;
        y = 0;

        unsigned char bufValue = m_reportBuffer[GetIndex(FACE_BUTTONS_INDEX)];
        if (bufValue == 7 || bufValue == 0 || bufValue == 1)
        {
            y = 1;
        }
        if (bufValue == 1 || bufValue == 2 || bufValue == 3)
        {
            x = 1;
        }
        if (bufValue == 3 || bufValue == 4 || bufValue == 5)
        {
            y = -1;
        }
        if (bufValue == 5 || bufValue == 6 || bufValue == 7)
        {
            x = -1;
        }
    }

    int getArbPair(int index) { return ReadInt16LE(m_reportBuffer, index); }
    int getArbSingle(int index) { return ReadInt8(m_reportBuffer, index); }

private:

    ConnectionMode m_mode;

    int GetIndex(int index) { return index + (m_mode == USB ? 0 : 2); }

    int ReadInt16LE(unsigned char buffer[], int index);
    int ReadInt16LEUnsigned(unsigned char buffer[], int index);
    int ReadInt8(unsigned char buffer[], int index);

    void ReadReport();
    void WriteHID();
    void ResetWriteBuffer();
    void UpdateTimeStep();
    void UpdateAHRS(float dt, float gx, float gy, float gz, float ax, float ay, float az);

    hid_device* m_hidDevice = nullptr;
    unsigned char m_reportBuffer[256];
    unsigned char m_writeBuffer[256];
    int m_resultFlag;

    float m_previousTimestamp;
    float m_timeStep;
    float m_time;

    float m_quaternion[4];
    float m_beta;
};
