#include "Dualshock4.h"
#include <iostream>
#include <Engine/System/InputManager.h>

bool Dualshock4::Setup()
{
    m_hidDevice = hid_open(VENDOR_ID, PRODUCT_ID, NULL);
    if (!m_hidDevice)
    {
        //std::cout << "Unable to find a Controller\n";
    }
    else
    {
        ReadReport();
        m_previousTimestamp = GetTimestamp();
        m_reportBuffer[0] = 0x2;
        hid_get_feature_report(m_hidDevice, m_reportBuffer, DS4_INPUT_REPORT_LENGTH); // When you first blaPair a controller via bluetooth 

        if (m_resultFlag < 0)
        {
            //std::cout << "unable to sent report\n";
        }

        m_time = 0.;												  // starts in low energy mode where IMU data not sent
        m_quaternion[0] = 1.0;										  // requesting feature report puts it into full mode. Source:   
        m_quaternion[1] = 0.;										  // (https://github.com/torvalds/linux/blob/master/drivers/hid/hid-sony.c)
        m_quaternion[2] = 0.;
        m_quaternion[3] = 0.;
        m_beta = BETA;

        ResetWriteBuffer();
    }
    return true;
}

void Dualshock4::Update() 
{
	if (m_hidDevice) 
    {
		ReadReport();

		UpdateTimeStep();
        
        ReportInputsToManager();

        //ControllerTest();

        if(m_mode == USB)
            WriteHID();

		//UpdateAHRS(m_timeStep, ax, ay, az, gx, gy, gz);
	}
    else
    {
        std::cout << "No Hid Device\n";
    }
}

void Dualshock4::WriteHID()
{
    if (m_hidDevice)
    {
        hid_write(m_hidDevice, m_writeBuffer, sizeof(m_writeBuffer));

        memset(m_writeBuffer, 0, 11);

        ResetWriteBuffer();
    }
}

void Dualshock4::ResetWriteBuffer()
{
    memset(m_writeBuffer, 0x00, DS4_OUTPUT_REPORT_LENGTH);

    if (m_mode == USB)
    {
        m_writeBuffer[0] = 0x05;
        m_writeBuffer[1] = 0xff;
    }
    else
    {
        m_writeBuffer[0] = 0x11;
        m_writeBuffer[1] = 0x80;
        m_writeBuffer[2] = 0xff;
    }
}

void Dualshock4::Close()
{
	if (m_hidDevice)
    {
		hid_close(m_hidDevice);
		hid_exit();
	}
}

void Dualshock4::SetLightBarColor(float r, float g, float b)
{
    m_writeBuffer[R_LED_WRITE_INDEX] = 0xFF * r;
    m_writeBuffer[G_LED_WRITE_INDEX] = 0xFF * g;
    m_writeBuffer[B_LED_WRITE_INDEX] = 0xFF * b;
}

void Dualshock4::ReportInputsToManager()
{
    float ax, ay, az, gx, gy, gz;
    ax = GetAccelX();
    ay = GetAccelY();
    az = GetAccelZ();

    gx = GetGyroX();
    gy = GetGyroY();
    gz = GetGyroZ();

    blaVec2 leftAnalog;
    blaVec2 rightAnalog;

    GetLeftJoystick(leftAnalog[0], leftAnalog[1]);
    GetRightJoystick(rightAnalog[0], rightAnalog[1]);

    BLAengine::InputStateSetter::SetGamepadSticks(leftAnalog, rightAnalog);

    int dpad[2];
    GetDPad(dpad[0], dpad[1]);

    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_DOWN, 0.f, GetCrossButton());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_LEFT, 0.f, GetSquareButton());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_UP, 0.f, GetTriangleButton());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_RIGHT, 0.f, GetCircleButton());

    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_DPAD_UP, 0.f, dpad[1] == 1);
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_DPAD_DOWN, 0.f, dpad[1] == -1);
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_RIGHT, 0.f, dpad[0] == -1);
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_FACEBUTTON_LEFT, 0.f, dpad[0] == -1);


    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_LEFT_SHOULDER, 0.f, GetL1Button());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_LEFT_TRIGGER, 0.f, GetL2Button());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_LEFT_STICK, 0.f, GetL3Button());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_RIGHT_SHOULDER, 0.f, GetR1Button());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_RIGHT_TRIGGER, 0.f, GetR2Button());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_RIGHT_STICK, 0.f, GetR3Button());

    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_SELECT, 0.f, GetSquareButton());
    BLAengine::InputStateSetter::SetGamepadButton(0, BLAengine::BLAGamepadButtons::BLA_GAMEPAD_START, 0.f, GetOptionsButton());
}

