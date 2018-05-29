#include "lx16driver.h"
#include <iostream>
#include <stdint.h>
#define GET_LOW_BYTE(A) (uint8_t)((A))
//Macro function  get lower 8 bits of A
#define GET_HIGH_BYTE(A) (uint8_t)((A) >> 8)
//Macro function  get higher 8 bits of A
#define BYTE_TO_HW(A, B) ((((uint16_t)(A)) << 8) | (uint8_t)(B))
//Macro Function  put A as higher 8 bits   B as lower 8 bits   which amalgamated into 16 bits integer

#define LOBOT_SERVO_FRAME_HEADER         0x55
#define LOBOT_SERVO_MOVE_TIME_WRITE      1
#define LOBOT_SERVO_MOVE_TIME_READ       2
#define LOBOT_SERVO_MOVE_TIME_WAIT_WRITE 7
#define LOBOT_SERVO_MOVE_TIME_WAIT_READ  8
#define LOBOT_SERVO_MOVE_START           11
#define LOBOT_SERVO_MOVE_STOP            12
#define LOBOT_SERVO_ID_WRITE             13
#define LOBOT_SERVO_ID_READ              14
#define LOBOT_SERVO_ANGLE_OFFSET_ADJUST  17
#define LOBOT_SERVO_ANGLE_OFFSET_WRITE   18
#define LOBOT_SERVO_ANGLE_OFFSET_READ    19
#define LOBOT_SERVO_ANGLE_LIMIT_WRITE    20
#define LOBOT_SERVO_ANGLE_LIMIT_READ     21
#define LOBOT_SERVO_VIN_LIMIT_WRITE      22
#define LOBOT_SERVO_VIN_LIMIT_READ       23
#define LOBOT_SERVO_TEMP_MAX_LIMIT_WRITE 24
#define LOBOT_SERVO_TEMP_MAX_LIMIT_READ  25
#define LOBOT_SERVO_TEMP_READ            26
#define LOBOT_SERVO_VIN_READ             27
#define LOBOT_SERVO_POS_READ             28
#define LOBOT_SERVO_OR_MOTOR_MODE_WRITE  29
#define LOBOT_SERVO_OR_MOTOR_MODE_READ   30
#define LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE 31
#define LOBOT_SERVO_LOAD_OR_UNLOAD_READ  32
#define LOBOT_SERVO_LED_CTRL_WRITE       33
#define LOBOT_SERVO_LED_CTRL_READ        34
#define LOBOT_SERVO_LED_ERROR_WRITE      35
#define LOBOT_SERVO_LED_ERROR_READ       36

lx16driver::lx16driver(const char* device, bool loopFix)
    :operational(false)
    ,m_loopbackFix(loopFix)
{
    int Ret = handle.Open(device,115200);
    if (Ret!=1) {
        std::cerr<<"ERROR: Comport not availabe!"<<std::endl;
        return;
    }
    operational = true;
    handle.FlushReceiver();
}

lx16driver::~lx16driver()
{
     handle.Close();
}

bool lx16driver::isOperational()
{
    return operational;
}

void lx16driver::RevriteId(int id)
{
    char buf[10];
    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = 254;//old id
    buf[3] = 4;
    buf[4] = LOBOT_SERVO_ID_WRITE;
    buf[5] = id;//new id
    buf[6] = LobotCheckSum(buf);
    handle.Write(buf,7);
}

void lx16driver::ServoMoveTimeWrite(int id, int position, int time)
{
    char buf[10];
    if(position < 0)
      position = 0;
    if(position > 1000)
      position = 1000;
    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = id;
    buf[3] = 7;
    buf[4] = LOBOT_SERVO_MOVE_TIME_WRITE;
    buf[5] = GET_LOW_BYTE(position);
    buf[6] = GET_HIGH_BYTE(position);
    buf[7] = GET_LOW_BYTE(time);
    buf[8] = GET_HIGH_BYTE(time);
    buf[9] = LobotCheckSum(buf);
    handle.Write(buf,10);
}



