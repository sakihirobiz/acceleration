#include <M5Core2.h>
#include <map>
 
float accX = 0.0F; // Define variables for storing inertial sensor data
float accY = 0.0F;
float accZ = 0.0F;
 
float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;
 
float temp = 0.0F;

// RTC
RTC_DateTypeDef RTC_DateStruct; // Data
RTC_TimeTypeDef RTC_TimeStruct; // Time
File output_file;
bool isLogging = true;
int num = 0;
int delay_time = 10;
long int bound_filesize = 50*1024*1024;

// Activities
int logging = 0; // 0: 'logging', 1: 'stopped'
std::map<int, std::string> loggingStatus;

// LCD Setting
int left_point = 0;

String zeroPadding(int num, int cnt)
{
  char tmp[256];
  char prm[5] = {'%', '0', (char)(cnt + 48), 'd', '\0'};
  sprintf(tmp, prm, num);
  return tmp;
}
 
void setup()
{
  // Activity Name
  loggingStatus[0] = "logging";
  loggingStatus[1] = "stopped";

  M5.begin();                        // Init M5Core.
  M5.IMU.Init();                     // Init IMU sensor.
  M5.Lcd.fillScreen(BLACK);          // Set the screen background color to black.
  M5.Lcd.setTextColor(WHITE, BLACK); // Sets the foreground color and background color of the displayed text.
  M5.Lcd.setTextSize(2);             // Set the font size.
  M5.Rtc.GetDate(&RTC_DateStruct);
  M5.Rtc.GetTime(&RTC_TimeStruct);
  
  // file open
  String datetime = zeroPadding(RTC_DateStruct.Year, 4) + zeroPadding(RTC_DateStruct.Month, 2) + zeroPadding(RTC_DateStruct.Date, 2) + "-" + zeroPadding(RTC_TimeStruct.Hours, 2) + zeroPadding(RTC_TimeStruct.Minutes, 2) + zeroPadding(RTC_TimeStruct.Seconds, 2);
  String filepath = "/imu_data_" + datetime + ".csv";
  output_file = SD.open(filepath.c_str(), FILE_WRITE);
  if (!output_file)
  {
    M5.Lcd.println("ERROR: OPEN FILE");
    while (1)
      ;
  }
  output_file.println("num,datetime,accX,accY,accZ,gyroX,gyroY,gyroZ,temp"); // Header  num = 0;
}
 
void loop()
{
  // Update
  M5.update();
  
  // Stores the triaxial gyroscope data of the inertial sensor to the relevant variable
  M5.Rtc.GetDate(&RTC_DateStruct);
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.IMU.getAccelData(&accX, &accY, &accZ); // Stores the triaxial accelerometer.
  M5.IMU.getTempData(&temp);                // Stores the inertial sensor temperature to temp.

  //Title
  M5.Lcd.setCursor(left_point,0);
  M5.Lcd.printf("Accerelation Sensor");

  //Date
  String datetime = zeroPadding(RTC_DateStruct.Year, 4) + zeroPadding(RTC_DateStruct.Month, 2) + zeroPadding(RTC_DateStruct.Date, 2) + "-" + zeroPadding(RTC_TimeStruct.Hours, 2) + zeroPadding(RTC_TimeStruct.Minutes, 2) + zeroPadding(RTC_TimeStruct.Seconds, 2);
  M5.Lcd.setCursor(left_point,20);
  M5.Lcd.printf("Date: %s",datetime.c_str());

  //Status
  M5.Lcd.setCursor(left_point,40);
  M5.Lcd.printf("Status:%s",loggingStatus[logging].c_str());

  //Bettery
  float batteryVoltage = M5.Axp.GetBatVoltage();
  int batteryPercentage = map(batteryVoltage * 100, 330, 420, 0, 100); // 3.3V～4.2Vの範囲を0%～100%にマッピング
  M5.Lcd.setCursor(left_point,60);
  M5.Lcd.printf("Voltage:%s(%s%%)",String(batteryVoltage, 1).c_str(),String(batteryPercentage).c_str());

  //SDカードサイズ
  uint64_t cardSize = SD.cardSize() / (1024 * 1024); // カードのサイズ（MB）
  uint64_t usedSize = SD.usedBytes()/ (1024 * 1024);
  M5.Lcd.setCursor(left_point,80);
  M5.Lcd.printf("rest SD:%s(/%s)MB",String(usedSize).c_str(),String(cardSize).c_str());

  // Temperature
  M5.Lcd.setCursor(left_point,110);
  M5.Lcd.printf("Temperature : %.2f C", temp);

  // gyro
  M5.Lcd.setCursor(left_point,130);
  M5.Lcd.printf("gyroX,  gyroY, gyroZ");
  M5.Lcd.setCursor(left_point,150);
  M5.Lcd.printf("%6.2f %6.2f%6.2f o/s", gyroX, gyroY, gyroZ);

  // acc
  M5.Lcd.setCursor(left_point, 170);
  M5.Lcd.printf("accX,   accY,  accZ");
  M5.Lcd.setCursor(left_point, 190);
  M5.Lcd.printf("%5.2f  %5.2f  %5.2f G", accX, accY, accZ);
  
  //footer
  M5.Lcd.setCursor(0,220);
  M5.Lcd.printf("start");
  M5.Lcd.setCursor(120,220);
  M5.Lcd.printf("end");

  // output to file
  output_file.printf("%d,%s,%.7e,%.7e,%.7e,%.7e,%.7e,%.7e,%.7e\n",num, datetime.c_str(), accX, accY, accZ, gyroX,gyroY,gyroZ,temp);
  output_file.flush();
  num++;
  
  if (logging == 0){

    //1ファイルの上限容量を設ける。
    if (output_file.size() > bound_filesize) {
      output_file.close();
      M5.Lcd.setCursor(240,220);
      M5.Lcd.printf("restart");
      // Reopen
      String filepath = "/imu_data_" + datetime + ".csv";
      output_file = SD.open(filepath.c_str(), FILE_WRITE);
      if (!output_file)
      {
        M5.Lcd.println("ERROR: OPEN FILE");
        while (1);
      }
      
      // header
      output_file.println("num,datetime,accX,accY,accZ,gyroX,gyroY,gyroZ,temp"); // Header  num = 0;
      num=0;

      M5.Lcd.setCursor(240,220);
      M5.Lcd.printf("       ");

    }

    if (M5.BtnB.wasPressed()){
    
    //save 
    output_file.close();
    M5.Lcd.setCursor(240,220);
    M5.Lcd.printf("Saved");
    delay(5000);
    M5.Lcd.setCursor(240,220);
    M5.Lcd.printf("      ");

    // status update
    logging = 1;
    }
  }else{
    if (M5.BtnA.wasPressed()){

      // restart
      M5.Lcd.setCursor(240,220);
      M5.Lcd.printf("fmaking");
      delay(5000);
      M5.Lcd.setCursor(240,220);
      M5.Lcd.printf("       ");
      // Reopen
      String filepath = "/imu_data_" + datetime + ".csv";
      output_file = SD.open(filepath.c_str(), FILE_WRITE);
      if (!output_file)
      {
        M5.Lcd.println("ERROR: OPEN FILE");
        while (1);
      }
      
      // header
      output_file.println("num,datetime,accX,accY,accZ,gyroX,gyroY,gyroZ,temp"); // Header  num = 0;
      num=0;

      // status update
      logging = 0;
    }
  }
  delay(delay_time); // Delay 10ms.
}