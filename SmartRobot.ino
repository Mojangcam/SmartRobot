#include <PRIZM.h>
#include <time.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

#define FRONT_LEFT_LINE_SENSOR_PIN   2
#define FRONT_RIGHT_LINE_SENSOR_PIN  3
#define CENTER_LEFT_LINE_SENSOR_PIN  4
#define CENTER_RIGHT_LINE_SENSOR_PIN 5
#define DEFAULT_MOTOR_SPEED 75
#define YELLOW_PILLAR_COLOR_ID 4

PRIZM prizm;
EXPANSION exc;
HUSKYLENS huskylens;

// Block_Position
int g_blueBlockPositionX;
int g_blueBlockPositionY;
int g_redBlockPositionX;
int g_redBlockPositionY;
int g_greenBlockPositionX;
int g_greenBlockPositionY;

// Pillar_position
int g_bluePillarPositionX;
int g_bluePillarPositionY;
int g_redPillarPositionX;
int g_redPillarPositionY;
int g_greenPillarPositionX;
int g_greenPillarPositionY;
int g_yellowPillarPositionX;
int g_yellowPillarPositionY;

// Robot_Currunt_Position
int g_currentPositionX = 0;
int g_currentPositionY = 0;

// Robot Is Turned 0 = Not, 1 = Right, 2 = Left
int g_is_Turned = 0;

// Robot direction to look front : 1 back : 0
int g_directionToLook = 0;

// Nearby Block Position
int g_nearbyBlockPositionX = 0;
int g_nearbyBlockPositionY = 0;

int rec = 0;
int rec2 = 0;

// Robot spin counter
int g_robotSpinCounter = 0;

// Huskylens
void huskylensStarting();
void huskylensPrintID(HUSKYLENSResult result);
void allBlockScan();

void colorRecognition(int currentPositionX, int curruntPositionY);

unsigned long g_asyncStartTimeMsec, g_asyncEndTimeMsec;
int g_directionToLook;

// Robot Move Control Functions
void servoMove();
void gotoPositionX(int setPositionX);
void gotoPositionY(int setPositionY);
void blockPickup();
void blockDrop();
void forwardMove(int moveDelayToStop = 0,
                 int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
                 int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
                 int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
                 int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED);

void backMove(int moveDelayToStop = 0,
              int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
              int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED);

void rightMove(int moveDelayToStop = 0,
               int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
               int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
               int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
               int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED);

void leftMove(int moveDelayToStop = 0,
              int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
              int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED);

void stopMove();
void startMove(int setMotorSpeed = DEFAULT_MOTOR_SPEED);
void turnRight(int setDelayTime = 0, int setAngle = 0);
void turnLeft(int setDelayTime = 0, int setAngle = 0);
void gotoColorBlock(int colorBlockID = 0);
void gotoColorPillar(int colorPillarID = 0);
int getLineSensorValue(int lineSensorPinNum);
void showLineSensorStatus();
void resetPosition();
// End Robot Control Functions

// Get Block Position
void getPositionXToBlockID(int colorID);
void getPositionYToBlockID(int colorID);

// Get Pillar Position
void getPositionXToPillarID(int colorID);
void getPositionYToPillarID(int colorID);

// NEW kj
int g_beforePillarX;
int g_beforePillarY;
int g_is_blockCatch = 0;
int g_missionSuccess = 0;
int g_firstBlockX = 0;
int g_firstBlockY = 0;
int g_lastBlockX = 0;
int g_lastBlockY = 0;

int First_ch = 0;
int target_color = 0;
int RGB_cyl = 0;
int startSpeed = 50;

int go_position_x, go_position_y;
int nearbyBlockColorID = 0;  // this variable is nearby block color ID [ 1 = Red, 2 = Green, 3 = Blue, 4 = Yellow]
int nearbyPillarColorID = 0; // this variable is nearby pillar color ID  [ 1 = Red, 2 = Green, 3 = Blue, 4 = Yellow ]
int beforeColorID = 0;
int Y = 0;

void pickup_block(int setDelayTime = 0)
{
    prizm.setMotorPower(2, -15);
    delay(setDelayTime);
    prizm.setMotorPower(2, 0);
}
// END kj

// kj Edited