int lx16driver::ServoPostionRead(int id)
{
    int ret;
    char buf[16];
    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = id;
    buf[3] = 3;
    buf[4] = LOBOT_SERVO_POS_READ;
    buf[5] = LobotCheckSum(buf);
    handle.Write(buf,6);
    handle.FlushReceiver();
    if(m_loopbackFix)//next line fix echo
        handle.ReadString(buf,0,6,100);
    // Read a string from the serial device
    ret=handle.ReadString(buf,'\n',16,200);                                // Read a maximum of 128 characters with a timeout of 5 seconds
    char crc = LobotCheckSum(buf);                                                                        // The final character of the string must be a line feed ('\n')
    if(buf[3]!=5 || buf[4]!=28)
    {
        std::cerr<<"Comminication error!"<<std::endl;
        return 0;
    }
    if(crc != buf[7])
    {
        std::cerr<<"CRC error"<<std::endl;
        return 0;
    }
    // Close the connection with the device
    ret = BYTE_TO_HW(buf[6], buf[5]);
    return ret;
}

int lx16driver::ServoVoltageRead(int id)
{
    int ret;
    char buf[16];

    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = id;
    buf[3] = 3;
    buf[4] = LOBOT_SERVO_VIN_READ;
    buf[5] = LobotCheckSum(buf);

    handle.Write(buf,6);
    handle.FlushReceiver();
    if(m_loopbackFix)//next line fix echo
        handle.ReadString(buf,0,6,100);
    for(int i=0;i<16;++i) buf[i]=-5;
    // Read a string from the serial device
    ret=handle.ReadString(buf,'\n',16,100);                                // Read a maximum of 128 characters with a timeout of 5 seconds
    char crc = LobotCheckSum(buf);                                                                        // The final character of the string must be a line feed ('\n')
    if(buf[3]!=5 || buf[4]!=LOBOT_SERVO_VIN_READ)
    {
        std::cerr<<"Comminication error!"<<std::endl;
        return 0;
    }
    if(crc != buf[7])
    {
        std::cerr<<"CRC error"<<std::endl;
        return 0;
    }
    // Close the connection with the device
    ret = BYTE_TO_HW(buf[6], buf[5]);
    return ret;
}

int lx16driver::ServoAdjustAngleGet(int id)
{
    int ret;
    char buf[16];
    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = id;
    buf[3] = 3;
    buf[4] = LOBOT_SERVO_ANGLE_OFFSET_READ;
    buf[5] = LobotCheckSum(buf);
    handle.Write(buf,6);
    handle.FlushReceiver();
    if(m_loopbackFix)//next line fix echo
        handle.ReadString(buf,0,6,100);
    for(int i=0;i<16;++i) buf[i]=-5;
    // Read a string from the serial device
    ret=handle.ReadString(buf,'\n',16,100);                                // Read a maximum of 128 characters with a timeout of 5 seconds
    char crc = LobotCheckSum(buf);                                                                        // The final character of the string must be a line feed ('\n')
    if(buf[3]!=4 || buf[4]!=LOBOT_SERVO_ANGLE_OFFSET_READ)
    {
        std::cerr<<"Comminication error!"<<std::endl;
        return 0;
    }
    if(crc != buf[6])
    {
        std::cerr<<"CRC error"<<std::endl;
        return 0;
    }
    // Close the connection with the device
    ret = (int8_t)( buf[5]);
    return ret;
}

void lx16driver::ServoAdjustAngleSave(int id)
{
    char buf[10];
    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = id;
    buf[3] = 4;
    buf[4] = LOBOT_SERVO_ANGLE_OFFSET_WRITE;
    buf[5] = LobotCheckSum(buf);
    handle.Write(buf, 6);
}

void lx16driver::ServoAdjustAngleSet(int id, char angle)
{
    char buf[10];
    buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
    buf[2] = id;
    buf[3] = 4;
    buf[4] = LOBOT_SERVO_ANGLE_OFFSET_ADJUST;
    buf[5] = (unsigned char)(angle);
    buf[6] = LobotCheckSum(buf);
    handle.Write(buf, 7);
}

char lx16driver::LobotCheckSum(char buf[])
{
    char i;
    uint16_t temp = 0;
    for (i = 2; i < buf[3] + 2; i++) {
        temp += buf[i];
    }
    temp = ~temp;
    i = (char)temp;
    return i;
}

