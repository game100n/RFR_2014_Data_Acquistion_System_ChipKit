/*
 IMUDatalogger
 Written by Kevin Zheng
 Edited by Harsh Agarwal
 11/28/2013
 
 SD Card on SPI bus as follows
 MOSI: D11
 MISO: D12
 CLK:  D13
 CS:   D10
 
 Logs raw data from MinIMU-v2 from Pololu
 i2c Accelerometer/Magnetometer and gyroscope board
 
 Edit:
 Logs Shock Pot and Steering Angle raw data
 */

#include <Wire.h>
#include <L3G.h>
#include <LSM303.h>
#include <SD.h>

L3G imu_gyro;
LSM303 imu_accmag;

// Analog Pins Shock Pots
const int SP_FR = A0;
const int SP_FL = A1;
const int SP_RR = A2;
const int SP_RL = A3;

int SP_FR_Val = 0;
int SP_FL_Val = 0;
int SP_RR_Val = 0;
int SP_RL_Val = 0;

// Analog Pin Steering Angle
const int SA = A6;

int SA_Val = 0;

// chip select pin of SD card
const int sdCSpin = 10;

// timers for intervals
unsigned long lastPrint,lastWrite;
int printDelay = 200;
int writeDelay = 200;
boolean enableWrite = true;

// calibration offset values
int c_acc_x = 0;
int c_acc_y = 0;
int c_acc_z = 0;
int c_gyr_x = -290;
int c_gyr_y = -256;
int c_gyr_z = -34;
int c_mag_x = 0;
int c_mag_y = 0;
int c_mag_z = 0;

// output calibrated values
int o_acc_x, o_acc_y, o_acc_z, o_gyr_x, o_gyr_y, o_gyr_z, o_mag_x, o_mag_y, o_mag_z;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  // SPI protocol on Arduino mandates hardware CS pin be set as output
  pinMode(10, OUTPUT);
  // initialize SD card
  initialize_sd_card();
  // initialize IMU sensors
  initialize_imu_sensors();
  lastPrint = millis();
  lastWrite = millis();
}

void loop()
{
  collectDataStream();
  if(millis() - lastPrint > printDelay)
  {
    lastPrint = millis();
    printDataStream();
  }
  if(enableWrite)
  {
    if(millis() - lastWrite > writeDelay)
    {
      lastWrite = millis();
      writeDataStream();
    }
  }

  // serial command parsing
  if(Serial.available() > 0)
  {
    int serialBuffer = Serial.read();
    switch(serialBuffer)
    {
    //case 'h':
      //Serial.println("'c' for calibration mode\n'o' to enable datalogging\n'f' to disable datalogging\n'd' to delete log file");
      //break;
    case 'c':
      calibrationMode();
      break;
    case 'o':
      Serial.println("Datalogging enabled!");
      enableWrite = true;
      break;
    case 'f':
      Serial.println("Datalogging disabled!");
      enableWrite = false;
      break;
    case 'd':
      deleteLogfile();
      break;
    }
  }
}

void initialize_sd_card()
{
  Serial.println("SD Card Initialization...");

  // perform SD card initialization
  if (!SD.begin(sdCSpin))
  {
    Serial.println("Error: Card not present or defective! Datalogging disabled!");
    enableWrite = false;
  }
  else Serial.println("Card Initialization Complete");
}

void initialize_imu_sensors()
{
  // initialize l3g gyroscope, check type
  if (!imu_gyro.init())
  {
    Serial.println("Gyroscope detection failure! Halting program!");
    // freeze application
    while(1);
  }
  imu_gyro.enableDefault();
  // initialize lsm303 magnetometer and accelerometer
  imu_accmag.init();
  imu_accmag.enableDefault();
}

void collectDataStream()
{
  imu_gyro.read();
  imu_accmag.read();
  o_acc_x = (int)imu_accmag.a.x + c_acc_x;
  o_acc_y = (int)imu_accmag.a.y + c_acc_y;
  o_acc_z = (int)imu_accmag.a.z + c_acc_z;
  o_gyr_x = (int)imu_gyro.g.x + c_gyr_x;
  o_gyr_y = (int)imu_gyro.g.y + c_gyr_y;
  o_gyr_z = (int)imu_gyro.g.z + c_gyr_z;
  o_mag_x = (int)imu_accmag.m.x + c_mag_x;
  o_mag_y = (int)imu_accmag.m.y + c_mag_y;
  o_mag_z = (int)imu_accmag.m.z + c_mag_z;
  
  SP_FR_Val = analogRead(SP_FR);
  SP_FL_Val = analogRead(SP_FL);
  SP_RR_Val = analogRead(SP_RR);
  SP_RL_Val = analogRead(SP_RL);
  SA_Val = analogRead(SA);
}

