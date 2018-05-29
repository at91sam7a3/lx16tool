#ifndef LX16DRIVER_H
#define LX16DRIVER_H
#include "serialib.h"


class lx16driver
{
public:
    lx16driver(const char*, bool);
    ~lx16driver();
    bool isOperational();

    void RevriteId(int id);

  //  void ServoMoveStop(int id);
  //  void ServoMoveStart(int id);

    void ServoMoveTimeWrite(int id, int angle, int moveTime);
//    void ServoMoveTimeWriteWait(int id, int angle, int moveTime);

 //   void ServoAngleLimitWrite(int id, int minAngle, int maxAngle);
 //   void ServoAngleLimitRead(int id, int& minAngle, int& maxAngle);

//    void ServoVoltageLimitWrite(int id, int minVoltage, int maxVoltage);
//    void ServoVoltageLimitRead(int id, int& minVoltage, int& maxVoltage);

    int ServoPostionRead(int id);
    int ServoVoltageRead(int id);
 //   int ServoTemperatureRead(int id);

 //   int ServoLedWrite(int id, int state);


    int ServoAdjustAngleGet(int id);
    void ServoAdjustAngleSet(int id, char angle);
    void ServoAdjustAngleSave(int id);
private:
    char LobotCheckSum(char buf[]);

private:

    bool m_loopbackFix;
    serialib handle;
    bool operational;
};

#endif // LX16DRIVER_H
