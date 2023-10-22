#include <PRIZM.h>
#include <time.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

#define FRONT_LEFT_LINE_SENSOR_PIN    2
#define FRONT_RIGHT_LINE_SENSOR_PIN   3
#define CENTER_LEFT_LINE_SENSOR_PIN   4
#define CENTER_RIGHT_LINE_SENSOR_PIN  5
#define DEFAULT_MOTOR_SPEED           75

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

// Robot Is Turned
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

void colorRecognition(int currentPositionX, int curruntPositionY);

unsigned long g_asyncStartTimeMsec, g_asyncEndTimeMsec;
int g_directionToLook;
void servoMove();
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
void moveX(int setPositionX);
void moveY(int setPositionY);
int getLineSensorValue(int lineSensorPinNum);
void showLineSensorStatus();
void getNearbyBlock(int blockID = 0);
void allBlockScan();

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
int start_speed = 50;

int go_position_x, go_position_y;
int color = 0;
int color1 = 0;
int Y = 0;
int before_color = 0;
int Y_Color = 0;
int alt_color = 0;

void pickup_block(int delay_time = 0) {
  prizm.setMotorPower(2, -15);
  delay(delay_time);
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
  for (int i = 2; i < 6; i++){
    if (red_y == i || green_y == i || blue_y == i){
      go_position_y = i;
      if (red_y == i){
        color = 1;
      } // 가장 가까운 값이 빨간색이면 
      else if (green_y == i){
        color = 2;
      }
      else if (blue_y == i){
        color = 3;
      }
      if (color != 0){
        break;
      }
    }
  }
  if (color == 1){
    if (red_y == cyl_gy && red_x == cyl_gx){
      color1 = 2;
    }
    else if (red_y == cyl_by && red_x == cyl_bx){
      color1 = 3;
    }
  }
  else if (color == 2){
    if (green_y == cyl_ry && green_x == cyl_rx){
      color1 = 1;
    }
    else if (green_y == cyl_by && green_x == cyl_bx){
      color1 = 3;
    }
  }
  else if (color == 3){
    if (blue_y == cyl_ry && blue_x == cyl_rx){
      color1 = 1;
    }
    else if (blue_y == cyl_gy && blue_x == cyl_gx){
      color1 = 2;
    }
  }
  Serial.print(color1);
  goto_color_block(color); // 레드로 가
  Servo_Grap();            // 찝어
  if (find_y_b(color1) == cyl_yy && cyl_yy == g_currentPositionY){
    if (Y == 0){
      Turn_Right(0, 180);
      Servo_Put();
      Y_Color = color;
      Y = 1;
    }
  }
  else
  {
    reset_position();
    if (Y == 0){
      goto_color_cyl(4);
      Servo_Put();
      Y_Color = color;
      Y = 1;
      reset_position();
    }
  }
  goto_color_block(color1); // color1 = blue color = red Y_color = red
  Servo_Grap();
  reset_position();
  Serial.print("First_down Color : ");
  Serial.println(color1);
  Serial.println(down);
  goto_color_block(Y_Color);
  Servo_Put();
  reset_position();
  Serial.print("First_down Color : ");
  Serial.println(color);
  Serial.println(down);
  goto_color_cyl(Y_Color);
  Servo_Grap();
  reset_position();
  Serial.print("First_down Color : ");
  Serial.println(color);
  Serial.println(down);
  goto_color_block(color1);
  Servo_Put();
  reset_position();
  Serial.print("First_down Color : ");
  Serial.println(color1);
  Serial.println(down);
  goto_color_cyl(4);
  Servo_Grap();
  reset_position();
  Serial.print("First_down Color : ");
  Serial.println("Yellow");
  Serial.println(down);
  goto_color_cyl(color);
  Servo_Put();
  reset_position();
  Serial.print("First_down Color : ");
  Serial.println(color);
  Serial.println(down);
  Move_y(1);
  forwardMove(4000);
  Right_Move(2000);
  Stop_Move();
}

int find_x_b(int color)
{
  if (color == 1)
  {
    return red_x;
  }
  else if (color == 2)
  {
    return green_x;
  }
  else if (color == 3)
  {
    return blue_x;
  }
}

int find_y_b(int color)
{
  if (color == 1)
  {
    return red_y;
  }
  else if (color == 2)
  {
    return green_y;
  }
  else if (color == 3)
  {
    return blue_y;
  }
}