void setup()
{
    prizm.PrizmBegin();
    prizm.setServoSpeed(1, 25);
    prizm.resetEncoder(1);
    Serial.begin(9600);

    prizm.setServoPosition(1, 0);

    huskylensStarting();
    startMove(75);
    allBlockScan();
    g_currentPositionX = 1;
}

void loop()
{
    //  가장 가까운 값 찾는 것
    for (int i = 2; i < 6; i++)
    {
        if (g_redBlockPositionY == i ||
            g_greenBlockPositionY == i ||
            g_blueBlockPositionY == i)
        {
            go_position_y = i;
            if (g_redBlockPositionY == i)
            {
                nearbyBlockColorID = 1;
            } // 가장 가까운 값이 빨간색이면
            else if (g_greenBlockPositionY == i)
            {
                nearbyBlockColorID = 2;
            }
            else if (g_blueBlockPositionY == i)
            {
                nearbyBlockColorID = 3;
            }
            if (nearbyBlockColorID != 0)
            {
                break;
            }
        }
    }
    if (nearbyBlockColorID == 1)
    {
        if (g_redBlockPositionY == g_greenPillarPositionY &&
            g_redBlockPositionX == g_greenPillarPositionX)
        {
            nearbyPillarColorID = 2;
        }
        else if (g_redBlockPositionY == g_bluePillarPositionY &&
                 g_redBlockPositionX == g_bluePillarPositionX)
        {
            nearbyPillarColorID = 3;
        }
    }
    else if (nearbyBlockColorID == 2)
    {
        if (g_greenBlockPositionY == g_redPillarPositionY &&
            g_greenBlockPositionX == g_redPillarPositionX)
        {
            nearbyPillarColorID = 1;
        }
        else if (g_greenBlockPositionY == g_bluePillarPositionY &&
                 g_greenBlockPositionX == g_bluePillarPositionX)
        {
            nearbyPillarColorID = 3;
        }
    }
    else if (nearbyBlockColorID == 3)
    {
        if (g_blueBlockPositionY == g_redPillarPositionY &&
            g_blueBlockPositionX == g_redPillarPositionX)
        {
            nearbyPillarColorID = 1;
        }
        else if (g_blueBlockPositionY == g_greenPillarPositionY &&
                 g_blueBlockPositionX == g_greenPillarPositionX)
        {
            nearbyPillarColorID = 2;
        }
    }
    Serial.print(nearbyPillarColorID);
    gotoColorBlock(nearbyBlockColorID); // 레드로 가
    blockPickup();                      // 찝어
    if (getPositionYToPillarID(nearbyPillarColorID) == g_yellowPillarPositionY &&
        g_yellowPillarPositionY == g_currentPositionY)
    {
        if (Y == 0)
        {
            turnRight(0, 180);
            blockDrop();
            beforeColorID = nearbyBlockColorID;
            Y = 1;
        }
    }
    else
    {
        resetPosition();
        if (Y == 0)
        {
            gotoColorPillar(YELLOW_PILLAR_COLOR_ID);
            blockDrop();
            beforeColorID = nearbyBlockColorID;
            Y = 1;
            resetPosition();
        }
    }
    sequenceBlockOrganize(nearbyPillarColorID, 1); // color1 = blue color = red beforeColorID = red
    sequenceBlockOrganize(beforeColorID, 1);
    sequenceBlockOrganize(beforeColorID, 0);
    sequenceBlockOrganize(nearbyPillarColorID, 1);
    sequenceBlockOrganize(YELLOW_PILLAR_COLOR_ID, 1);
    sequenceBlockOrganize(nearbyBlockColorID, 0);
    gotoPositionY(1);
    forwardMove(4000);
    Right_Move(2000);
    Stop_Move();
}

void sequenceBlockOrganize(int moveToColorID, int is_Block)
{
    if (is_Block)
    {
        gotoColorBlock(moveToColorID);
        blockPickup();
        resetPosition();
    }
    else if (!is_Block)
    {
        gotoColorPillar(moveToColorID);
        blockPickup();
        resetPosition();
    }
}

int getPositionXToBlockID(int colorID)
{
    if (colorID == 1)
    {
        return g_redBlockPositionX;
    }
    else if (colorID == 2)
    {
        return g_greenBlockPositionX;
    }
    else if (colorID == 3)
    {
        return g_blueBlockPositionX;
    }
}