void printDataStream()
{
  // print accelerometer data
  Serial.print(millis());
  Serial.print(" Acc X: ");
  Serial.print(o_acc_x);
  Serial.print(" Acc Y: ");
  Serial.print(o_acc_y);
  Serial.print(" Acc Z: ");
  Serial.print(o_acc_z);

  // print gyroscope data
  Serial.print(" Gyr X: ");
  Serial.print(o_gyr_x);
  Serial.print(" Gyr Y: ");
  Serial.print(o_gyr_y);
  Serial.print(" Gyr Z: ");
  Serial.print(o_gyr_z);

  // print magnetometer data
  Serial.print(" Mag X: ");
  Serial.print(o_mag_x);
  Serial.print(" Mag Y: ");
  Serial.print(o_mag_y);
  Serial.print(" Mag Z: ");
  Serial.print(o_mag_z);
  
  // print Shock Pot data
  Serial.print(" SP_FR: ");
  Serial.print(SP_FR_Val);
  Serial.print(" SP_FL: ");
  Serial.print(SP_FL_Val);
  Serial.print(" SP_RR: ");
  Serial.print(SP_RR_Val);
  Serial.print(" SP_RL: ");
  Serial.print(SP_RL_Val);
  
  // print Steering Angle data
  Serial.print(" SA: ");
  Serial.print(SA_Val);

  // line break
  Serial.print("\n");
}

void writeDataStream()
{

  File dataFile = SD.open("IMU_DATA.CSV", FILE_WRITE);
  // confirm data file is writable and perform write operation
  if(dataFile)
  {
    // logs CSV of form "acc_x,acc_y,acc_z,gyr_x,gyr_y,gyr_z,mag_x,mag_y,mag_z"
    // log accelerometer data
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(o_acc_x);
    dataFile.print(",");
    dataFile.print(o_acc_y);
    dataFile.print(",");
    dataFile.print(o_acc_z);

    // log gyroscope data
    dataFile.print(",");
    dataFile.print(o_gyr_x);
    dataFile.print(",");
    dataFile.print(o_gyr_y);
    dataFile.print(",");
    dataFile.print(o_gyr_z);

    // log magnetometer data
    dataFile.print(",");
    dataFile.print(o_mag_x);
    dataFile.print(",");
    dataFile.print(o_mag_y);
    dataFile.print(",");
    dataFile.print(o_mag_z);
    
    // log Shock Pot data
    dataFile.print(",");
    dataFile.print(SP_FR_Val);
    dataFile.print(",");
    dataFile.print(SP_FL_Val);
    dataFile.print(",");
    dataFile.print(SP_RR_Val);
    dataFile.print(",");
    dataFile.print(SP_RL_Val);
    
    // log Steering Angle data
    dataFile.print(",");
    dataFile.print(SA_Val);
    
    dataFile.println("");
    dataFile.close();
  }
  // throw error if file fails to open
  else
  {
    Serial.println("Failed to open IMU_DATA.CSV! Disabling datalogging!");
    enableWrite = false;
  }
}

void calibrationMode()
{
  Serial.println("Enter 'x' to exit calibration mode...");
  boolean exit = false;
  // samples rapidly 100 values, averages those 100 values and reports back
  // testing samples prefixed with "t"
  while(!exit)
  {
    long t_acc_x = 0;
    long t_acc_y = 0;
    long t_acc_z = 0;
    long t_gyr_x = 0;
    long t_gyr_y = 0;
    long t_gyr_z = 0;
    long t_mag_x = 0;
    long t_mag_y = 0;
    long t_mag_z = 0;
    if(Serial.available() > 0)
    {
      if(Serial.read() == 'x') exit = true;
    }
    for(int i = 0; i < 1000; i++)
    {
      collectDataStream();
      t_acc_x += o_acc_x;
      t_acc_y += o_acc_y;
      t_acc_z += o_acc_z;
      t_gyr_x += o_gyr_x;
      t_gyr_y += o_gyr_y;
      t_gyr_z += o_gyr_z;
      t_mag_x += o_mag_x;
      t_mag_y += o_mag_y;
      t_mag_z += o_mag_z;
    }
    Serial.print("Acc X: ");
    Serial.print(t_acc_x/1000);
    Serial.print(" Acc Y: ");
    Serial.print(t_acc_y/1000);
    Serial.print(" Acc Z: ");
    Serial.print(t_acc_z/1000);
    Serial.print(" Gyr X: ");
    Serial.print(t_gyr_x/1000);
    Serial.print(" Gyr Y: ");
    Serial.print(t_gyr_y/1000);
    Serial.print(" Gyr Z: ");
    Serial.print(t_gyr_z/1000);
    Serial.print(" Mag X: ");
    Serial.print(t_mag_x/1000);
    Serial.print(" Mag Y: ");
    Serial.print(t_mag_y/1000);
    Serial.print(" Mag Z: ");
    Serial.print(t_mag_z/1000);
    Serial.println("");
  }
}

void deleteLogfile()
{
  Serial.println("Are you sure you want to delete the log file IMU_DATA.CSV (y/n)?");
  boolean exit = false;
  while(!exit)
  {
    if(Serial.available() > 0)
    {
      int serialBuffer = Serial.read();
      switch(serialBuffer)
      {
      case 'y':
        SD.remove("IMU_DATA.CSV");
        Serial.println("Log file permanently deleted!");
        exit = true;
        break;
      case 'n':
        Serial.println("Log file not deleted.");
        exit = true;
        break;
      }
    }
  }
}