int find_x_c(int color)
{
  if (color == 1)
  {
    return cyl_rx;
  }
  else if (color == 2)
  {
    return cyl_gx;
  }
  else if (color == 3)
  {
    return cyl_bx;
  }
  else if (color == 4)
  {
    return cyl_yx;
  }
}

int find_y_c(int color)
{
  if (color == 1)
  {
    return cyl_ry;
  }
  else if (color == 2)
  {
    return cyl_gy;
  }
  else if (color == 3)
  {
    return cyl_by;
  }
  else if (color == 4)
  {
    return cyl_yy;
  }
}

void reset_position()
{
  if (g_is_Turned == 1)
  {
    if (g_currentPositionY == 5 &&
        g_directionToLook == 0 ||
        g_currentPositionY == 2 &&
        g_directionToLook == 1) {
      turnLeft(0, 90);
      if (g_directionToLook == 1) {
        g_directionToLook = 0;
      }

      else if (g_directionToLook == 0) {
        g_directionToLook = 1;
      }

    }
    else {
      turnRight(0, 90);
    }
  }
  else if (g_is_Turned == 2) {
    if (g_currentPositionY == 2 &&
        g_directionToLook == 1 ||
        g_currentPositionY == 5 &&
        g_directionToLook == 0) {
      turnRight(0, 90);
      if (g_directionToLook == 1)
        g_directionToLook = 0;
      else if (g_directionToLook == 0)
        g_directionToLook = 1;
    }
    else
    {
      turnLeft(0, 90);
    }
  }
}

void moveX(int x)
{
  if (x >= 0 && x < 3)
  {
    if (x == 0)
    {
      if (down == 0)
      {
        Turn_Left(0, 90);
        g_is_Turned = 1;
      }
      if (down == 1)
      {
        Turn_Right(0, 90);
        g_is_Turned = 2;
      }
      c_current_x = 0;
    }
    else if (x == 2)
    {
      if (down == 1)
      {
        Turn_Left(0, 90);
        g_is_Turned = 1;
      }
      if (down == 0)
      {
        Turn_Right(0, 90);
        g_is_Turned = 2;
      }
    }
    c_current_x = 2;
  }
}

void gotoColorBlock(int colorID = 0)
{
  if (colorID != 0)
  {
    switch (colorID)
    {
    case 1:
      Move_y(red_y);
      Move_x(red_x); // 바라보기 까지만 함
      break;
    case 2:
      Move_y(green_y);
      Move_x(green_x);
      break;
    case 3:
      Move_y(blue_y);
      Move_x(blue_x);
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
      Move_y(cyl_ry);
      Move_x(cyl_rx); // 바라보기 까지만 함
      break;
    case 2:
      Move_y(cyl_gy);
      Move_x(cyl_gx);
      break;
    case 3:
      Move_y(cyl_by);
      Move_x(cyl_bx);
      break;

    case 4:
      Move_y(cyl_yy);
      Move_x(cyl_yx);
      break;
    }
  }
}

void allBlockScan()
{
  unsigned long scan_start, scan_end;
  Serial.println("Move 2");
  Move_y(2);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    getcol(c_current_x, g_currentPositionY);
  }
  Serial.println("Move 3");
  Move_y(3);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Serial.println("Move 4");
  Move_y(4);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Serial.println("Move 5");
  Move_y(5);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Turn_Right(0, 180);
  forwardMove();
  down = 1;
  c_current_x = 2;
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Serial.println("Move 4");
  Move_y(4);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Serial.println("Move 3");
  Move_y(3);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Serial.println("Move 2");
  Move_y(2);
  delay(500);
  scan_start = millis();
  while (1)
  {
    scan_end = millis();
    if (scan_end - scan_start > 500)
    {
      break;
    }
    colorRecognition(c_current_x, g_currentPositionY);
  }
  Serial.println(String() + F("red block : ") + red_x + F(", ") + red_y);
  Serial.println(String() + F("red cyl : ") + cyl_rx + F(", ") + cyl_ry);
  Serial.println(String() + F("green block : ") + green_x + F(", ") + green_y);
  Serial.println(String() + F("green cyl : ") + cyl_gx + F(", ") + cyl_gy);
  Serial.println(String() + F("blue block : ") + blue_x + F(", ") + blue_y);
  Serial.println(String() + F("blue cyl : ") + cyl_bx + F(", ") + cyl_by);
  Serial.println(String() + F("Yellow cyl : ") + cyl_yx + F(", ") + cyl_yy);
}