int getPositionYToBlockID(int colorID)
{
    if (colorID == 1)
    {
        return g_redBlockPositionY;
    }
    else if (colorID == 2)
    {
        return g_greenBlockPositionY;
    }
    else if (colorID == 3)
    {
        return g_blueBlockPositionY;
    }
}

int getPositionXToPillarID(int colorID)
{
    if (colorID == 1)
    {
        return g_redPillarPositionX;
    }
    else if (colorID == 2)
    {
        return g_greenPillarPositionX;
    }
    else if (colorID == 3)
    {
        return g_bluePillarPositionX;
    }
    else if (colorID == 4)
    {
        return g_yellowPillarPositionX;
    }
}

int getPositionYToPillarID(int colorID)
{
    if (colorID == 1)
    {
        return g_redPillarPositionY;
    }
    else if (colorID == 2)
    {
        return g_greenPillarPositionY;
    }
    else if (colorID == 3)
    {
        return g_bluePillarPositionY;
    }
    else if (colorID == 4)
    {
        return g_yellowPillarPositionY;
    }
}

void resetPosition()
{
    if (g_is_Turned == 1)
    {
        if (g_currentPositionY == 5 &&
                g_directionToLook == 0 ||
            g_currentPositionY == 2 &&
                g_directionToLook == 1)
        {
            turnLeft(0, 90);
            if (g_directionToLook == 1)
            {
                g_directionToLook = 0;
            }

            else if (g_directionToLook == 0)
            {
                g_directionToLook = 1;
            }
        }
        else
        {
            turnRight(0, 90);
        }
    }
    else if (g_is_Turned == 2)
    {
        if (g_currentPositionY == 2 &&
                g_directionToLook == 1 ||
            g_currentPositionY == 5 &&
                g_directionToLook == 0)
        {
            turnRight(0, 90);
            if (g_directionToLook == 1)
            {
                g_directionToLook = 0;
            }
            else if (g_directionToLook == 0)
            {
                g_directionToLook = 1;
            }
        }
        else
        {
            turnLeft(0, 90);
        }
    }
}

void gotoPositionX(int setPositionX)
{
    if (setPositionX >= 0 && setPositionX < 3)
    {
        if (setPositionX == 0)
        {
            if (g_directionToLook == 0)
            {
                turnLeft(0, 90);
                g_is_Turned = 1;
            }
            if (g_directionToLook == 1)
            {
                turnRight(0, 90);
                g_is_Turned = 2;
            }
            g_currentPositionX = 0;
        }
        else if (setPositionX == 2)
        {
            if (g_directionToLook == 1)
            {
                turnLeft(0, 90);
                g_is_Turned = 1;
            }
            if (g_directionToLook == 0)
            {
                turnRight(0, 90);
                g_is_Turned = 2;
            }
        }
        g_currentPositionX = 2;
    }
}

void gotoColorBlock(int colorID = 0)
{
    if (colorID != 0)
    {
        switch (colorID)
        {
        case 1:
            gotoPositionY(g_redBlockPositionY);
            gotoPositionX(g_redBlockPositionX); // 바라보기 까지만 함
            break;
        case 2:
            gotoPositionY(g_greenBlockPositionY);
            gotoPositionX(g_greenBlockPositionX);
            break;
        case 3:
            gotoPositionY(g_blueBlockPositionY);
            gotoPositionX(g_blueBlockPositionX);
            break;
        }
    }
}

void gotoColorPillar(int colorID = 0)
{
    if (colorID != 0)
    {
        switch (colorID)
        {
        case 1:
            gotoPositionY(g_redPillarPositionY);
            gotoPositionX(g_redPillarPositionX); // 바라보기 까지만 함
            break;
        case 2:
            gotoPositionY(g_greenPillarPositionY);
            gotoPositionX(g_greenPillarPositionX);
            break;
        case 3:
            gotoPositionY(g_bluePillarPositionY);
            gotoPositionX(g_bluePillarPositionX);
            break;

        case 4:
            gotoPositionY(g_yellowPillarPositionY);
            gotoPositionX(g_yellowPillarPositionX);
            break;
        }
    }
}