int Dualshock4::ReadInt16LE(unsigned char buffer[], int index)
{
	unsigned short highByte = buffer[index];	//Byte order swapped in BLADualshock4 io buffer
	unsigned short lowByte = buffer[index++];
	short combined = lowByte << 8 | highByte ;
	return int(combined);
}

int Dualshock4::ReadInt16LEUnsigned(unsigned char buffer[], int index)
{
	unsigned short highByte = buffer[index];
	unsigned short lowByte = buffer[index++];
	unsigned short combined = lowByte << 8 | highByte;
	return int(combined);
}

int Dualshock4::ReadInt8(unsigned char buffer[], int index)
{
	return int(buffer[index]);
}

void Dualshock4::ReadReport()
{
	m_resultFlag = 0;
	while (m_resultFlag == 0) 
    {
		m_resultFlag = hid_read(m_hidDevice, m_reportBuffer, sizeof(m_reportBuffer));
        if (m_resultFlag < 0)
        {
            m_hidDevice = nullptr;
        }
        else
        {
            m_mode = USB;
            if(m_reportBuffer[0] == 0x11 && m_reportBuffer[1] == 0xc0 && m_reportBuffer[2] == 0x00)
            {
                m_mode = BLUETOOTH;
            }
        }
    }
}

void Dualshock4::UpdateTimeStep()
{
	float timestamp = GetTimestamp();
	float dif = timestamp - m_previousTimestamp;
	if (dif > 0) { m_timeStep = dif; }
	else { m_timeStep = 0.00125; }
	m_previousTimestamp = timestamp;
	m_time += m_timeStep;
}

void Dualshock4::UpdateAHRS(float dt, float gx, float gy, float gz, float ax, float ay, float az)
{
	/*
	// This is an implementation of Seb Madgwick's AHRS filter.
	// (http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/)
	// TODO: This assumes that the initial orientation of the controller
	// is such that gravity points in the -z direction, but in reality the 
	// controller is usually initially oriented so that gravity points in the 
	// -y direction. This is why the sphere in the test app processes pi/2 radians
	// about the x axis in the ~5 secs after the test app is started. It's not ideal 
	// and could be fixed, either by re-writing the algo' with the correct 
	// initial condition, or (possibly) by altering the axis order of the controller.
	*/

	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-m_quaternion[1] * gx - m_quaternion[2] * gy - m_quaternion[3] * gz);
	qDot2 = 0.5f * (m_quaternion[0] * gx + m_quaternion[2] * gz - m_quaternion[3] * gy);
	qDot3 = 0.5f * (m_quaternion[0] * gy - m_quaternion[1] * gz + m_quaternion[3] * gx);
	qDot4 = 0.5f * (m_quaternion[0] * gz + m_quaternion[1] * gy - m_quaternion[2] * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = 1. / sqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * m_quaternion[0];
		_2q1 = 2.0f * m_quaternion[1];
		_2q2 = 2.0f * m_quaternion[2];
		_2q3 = 2.0f * m_quaternion[3];
		_4q0 = 4.0f * m_quaternion[0];
		_4q1 = 4.0f * m_quaternion[1];
		_4q2 = 4.0f * m_quaternion[2];
		_8q1 = 8.0f * m_quaternion[1];
		_8q2 = 8.0f * m_quaternion[2];
		q0q0 = m_quaternion[0] * m_quaternion[0];
		q1q1 = m_quaternion[1] * m_quaternion[1];
		q2q2 = m_quaternion[2] * m_quaternion[2];
		q3q3 = m_quaternion[3] * m_quaternion[3];

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * m_quaternion[1] - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * m_quaternion[2] + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * m_quaternion[3] - _2q1 * ax + 4.0f * q2q2 * m_quaternion[3] - _2q2 * ay;
		recipNorm = 1. / sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= m_beta * s0;
		qDot2 -= m_beta * s1;
		qDot3 -= m_beta * s2;
		qDot4 -= m_beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	m_quaternion[0] += qDot1 * dt;
	m_quaternion[1] += qDot2 * dt;
	m_quaternion[2] += qDot3 * dt;
	m_quaternion[3] += qDot4 * dt;

	// Normalise quaternion
	recipNorm = 1. / sqrt(m_quaternion[0] * m_quaternion[0] + m_quaternion[1] * m_quaternion[1] + m_quaternion[2] * m_quaternion[2] + m_quaternion[3] * m_quaternion[3]);
	m_quaternion[0] *= recipNorm;
	m_quaternion[0] *= recipNorm;
	m_quaternion[2] *= recipNorm;
	m_quaternion[3] *= recipNorm;
}