void rightMove(int delay_time = 0,
                int leftTopMotorSpeed = DEFAULT_MOTOR_SPEED,
                int rightTopMotorSpeed = DEFAULT_MOTOR_SPEED,
                int leftBottomMotorSpeed = DEFAULT_MOTOR_SPEED,
                int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED) {
  exc.setMotorSpeeds(1, -rightTopMotorSpeed, RightBottomMotorSpeed);
  exc.setMotorSpeeds(2, RightBottomMotorSpeed, -leftTopMotorSpeed);
  if (delay_time != 0)
  {
    delay(delay_time);
    exc.setMotorSpeeds(1, 0, 0);
    exc.setMotorSpeeds(2, 0, 0);
  }
}

void Back_Move(int delay_time = 0, int L_T = SPEED, int R_T = SPEED, int L_B = SPEED, int R_B = SPEED)
{
  exc.setMotorSpeeds(1, -R_T, -R_B);
  exc.setMotorSpeeds(2, L_B, L_T);
  if (delay_time != 0)
  {
    delay(delay_time);
    exc.setMotorSpeeds(1, 0, 0);
    exc.setMotorSpeeds(2, 0, 0);
  }
}

void Left_Move(int delay_time = 0, int L_T = SPEED, int R_T = SPEED, int L_B = SPEED, int R_B = SPEED)
{
  exc.setMotorSpeeds(1, R_T, -R_B);
  exc.setMotorSpeeds(2, -L_B, L_T);
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
                int RightBottomMotorSpeed = DEFAULT_MOTOR_SPEED) {
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

void Move_y(int set_y)
{
  Serial.println(set_y);
  Serial.println(g_currentPositionY);
  if (set_y != 0)
  {
    if (set_y > 5)
      set_y = 5;
    if (set_y == 0)
      set_y = 1;
    while (g_currentPositionY != set_y)
    {
      if (set_y > g_currentPositionY)
      {
        if (down == 1)
        {
          down = 0;
          Turn_Left(0, 180);
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
            else if (getLineSensorValue(Center_Left) == 1)
            {
              break;
            }
          }
        }
        if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 1)
        {
          forwardMove();
        }
        else
        {
          if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 0)
          {
            forwardMove(0, SPEED, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10));
          }
          else if (getLineSensorValue(Front_Left) == 0 && getLineSensorValue(Front_Right) == 1)
          {
            forwardMove(0, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10), SPEED);
          }
        }
        if (getLineSensorValue(Center_Left) == 1 && rec == 0 && down == 0)
        {
          Serial.println("센서감지123145564566");
          g_currentPositionY += 1;
          Serial.print("현재 좌표 :\tX : ");
          Serial.print(c_current_x);
          Serial.print("\tY : ");
          Serial.println(g_currentPositionY);
          rec = 1;
          if (g_currentPositionY == set_y)
          {
            Stop_Move();
            break;
          }
        }
        else if (getLineSensorValue(Center_Left) == 0 && rec == 1 && down == 0)
        {
          rec = 0;
        }
      }
      if (set_y < g_currentPositionY)
      {
        if (down == 0)
        {
          Turn_Right(0, 180);
          down = 1;
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
            else if (getLineSensorValue(Center_Left) == 1)
            {
              break;
            }
          }
        }
        if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 1)
        {
          forwardMove();
        }
        else
        {
          if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 0)
          {
            forwardMove(0,
                        DEFAULT_MOTOR_SPEED,
                        DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                        DEFAULT_MOTOR_SPEED,
                        DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
          }
          else if (getLineSensorValue(Front_Left) == 0 && getLineSensorValue(Front_Right) == 1)
          {
            forwardMove(0,
                        DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                        DEFAULT_MOTOR_SPEED,
                        DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                        DEFAULT_MOTOR_SPEED);
          }
        }
        if (getLineSensorValue(Center_Left) == 1 && rec == 0 && down == 1)
        {
          g_currentPositionY -= 1;
          Serial.print("현재 좌표 :\tX : 0");
          Serial.print("\tY : ");
          Serial.println(g_currentPositionY);
          rec = 1;
          if (g_currentPositionY == set_y)
          {
            Stop_Move();
            break;
          }
        }
        else if (getLineSensorValue(Center_Left) == 0 && rec == 1 && down == 1)
        {
          rec = 0;
        }
      }
    }
  }
}