void allBlockScan()
{
    unsigned long scan_start, scan_end;
    Serial.println("Move 2");
    gotoPositionY(2);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        getcol(g_currentPositionX, g_currentPositionY);
    }
    Serial.println("Move 3");
    gotoPositionY(3);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }
    Serial.println("Move 4");
    gotoPositionY(4);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }
    Serial.println("Move 5");
    gotoPositionY(5);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }
    turnRight(0, 180);
    forwardMove();
    g_directionToLook = 1;
    g_currentPositionX = 2;
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }
    Serial.println("Move 4");
    gotoPositionY(4);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }
    Serial.println("Move 3");
    gotoPositionY(3);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }
    Serial.println("Move 2");
    gotoPositionY(2);
    delay(500);
    scan_start = millis();
    while (1)
    {
        scan_end = millis();
        if (scan_end - scan_start > 500)
        {
            break;
        }
        colorRecognition(g_currentPositionX, g_currentPositionY);
    }

    Serial.println(String() + F("red block : ") + g_redBlockPositionX + F(", ") + g_redBlockPositionY);
    Serial.println(String() + F("red cyl : ") + g_redPillarPositionX + F(", ") + g_redPillarPositionY);
    Serial.println(String() + F("green block : ") + g_greenBlockPositionX + F(", ") + g_greenBlockPositionY);
    Serial.println(String() + F("green cyl : ") + g_greenPillarPositionX + F(", ") + g_greenPillarPositionY);
    Serial.println(String() + F("blue block : ") + g_blueBlockPositionX + F(", ") + g_blueBlockPositionY);
    Serial.println(String() + F("blue cyl : ") + g_bluePillarPositionX + F(", ") + g_bluePillarPositionY);
    Serial.println(String() + F("Yellow cyl : ") + g_yellowPillarPositionX + F(", ") + g_yellowPillarPositionY);
}

void rightMove(int delay_time = 0,
               int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
               int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
               int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
               int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED)
{
    exc.setMotorSpeeds(1, -rightTopMotorSpeed, RightBottomMotorSpeed);
    exc.setMotorSpeeds(2, RightBottomMotorSpeed, -leftTopMotorSpeed);
    if (delay_time != 0)
    {
        delay(delay_time);
        exc.setMotorSpeeds(1, 0, 0);
        exc.setMotorSpeeds(2, 0, 0);
    }
}

void backMove(int delay_time = 0,
              int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
              int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED)
{
    exc.setMotorSpeeds(1, -rightTopMotorSpeed, -RightBottomMotorSpeed);
    exc.setMotorSpeeds(2, RightBottomMotorSpeed, leftTopMotorSpeed);
    if (delay_time != 0)
    {
        delay(delay_time);
        exc.setMotorSpeeds(1, 0, 0);
        exc.setMotorSpeeds(2, 0, 0);
    }
}

void leftMove(int delay_time = 0,
              int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
              int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
              int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED)
{
    exc.setMotorSpeeds(1, rightTopMotorSpeed, -RightBottomMotorSpeed);
    exc.setMotorSpeeds(2, -RightBottomMotorSpeed, leftTopMotorSpeed);
    if (delay_time != 0)
    {
        delay(delay_time);
        exc.setMotorSpeeds(1, 0, 0);
        exc.setMotorSpeeds(2, 0, 0);
    }
}

void forwardMove(int moveDelayToStop = 0,
                 int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
                 int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
                 int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
                 int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED)
{
    exc.setMotorSpeeds(1, rightTopMotorSpeed, RightBottomMotorSpeed);
    exc.setMotorSpeeds(2, -leftBottomMotorSpeed, -leftTopMotorSpeed);
    if (moveDelayToStop != 0)
    {
        delay(moveDelayToStop);
        exc.setMotorSpeeds(1, 0, 0);
        exc.setMotorSpeeds(2, 0, 0);
    }
}

void stopMove()
{
    exc.setMotorSpeeds(1, 0, 0);
    exc.setMotorSpeeds(2, 0, 0);
}

