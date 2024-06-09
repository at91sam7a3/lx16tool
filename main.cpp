#include "../lx16lib/lx16driver.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>

char *getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[])
{
    // check for help
    if ((argc == 1) || (cmdOptionExists(argv, argv + argc, "-h")) ||
        (cmdOptionExists(argv, argv + argc, "--help")))
    {
        std::cout << "This tool designed to be used from console with servos LX-16"
                  << std::endl;
        std::cout << "avaliable keys: " << std::endl;
        std::cout << std::endl;
        std::cout
            << "-id id, update id in range [0-253] , 254 reserved for broadcast"
            << std::endl; //+
        std::cout << "-p port, serial port name, default /dev/ttyUSB0"
                  << std::endl;                                                //+
        std::cout << "-s angle, raw angle to set,range [0-1000]" << std::endl; //+
        std::cout << "-c angle, correct angle to set,range [-120 +120]"
                  << std::endl;
        std::cout << "-g , correct angle to get,range [-125 +125]" << std::endl; //+
        std::cout << "-cs , save correct angle" << std::endl;
        std::cout << "-r ,get current angle" << std::endl;              //+
        std::cout << "-v ,get current voltage" << std::endl;            //+
        std::cout << "-I id, rewrite id in range [0-254]" << std::endl; //+
        std::cout << "-L for correct loop processing (for example RPi connecter "
                     "over UART)"
                  << std::endl;
        std::cout << std::endl;
        std::cout << "Warning! update id with only one servo connected!"
                  << std::endl;
        std::cout << "-interctive for 1)set ID to write 2) move to angle 3) tweak "
                     "correction"
                  << std::endl;
        std::cout << "-tune for 1)set ID 2) tweak correction" << std::endl;
        std::cout << "-info to get servo data" << std::endl;
        std::cout << "-lim to set servo limits 0..1000" << std::endl;
        return 0;
    }
    bool loopbackFix = false;
    if (cmdOptionExists(argv, argv + argc, "-L"))
    {
        loopbackFix = true;
    }
    const char *device = "/dev/ttyUSB0";
    if (cmdOptionExists(argv, argv + argc, "-p"))
    {
        device = getCmdOption(argv, argv + argc, "-p");
    }

    lx16driver driver(device, loopbackFix);
    if (driver.isOperational() == false)
    {
        std::cout << "Something wrong with connection, do you have rights? is port "
                     "correct? is device online?"
                  << std::endl;
        return 1;
    }

    if (cmdOptionExists(argv, argv + argc, "-interactive"))
    {
        std::cout << "Lets setup servo" << std::endl;
        std::cout << "Enter servo ID to set: ";
        int servoId;
        std::cin >> servoId;
        driver.RevriteId(servoId);
        std::cout << std::endl
                  << "Enter the angle to set: ";
        int angle;
        std::cin >> angle;
        driver.ServoMoveTimeWrite(servoId, angle, 100);
        int adjust = driver.ServoAdjustAngleGet(servoId);
        std::cout << std::endl
                  << "Time to set precise angle, current "<< adjust << std::endl;
        
        while (adjust != 999)
        {
            driver.ServoAdjustAngleSet(servoId, adjust);
            std::cout << "enter new angle to set(-127 to 127) or 999 to save"
                      << std::endl;
            std::cin >> adjust;
            std::cout << std::endl;
        }
        std::cout << "Done with servo " << servoId << std::endl;
        driver.ServoAdjustAngleSave(servoId);
    }

    if (cmdOptionExists(argv, argv + argc, "-all"))
    {
        std::cout<<"Checking connected servos"<<std::endl;
        for (int i =0 ; i<18; ++i)
        {
            std::cout<<"Servo #"<<i<<"...";
            int voltage=driver.ServoVoltageRead(i);
            if(voltage == 0)
            {
                std::cout<<"Error"<<std::endl;
                continue;
            }
            else
            {
                std::cout<<"OK"<<std::endl;
            }
            float volt = voltage / 1000;
            std::cout<<"Voltage = "<<volt<<std::endl;
            std::cout<<"Adjustments angle = " << driver.ServoAdjustAngleGet(i)<<std::endl;
            std::pair<int,int> bounds = driver.GetAngleLimits(i);
            std::cout<<"Limits "<<bounds.first<<" .. "<<bounds.second<<std::endl;
            std::cout<<"Current angle "<<driver.ServoPositionRead(i)<<std::endl;
            std::cout<<"Status = "<<(int)driver.GetServoErrorStatus(i)<<std::endl;
            
        }
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-tune"))
    {
        std::cout << "Lets setup servo" << std::endl;
        std::cout << "Enter servo ID: ";
        int servoId;
        std::cin >> servoId;
        driver.RevriteId(servoId);

        int angle = 500;

        driver.ServoMoveTimeWrite(servoId, angle, 100);
        std::cout << std::endl
                  << "Time to set precise angle, writing 0" << std::endl;
        int adjust = 0;
        while (adjust != 999)
        {
            driver.ServoAdjustAngleSet(servoId, adjust);
            std::cout << "enter new angle to set(-127 to 127) or 999 to save"
                      << std::endl;
            std::cin >> adjust;
            std::cout << std::endl;
        }
        std::cout << "Done with servo " << servoId << std::endl;
        driver.ServoAdjustAngleSave(servoId);
    }

    // check for set id
    if (cmdOptionExists(argv, argv + argc, "-I"))
    {
        char *idNumberStr = getCmdOption(argv, argv + argc, "-I");
        if (!idNumberStr)
            return -2;
        int newIdToSet = atoi(idNumberStr);
        driver.RevriteId(newIdToSet);
        return 0;
    }

    int servoId = 0;
    if (cmdOptionExists(argv, argv + argc, "-id"))
    {
        char *idNumberStr = getCmdOption(argv, argv + argc, "-id");
        if (!idNumberStr)
            return -2;
        servoId = atoi(idNumberStr);
        std::cout << "Operating with id = " << servoId << std::endl;
    }
    else
    {
        std::cout << "Error: Please, set existing id !" << std::endl;
        return 1;
    }

    if (cmdOptionExists(argv, argv + argc, "-info"))
    {
        std::cout << "**************************************" << std::endl;
        std::cout << "Data for servo id = " << servoId << std::endl;

        char c = driver.GetServoErrorStatus(servoId);
        switch (c)
        {
        case 0:
            std::cout << "No Alarms, OK" << std::endl;
            break;
        case 1:
            std::cout << "Over temperature!" << std::endl;
            break;
        case 2:
            std::cout << "Over Voltage" << std::endl;
            break;
        case 3:
            std::cout << "Over temp and voltage" << std::endl;
            break;
        case 4:
            std::cout << "Locked Rotor" << std::endl;
            break;
        case 5:
            std::cout << "Over temp and stalled" << std::endl;
            break;
        case 6:
            std::cout << "Over voltage and stalled" << std::endl;
            break;
        default:
            std::cout << "Something wrong with error code, not exist: "<<int(c) << std::endl;
        }

        int angle = driver.ServoPositionRead(servoId);

        int voltage = driver.ServoVoltageRead(servoId);

        int adjust = driver.ServoAdjustAngleGet(servoId);
        std::pair<int, int> limits = driver.GetAngleLimits(servoId);

        std::cout << "Angle = " << angle << std::endl;
        std::cout << "Voltage = " << voltage << std::endl;
        std::cout << "Adjust = " << adjust << std::endl;
        std::cout << "Limits= (" << limits.first << " : " << limits.second << ")" << std::endl;

        std::cout << "**************************************" << std::endl;
    }

    if (cmdOptionExists(argv, argv + argc, "-lim"))
    {
        std::cout << "updating limits " << std::endl;
        driver.SetAngleLimits(servoId, 0, 999);
        sleep(2);
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-s"))
    {
        char *tmpstr = getCmdOption(argv, argv + argc, "-s");
        if (!tmpstr)
            return -2;
        int angle = atoi(tmpstr);
        std::cout << "Moving to raw angle " << angle << std::endl;
        driver.ServoMoveTimeWrite(servoId, angle, 100);
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-r"))
    {
        std::cout << "Servo position raw read " << driver.ServoPositionRead(servoId)
                  << std::endl;
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-v"))
    {
        std::cout << "Voltage read " << driver.ServoVoltageRead(servoId)
                  << std::endl;
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-g"))
    {
        std::cout << "Servo correction position raw read "
                  << driver.ServoAdjustAngleGet(servoId) << std::endl;
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-c"))
    {
        char *tmpstr = getCmdOption(argv, argv + argc, "-c");
        if (!tmpstr)
            return -2;
        int angle = atoi(tmpstr);
        std::cout << "setting correction angle " << angle << std::endl;
        driver.ServoAdjustAngleSet(servoId, angle);
    }

    if (cmdOptionExists(argv, argv + argc, "-cs"))
    {
        std::cout << "saving correction angle" << std::endl;
        driver.ServoAdjustAngleSave(servoId);
        return 0;
    }

    return 0;
}