void cho(int PIN_MODE)
{
  pinMode(PIN_MODE, OUTPUT);
  digitalWrite(PIN_MODE, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_MODE, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_MODE, LOW);

  pinMode(3, INPUT);
  double duration = pulseIn(3, HIGH);
  double cm = duration * 340 / 10000 / 2;

  Serial.println(cm);
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

void huskylensPrintID(HUSKYLENSResult result) {
  Serial.println("함수 입장");
  if (result.command == COMMAND_RETURN_BLOCK) {
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

  else if (result.command == COMMAND_RETURN_ARROW) {
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

  else{
    Serial.println("Object unknown!");
  }
}

void colorRecognition(int x, int y)
{
  if (!huskylens.request())
    Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if (!huskylens.isLearned())
    Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if (!huskylens.available())
    Serial.println(F("No block or arrow appears on the screen!"));
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

void startMove(int start_speed = DEFAULT_MOTOR_SPEED)
{
  int dongjak = 1;
  Serial.println("Start");
  int position_reset = 0;
  forwardMove(0, start_speed, start_speed, start_speed, start_speed);
  start = millis();
  while (dongjak)
  {
    line_checker();
    end = millis();
    if (end - start > 3000)
    {
      Stop_Move();
      Back_Move(3000, start_speed, start_speed, start_speed, start_speed);
      Serial.println("3S Over");
      break;
    }
    if (getLineSensorValue(Front_Left) == 1)
    {
      Serial.println("인식 완료 ");
      Stop_Move();
      Left_Move(0, start_speed, start_speed, start_speed, start_speed);
      unsigned long ls, le;
      ls = millis();
      while (1)
      {
        le = millis();
        if (le - ls > 15000)
        {
          Serial.println("15초 지남");
          Stop_Move();
          Right_Move(15000, start_speed, start_speed, start_speed, start_speed);

          break;
        }
        if (getLineSensorValue(Center_Left) == 1)
        {
          Serial.println("라인 감지");
          position_reset = 1;
          if (getLineSensorValue(Center_Left) == 1 && getLineSensorValue(Front_Left) == 1)
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
      Left_Move(0, start_speed, start_speed, start_speed, start_speed);
      length_s = millis();
      while (1)
      {
        length_e = millis();
        if (length_e - length_s > 8000)
        {
          Serial.println("8초 지남");
          Stop_Move();
          Right_Move(8000, start_speed, start_speed, start_speed, start_speed);
          break;
        }
        if (length_e - length_s > 2000)
        {
          if (getLineSensorValue(Center_Left) == 1)
          {
            Serial.println("감지 완료");
            length_e = millis();
            int cm_length = (length_e - length_s) / 42;
            int center_length = (length_e - length_s) / 2;
            Right_Move(cm_length * 7, start_speed, start_speed, start_speed, start_speed);
            Serial.println("오른쪽으로");
            Serial.println(cm_length * 9);
            // Left_Move(cm_length * 16, start_speed, start_speed, start_speed, start_speed);
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
    if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 1)
    {
      Move_y(0);
      Serial.println("마지막 감지완료!");
      Stop_Move();
      break;
    }
  }
  Serial.println("Start 끝!");
}

void Servo_Grap()
{
  unsigned long start_time, end_time;
  start_time = millis();
  while (1) {
    forwardMove();
    end_time = millis();
    if (end_time - start_time > 1120) {
      Stop_Move();
      break;
    }
    if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1&&
        getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1) {
      forwardMove();
    }
    else
    {
      if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 1 &&
          getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 0) {
        forwardMove(0,
                    DEFAULT_MOTOR_SPEED,
                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10),
                    DEFAULT_MOTOR_SPEED,
                    DEFAULT_MOTOR_SPEED + ((DEFAULT_MOTOR_SPEED * 2) / 10));
      }
      else if (getLineSensorValue(FRONT_LEFT_LINE_SENSOR_PIN) == 0 &&
              getLineSensorValue(FRONT_RIGHT_LINE_SENSOR_PIN) == 1) {
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
    if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 1)
    {
      Back_Move();
    }
    else
    {
      if (getLineSensorValue(Front_Left) == 0 && getLineSensorValue(Front_Right) == 1)
      {
        Back_Move(0, SPEED, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10));
      }
      else if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 0)
      {
        Back_Move(0, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10), SPEED);
      }
    }
    if (getLineSensorValue(Center_Left) == 1 || getLineSensorValue(Center_Right) == 1)
    {
      Stop_Move();
      break;
    }
  }
}

void Servo_Put()
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
    if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 1)
    {
      forwardMove();
    }
    else
    {
      if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 0)
      {
        forwardMove(0, SPEED, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10));
      }
      else if (getLineSensorValue(Front_Left) == 0 && getLineSensorValue(Front_Right) == 1)
      {
        forwardMove(0, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10), SPEED);
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
    if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 1)
    {
      Back_Move();
    }
    else
    {
      if (getLineSensorValue(Front_Left) == 0 && getLineSensorValue(Front_Right) == 1)
      {
        Back_Move(0, SPEED, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10));
      }
      else if (getLineSensorValue(Front_Left) == 1 && getLineSensorValue(Front_Right) == 0)
      {
        Back_Move(0, SPEED + ((SPEED * 2) / 10), SPEED, SPEED + ((SPEED * 2) / 10), SPEED);
      }
    }
    if (getLineSensorValue(Center_Left) == 1 || getLineSensorValue(Center_Right) == 1)
    {
      Stop_Move();
      break;
    }
  }
}