void gotoPositionY(int setPositionY)
{
    Serial.println(setPositionY);
    Serial.println(g_currentPositionY);
    if (setPositionY != 0)
    {
        if (setPositionY > 5)
        {
            setPositionY = 5;
        }
        if (setPositionY == 0)
        {
            setPositionY = 1;
        }
        while (g_currentPositionY != setPositionY)
        {
            if (setPositionY > g_currentPositionY)
            {
                if (g_directionToLook == 1)
                {
                    g_directionToLook = 0;
                    turnLeft(0, 180);
                    unsigned long start_time, end_time;
                    start_time = millis();
                    while (1)
                    {
                        forwardMove();
                        end_time = millis();
                        if (end_time - start_time > 500)
                        {
                            Stop_Move();
                            break;
                        }
                        else if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1)
                        {
                            break;
                        }
                    }
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
                {
                    forwardMove();
                }
                else
                {
                    if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                        getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0)
                    {
                        forwardMove(0,
                                    DEFAULT_MOTOR_SPEED,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                                    DEFAULT_MOTOR_SPEED,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
                    }
                    else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                             getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
                    {
                        forwardMove(0,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                                    DEFAULT_MOTOR_SPEED,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                                    DEFAULT_MOTOR_SPEED);
                    }
                }
                if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1 &&
                    rec == 0 &&
                    g_directionToLook == 0)
                {
                    Serial.println("센서감지123145564566");
                    g_currentPositionY += 1;
                    Serial.print("현재 좌표 :\tX : ");
                    Serial.print(g_currentPositionX);
                    Serial.print("\tY : ");
                    Serial.println(g_currentPositionY);
                    rec = 1;
                    if (g_currentPositionY == setPositionY)
                    {
                        Stop_Move();
                        break;
                    }
                }
                else if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 0 &&
                         rec == 1 &&
                         g_directionToLook == 0)
                {
                    rec = 0;
                }
            }
            if (setPositionY < g_currentPositionY)
            {
                if (g_directionToLook == 0)
                {
                    turnRight(0, 180);
                    g_directionToLook = 1;
                    unsigned long start_time, end_time;
                    start_time = millis();
                    while (1)
                    {
                        forwardMove();
                        end_time = millis();
                        if (end_time - start_time > 500)
                        {
                            Stop_Move();
                            break;
                        }
                        else if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1)
                        {
                            break;
                        }
                    }
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
                {
                    forwardMove();
                }
                else
                {
                    if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                        getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0)
                    {
                        forwardMove(0,
                                    DEFAULT_MOTOR_SPEED,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                                    DEFAULT_MOTOR_SPEED,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
                    }
                    else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                             getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
                    {
                        forwardMove(0,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                                    DEFAULT_MOTOR_SPEED,
                                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                                    DEFAULT_MOTOR_SPEED);
                    }
                }
                if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1 &&
                    rec == 0 &&
                    g_directionToLook == 1)
                {
                    g_currentPositionY -= 1;

                    Serial.print("현재 좌표 :\tX : 0");
                    Serial.print("\tY : ");
                    Serial.println(g_currentPositionY);

                    rec = 1;
                    if (g_currentPositionY == setPositionY)
                    {
                        Stop_Move();
                        break;
                    }
                }
                else if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 0 &&
                         rec == 1 &&
                         g_directionToLook == 1)
                {
                    rec = 0;
                }
            }
        }
    }
}

void huskylensStarting()
{
    Wire.begin();
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1. Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings >> Protocol Type >> I2C)"));
        Serial.println(F("2. Please recheck the connection."));
    }
    delay(100);
}

void huskylensPrintID(HUSKYLENSResult result)
{
    Serial.println("함수 입장");
    if (result.command == COMMAND_RETURN_BLOCK)
    {
        Serial.println(String() +
                       F("Block:xCenter=") +
                       result.xCenter +
                       F(", yCenter=") +
                       result.yCenter +
                       F(", width=") +
                       result.width +
                       F(", height=") +
                       result.height +
                       F(", ID=") +
                       result.ID);
    }

    else if (result.command == COMMAND_RETURN_ARROW)
    {
        Serial.println(String() +
                       F("Arrow:xOrigin=") +
                       result.xOrigin +
                       F(", yOrigin=") +
                       result.yOrigin +
                       F(", xTarget") +
                       result.xTarget +
                       F(", yTarget=") +
                       result.yTarget +
                       F(", ID=") +
                       result.ID);
    }

    else
    {
        Serial.println("Object unknown!");
    }
}

