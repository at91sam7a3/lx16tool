#include <iostream>
#include <stdlib.h>
#include "lx16driver.h"
#include <algorithm>

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int main(int argc, char*argv[])
{
    //check for help
    if((argc==1) || (cmdOptionExists(argv, argv+argc, "-h"))  || (cmdOptionExists(argv, argv+argc, "--help")))
    {
        std::cout   << "This tool designed to be used from console with servos LX-16"   <<  std::endl;
        std::cout   << "avaliable keys: "   <<  std::endl;
        std::cout   <<  std::endl;
        std::cout   <<  "-id id, update id in range [0-253] , 254 reserved for broadcast" <<std::endl;//+
        std::cout   <<  "-p port, serial port name, default /dev/ttyUSB0"   <<  std::endl;//+
        std::cout   <<  "-s angle, raw angle to set,range [0-1000]" <<  std::endl;//+
        std::cout   <<  "-c angle, correct angle to set,range [-120 +120]" <<  std::endl;
        std::cout   <<  "-g , correct angle to get,range [-125 +125]" <<  std::endl; //+
        std::cout   <<  "-cs , save correct angle" <<  std::endl;
        std::cout   <<  "-r ,get current angle" <<  std::endl; //+
        std::cout   <<  "-I id, update id in range [0-254]" <<std::endl;//+
        std::cout   <<  std::endl;
        std::cout   << "Warning! update id with only one servo connected!"<<std::endl;
        return 0;

    }
    const char* device = "/dev/ttyUSB0";
    if(cmdOptionExists(argv, argv+argc, "-p"))
    {
        device = getCmdOption(argv, argv + argc, "-p");
    }

    lx16driver driver(device);
    if (driver.isOperational()==false)
    {
        std::cout<<"Something wrong with connection, do you have rights? is port correct? is device online?"<<std::endl;
        return 1;
    }

    //check for set id
    if(cmdOptionExists(argv, argv+argc, "-I"))
    {
        char * idNumberStr = getCmdOption(argv, argv + argc, "-I");
        if(!idNumberStr)
            return -2;
        int newIdToSet=atoi(idNumberStr);
        driver.RevriteId(newIdToSet);
        return 0;
    }

    int servoId=0;
    if(cmdOptionExists(argv, argv+argc, "-id"))
    {
        char * idNumberStr = getCmdOption(argv, argv + argc, "-id");
        if(!idNumberStr)
            return -2;
        servoId=atoi(idNumberStr);
        std::cout<<"Operating with id = " << servoId <<std::endl;
    }
    else
    {
        std::cout<<"Error: Please, set existing id !" << std::endl;
        return 1;
    }

    if(cmdOptionExists(argv, argv+argc, "-s"))
    {
        char * tmpstr = getCmdOption(argv, argv + argc, "-s");
        if(!tmpstr)
            return -2;
        int angle=atoi(tmpstr);
        std::cout << "Moving to raw angle " << angle <<std::endl;
        driver.ServoMoveTimeWrite(servoId,angle,100);
        sleep(2);
        return 0;
    }

    if(cmdOptionExists(argv, argv+argc, "-r"))
    {
        std::cout<<"Servo position raw read "
                 << driver.ServoPostionRead(servoId)
                 << std::endl;
        return 0;
    }

    if(cmdOptionExists(argv, argv+argc, "-g"))
    {
        std::cout<<"Servo correction position raw read "
                 << driver.ServoAdjustAngleGet(servoId)
                 << std::endl;
        return 0;
    }

    if(cmdOptionExists(argv, argv+argc, "-c"))
    {
        char * tmpstr = getCmdOption(argv, argv + argc, "-c");
        if(!tmpstr)
            return -2;
        int angle=atoi(tmpstr);
        std::cout << "setting correction angle " << angle <<std::endl;
        driver.ServoMoveTimeWrite(servoId,angle,100);
        sleep(2);
      //  return 0;
    }

    if(cmdOptionExists(argv, argv+argc, "-cs"))
    {
        std::cout << "saving correction angle" <<std::endl;
        driver.ServoAdjustAngleSave(servoId);
        sleep(2);
        return 0;
    }

 //   std::cout<<"Servo vin read "
 //           << driver.ServoVoltageRead(1)
 //           << std::endl;



    return 0;
}