void Turn_Right(int delay_time = 0, int angle = 0)
{
  exc.setMotorSpeeds(1, -SPEED * 2, -SPEED * 2);
  exc.setMotorSpeeds(2, -SPEED * 2, -SPEED * 2);
  delay(500);
  if (delay_time > 0)
  {
    delay(delay_time);
    exc.setMotorSpeeds(1, 0, 0);
    exc.setMotorSpeeds(2, 0, 0);
  }
  if (angle != 0 && delay_time == 0)
  {
    int turn_controll = 0;
    int current_turn = 0;
    switch (angle / 90)
    {
    case 1:
      start = millis();
      while (1)
      {
        end = millis();
        if (getLineSensorValue(Front_Left) == 0 && turn_controll == 0)
          turn_controll = 1;
        if (getLineSensorValue(Front_Left) == 1 && turn_controll == 1)
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
        if (getLineSensorValue(Front_Left) == 0 && turn_controll == 0)
        {
          turn_controll = 1;
        }
        if (getLineSensorValue(Front_Left) == 1 && turn_controll == 1)
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
        if (getLineSensorValue(Front_Left) == 0 && turn_controll == 0)
        {
          turn_controll = 1;
        }
        if (getLineSensorValue(Front_Left) == 1 && turn_controll == 1)
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

void Turn_Left(int delay_time = 0, int angle = 0)
{
  exc.setMotorSpeeds(1, SPEED * 2, SPEED * 2);
  exc.setMotorSpeeds(2, SPEED * 2, SPEED * 2);
  delay(500);
  if (delay_time > 0)
  {
    delay(delay_time);
    exc.setMotorSpeeds(1, 0, 0);
    exc.setMotorSpeeds(2, 0, 0);
  }
  if (angle != 0 && delay_time == 0)
  {
    int turn_controll = 0;
    int current_turn = 0;
    switch (angle / 90)
    {
    case 1:
      start = millis();
      while (1)
      {
        end = millis();
        if (getLineSensorValue(Front_Left) == 0 && turn_controll == 0)
          turn_controll = 1;
        if (getLineSensorValue(Front_Left) == 1 && turn_controll == 1)
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
        if (getLineSensorValue(Front_Left) == 0 && turn_controll == 0)
        {
          turn_controll = 1;
        }
        if (getLineSensorValue(Front_Left) == 1 && turn_controll == 1)
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
        if (getLineSensorValue(Front_Left) == 0 && turn_controll == 0)
        {
          turn_controll = 1;
        }
        if (getLineSensorValue(Front_Left) == 1 && turn_controll == 1)
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

int getLineSensorValue(int pin_num)
{
  return getLineSensorValue(pin_num);
}

void showLineSensorStatus()
{
  Serial.print("앞에 왼쪽(2번핀) : ");
  Serial.print(getLineSensorValue(Front_Left));
  Serial.print("\t앞에 오른쪽(3번핀) : ");
  Serial.print(getLineSensorValue(Front_Right));
  Serial.print("\t가운데 왼쪽(4번핀) : ");
  Serial.print(getLineSensorValue(Center_Left));
  Serial.print("\t가운데 오른쪽(5번핀) : ");
  Serial.print(getLineSensorValue(Center_Right));
  Serial.println();
}