void colorRecognition(int x, int y)
{
    if (!huskylens.request())
    {
        Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    }
    else if (!huskylens.isLearned())
    {
        Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    }
    else if (!huskylens.available())
    {
        Serial.println(F("No block or arrow appears on the screen!"));
    }
    else
    {
        Serial.println("#####################");
        while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            if (result.yCenter > 100)
            {
                if (result.ID == 1)
                {
                    g_redPillarPositionX = x;
                    g_redPillarPositionY = y; // 만약에 렌즈 다시찍으면 여기 보셈
                }
                else if (result.ID == 2)
                {
                    g_greenPillarPositionX = x;
                    g_greenPillarPositionY = y;
                }
                else if (result.ID == 3)
                {
                    g_bluePillarPositionX = x;
                    g_bluePillarPositionY = y;
                    if ((g_bluePillarPositionX == g_redPillarPositionX &&
                         g_bluePillarPositionY == g_redPillarPositionY) ||
                        (g_bluePillarPositionX == g_greenPillarPositionX &&
                         g_bluePillarPositionY == g_greenBlockPositionY))
                    {
                        g_blueBlockPositionX = x;
                        g_blueBlockPositionY = y; // 만약에 렌즈 다시찍으면 여기 보셈
                    }
                    else
                    {
                        g_bluePillarPositionX = x;
                        g_bluePillarPositionY = y;
                    }
                }
                else if (result.ID == 4)
                {
                    g_yellowPillarPositionX = x;
                    g_yellowPillarPositionY = y;
                }
            }
            else if (result.yCenter < 100)
            {
                if (result.ID == 1)
                {
                    g_redBlockPositionX = x;
                    g_redBlockPositionY = y;
                }
                else if (result.ID == 2)
                {
                    g_greenBlockPositionX = x;
                    g_greenBlockPositionY = y;
                }
                else if (result.ID == 3)
                {
                    g_blueBlockPositionX = x;
                    g_blueBlockPositionY = y;
                }
            }
        }
    }
}

void startMove(int startSpeed = DEFAULT_MOTOR_SPEED)
{
    int dongjak = 1;
    Serial.println("Start");
    int position_reset = 0;
    forwardMove(0, startSpeed, startSpeed, startSpeed, startSpeed);
    start = millis();
    while (dongjak)
    {
        line_checker();
        end = millis();
        if (end - start > 3000)
        {
            Stop_Move();
            Back_Move(3000, startSpeed, startSpeed, startSpeed, startSpeed);
            Serial.println("3S Over");
            break;
        }
        if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1)
        {
            Serial.println("인식 완료 ");
            Stop_Move();
            Left_Move(0, startSpeed, startSpeed, startSpeed, startSpeed);
            unsigned long ls, le;
            ls = millis();
            while (1)
            {
                le = millis();
                if (le - ls > 15000)
                {
                    Serial.println("15초 지남");
                    Stop_Move();
                    Right_Move(15000, startSpeed, startSpeed, startSpeed, startSpeed);

                    break;
                }
                if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1)
                {
                    Serial.println("라인 감지");
                    position_reset = 1;
                    if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1 &&
                        getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1)
                    {
                        position_reset = 1;
                        Serial.println("포지션 설정 완료");
                        break;
                    }
                    else
                    {
                        Serial.println(position_reset);
                        break;
                    }
                }
            }
        }
        if (position_reset == 1)
        {
            unsigned length_s, length_e;
            Serial.println("왼쪽 이동");
            Left_Move(0, startSpeed, startSpeed, startSpeed, startSpeed);
            length_s = millis();
            while (1)
            {
                length_e = millis();
                if (length_e - length_s > 8000)
                {
                    Serial.println("8초 지남");
                    Stop_Move();
                    Right_Move(8000, startSpeed, startSpeed, startSpeed, startSpeed);
                    break;
                }
                if (length_e - length_s > 2000)
                {
                    if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1)
                    {
                        Serial.println("감지 완료");
                        length_e = millis();
                        int cm_length = (length_e - length_s) / 42;
                        int center_length = (length_e - length_s) / 2;
                        Right_Move(cm_length * 7, startSpeed, startSpeed, startSpeed, startSpeed);
                        Serial.println("오른쪽으로");
                        Serial.println(cm_length * 9);
                        // Left_Move(cm_length * 16, startSpeed, startSpeed, startSpeed, startSpeed);
                        // Serial.println("왼쪽으로");
                        // Serial.println(cm_length * 19);
                        dongjak = 0;
                        break;
                    }
                }
            }
            break;
        }
    }
    Serial.println("탈출!");
    delay(500);
    start = millis();
    forwardMove();
    Serial.println("앞으로");
    while (1)
    {
        end = millis();
        if (end - start > 10000)
        {
            Serial.println("10초 오버");
            Stop_Move();
            break;
        }
        if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
            getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            gotoPositionY(0);
            Serial.println("마지막 감지완료!");
            Stop_Move();
            break;
        }
    }
    Serial.println("Start 끝!");
}

void blockPickup()
{
    unsigned long start_time, end_time;
    start_time = millis();
    while (1)
    {
        forwardMove();
        end_time = millis();
        if (end_time - start_time > 1120)
        {
            Stop_Move();
            break;
        }
        if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
            getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            forwardMove();
        }
        else
        {
            if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0)
            {
                forwardMove(0,
                            DEFAULT_MOTOR_SPEED,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                            DEFAULT_MOTOR_SPEED,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
            }
            else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                     getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
            {
                forwardMove(0,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                            DEFAULT_MOTOR_SPEED,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                            DEFAULT_MOTOR_SPEED);
            }
        }
    }
    prizm.setServoPosition(1, 120);
    delay(1600);
    exc.setMotorSpeeds(5, 100, 100);
    delay(600);
    exc.setMotorSpeeds(5, 0, 0);
    start_time = millis();
    while (1)
    {
        Back_Move();
        end_time = millis();
        if (end_time - start_time > 1000)
        {
            Stop_Move();
            break;
        }
        if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
            getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            Back_Move();
        }
        else
        {
            if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
            {

                Back_Move(0,
                          DEFAULT_MOTOR_SPEED,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                          DEFAULT_MOTOR_SPEED,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
            }
            else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                     getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0)
            {

                Back_Move(0,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                          DEFAULT_MOTOR_SPEED,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                          DEFAULT_MOTOR_SPEED);
            }
        }
        if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1 ||
            getLineSensorValue(CENTER_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            Stop_Move();
            break;
        }
    }
}

void blockDrop()
{ // wait for 3 seconds to give servo1 time                            // to get to position 180
    unsigned long start_time, end_time;
    start_time = millis();
    while (1)
    {
        forwardMove();
        end_time = millis();
        if (end_time - start_time > 1120)
        {
            Stop_Move();
            break;
        }
        if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
            getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            forwardMove();
        }
        else
        {
            if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0)
            {
                forwardMove(0,
                            DEFAULT_MOTOR_SPEED,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                            DEFAULT_MOTOR_SPEED,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
            }
            else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                     getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
            {
                forwardMove(0,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                            DEFAULT_MOTOR_SPEED,
                            DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                            DEFAULT_MOTOR_SPEED);
            }
        }
    }
    delay(1600);
    exc.setMotorSpeeds(5, -100, -100);
    delay(600);
    exc.setMotorSpeeds(5, 0, 0);
    prizm.setServoPosition(1, 0);
    start_time = millis();
    while (1)
    {
        Back_Move();
        end_time = millis();
        if (end_time - start_time > 1000)
        {
            Stop_Move();
            break;
        }
        if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
            getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            Back_Move();
        }
        else
        {
            if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1)
            {

                Back_Move(0,
                          DEFAULT_MOTOR_SPEED,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                          DEFAULT_MOTOR_SPEED,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
            }
            else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                     getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0)
            {

                Back_Move(0,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                          DEFAULT_MOTOR_SPEED,
                          DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                          DEFAULT_MOTOR_SPEED);
            }
        }
        if (getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN) == 1 ||
            getLineSensorValue(CENTER_RIGHT_LINE_SENSOR_PIN) == 1)
        {
            Stop_Move();
            break;
        }
    }
}

void turnRight(int setDelayTime = 0, int setAngle = 0)
{
    exc.setMotorSpeeds(1,
                       -DEFAULT_MOTOR_SPEED * 2,
                       -DEFAULT_MOTOR_SPEED * 2);

    exc.setMotorSpeeds(2,
                       -DEFAULT_MOTOR_SPEED * 2,
                       -DEFAULT_MOTOR_SPEED * 2);

    delay(500);
    if (setDelayTime > 0)
    {
        delay(setDelayTime);
        exc.setMotorSpeeds(1, 0, 0);
        exc.setMotorSpeeds(2, 0, 0);
    }
    if (setAngle != 0 && setDelayTime == 0)
    {
        int turn_controll = 0;
        int current_turn = 0;
        switch (setAngle / 90)
        {
        case 1:
            start = millis();
            while (1)
            {
                end = millis();
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                    turn_controll == 0)
                {
                    turn_controll = 1;
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    turn_controll == 1)
                {
                    Stop_Move();
                    break;
                }
            }
            break;

        case 2:
            start = millis();
            while (1)
            {
                end = millis();
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                    turn_controll == 0)
                {
                    turn_controll = 1;
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    turn_controll == 1)
                {
                    turn_controll = 0;
                    current_turn += 1;
                }
                if (current_turn == 2)
                {
                    Stop_Move();
                    break;
                }
                if (end - start > 20000)
                {
                    Stop_Move();
                    break;
                }
            }
            break;

        case 3:
            start = millis();
            while (1)
            {
                end = millis();
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                    turn_controll == 0)
                {
                    turn_controll = 1;
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    turn_controll == 1)
                {

                    turn_controll = 0;
                    current_turn += 1;
                }
                if (current_turn == 3)
                {
                    Stop_Move();
                    break;
                }
                if (end - start > 20000)
                {
                    Stop_Move();
                    break;
                }
            }
            break;

        default:
            Stop_Move();
            break;
        }
    }
}

void turnLeft(int setDelayTime = 0, int setAngle = 0)
{
    exc.setMotorSpeeds(1,
                       DEFAULT_MOTOR_SPEED * 2,
                       DEFAULT_MOTOR_SPEED * 2);
    exc.setMotorSpeeds(2,
                       DEFAULT_MOTOR_SPEED * 2,
                       DEFAULT_MOTOR_SPEED * 2);
    delay(500);
    if (setDelayTime > 0)
    {
        delay(setDelayTime);
        exc.setMotorSpeeds(1, 0, 0);
        exc.setMotorSpeeds(2, 0, 0);
    }
    if (setAngle != 0 && setDelayTime == 0)
    {
        int turn_controll = 0;
        int current_turn = 0;
        switch (setAngle / 90)
        {
        case 1:
            start = millis();
            while (1)
            {
                end = millis();
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                    turn_controll == 0)
                {

                    turn_controll = 1;
                }

                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    turn_controll == 1)
                {

                    Stop_Move();
                    break;
                }
            }
            break;

        case 2:
            start = millis();
            while (1)
            {
                end = millis();
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                    turn_controll == 0)
                {

                    turn_controll = 1;
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    turn_controll == 1)
                {

                    turn_controll = 0;
                    current_turn += 1;
                }
                if (current_turn == 2)
                {
                    Stop_Move();
                    break;
                }
                if (end - start > 20000)
                {
                    Stop_Move();
                    break;
                }
            }
            break;

        case 3:
            start = millis();
            while (1)
            {
                end = millis();
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
                    turn_controll == 0)
                {
                    turn_controll = 1;
                }
                if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
                    turn_controll == 1)
                {
                    turn_controll = 0;
                    current_turn += 1;
                }
                if (current_turn == 3)
                {
                    Stop_Move();
                    break;
                }
                if (end - start > 20000)
                {
                    Stop_Move();
                    break;
                }
            }
            break;

        default:
            Stop_Move();
            break;
        }
    }
}

int getLineSensorValue(int digitalPinNumber)
{
    return prizm.readLineSensor(digitalPinNumber);
}

void showLineSensorStatus()
{
    Serial.print("앞에 왼쪽(2번핀) : ");
    Serial.print(getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN));
    Serial.print("\t앞에 오른쪽(3번핀) : ");
    Serial.print(getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN));
    Serial.print("\t가운데 왼쪽(4번핀) : ");
    Serial.print(getLineSensorValue(CENTER_LEFT_LINE_SENSOR_PIN));
    Serial.print("\t가운데 오른쪽(5번핀) : ");
    Serial.print(getLineSensorValue(CENTER_RIGHT_LINE_SENSOR_PIN));
    Serial.println();
}
