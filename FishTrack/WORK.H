//Pin 18 To Reset
//Pin 19 To Select
//Pin 20 To Scroll Up
//Pin 21 To Scroll Down
//Serial0 for Debugging
//Serial1 For GPS
//Pin 51 For MOSI
//Pin 50 For MISO
//Pin 52 For SCK
//Pin 53 Must be set as Output for Perfect SPI Operation
//Pin 4 For SS
//Pin 12 For RS LCD
//Pin 11 For EN LCD
//Pin 13 For D4 LCD
//Pin 14 For D5 LCD
//Pin 15 For D6 LCD
//Pin 16 For D7 LCD
//R/W LCD To GND
//VSS LCD To GND
//VCC LCD To +5V
//10K POT: Ends To 5V and GND. Control to V0

#include <LiquidCrystal.h>
#include <SPI.h>
#include <TinyGPS.h>
#include <SD.h>
#include <math.h>


TinyGPS gps;
File logs;
LiquidCrystal lcd(12, 11, 13, 14, 15, 16);
float locationdb[100][2];
float present[2];
void Reset();

void select(int datacount)
{
  detachInterrupt(3);
  int count=0;
  while(1)
  {
    if(digitalRead(20)==LOW)
    {
      count++;
      if(count<0)
      count=datacount+count;
    }
    if(digitalRead(21)==LOW)
    {
      count--;
      if(count<0)
      count=datacount+count;
    }
    if(digitalRead(19)==LOW)
    {
      present[0]=locationdb[count%datacount][0];
      present[1]=locationdb[count%datacount][1];
      String data;
      data=String(present[0],6);
      data+=',';
      data+=String(present[1],6);
      Serial.println("The Selected Location is:");
      Serial.println(data);
      attachInterrupt(3, Reset, FALLING);
      break;
    }
  }
}
int separatelocation(String input)
{
  char data[input.length()+1];
  int k=0,z=0;
  input.toCharArray(data,input.length()+1);
  String check[100][2];
  for(int i=0;i<input.length();i++)
  {
    if(data[i]=='n')
    {
      k++;
      i++;
      z=0;
    }
    if(data[i]==',')
    {
      z=1;
      i++;
    }
    //Serial.println(data[i]);
    //broken[k][z][j]=data[i];
    check[k][z]+=data[i];
  }
  Serial.println(check[0][0]);
  Serial.println(check[0][1]);
  Serial.println(check[1][0]);
  Serial.println(check[1][1]);
  Serial.println(check[2][0]);
  Serial.println(check[2][1]);
  for(int i=0;i<k;i++)
  {
    locationdb[i][0]=check[i][0].toFloat();
    locationdb[i][1]=check[i][1].toFloat();
  }
  return k;
}

String readlocation()
{
  String val;
  logs=SD.open("Locn.txt", FILE_READ);
  if(logs)
  Serial.println("Locn is Open");
  while(logs.available())
  {
    val+=(char)logs.read();
  }
  Serial.println(val);
  logs.close();
  Serial.println("Locn Closed");
  return val;
}

void Reset()
{
  Serial.println("ISR enter");
  bool flag=0;
  for(int i=0;i<6400;i++)
  {
    if(digitalRead(19)==LOW)
    {
      flag=1;
      break;
    }
  }
  if(flag==1)
  {
    select(separatelocation(readlocation()));
  }
  return;
}

void writelog(String data)
{
  logs=SD.open("Logs.txt", FILE_WRITE);
  if(logs)
  Serial.println("Logs is Open");
  logs.print(data);
  logs.close();
  Serial.println("Logs is Closed");
}

String datanew()
{
  String data;
  float flat,flong,lat,lon;
  unsigned long age;
  gps.f_get_position(&flat,&flong,&age);
  lat=(flat==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
  lon=(flong==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flong);
  data=String(lat,6);
  data+=',';
  data+=String(lon,6);
  data+='n';
  Serial.println(data);
  return data;
}

void test()
{
  int count=separatelocation(readlocation());
  Serial.println(count);
  for(int i=0;i<count;i++)
  {
    String data;
    data=String(locationdb[i][0],6);
    data+=',';
    data+=String(locationdb[i][1],6);
    data+='n';
    Serial.println(data);     
  }
}

float distance(float olat,float olong,float dlat, float dlong)
{
  float radius=6371;
  float ndlat= (dlat-olat)*1000/57296;
  float ndlong= (dlong-olong)*1000/57296;
  olat=(olat)*1000/57296;
  dlat=(dlat)*1000/57296;
  float a=pow(sin(ndlat/2),2)+cos(olat)*cos(dlat)*pow(sin(ndlong/2),2);
  float b=2*atan2(sqrt(a),sqrt(1-a));
  float c=radius*b;
  return c;
}

void displayupdate(float dist, float bearing)

void prepare()
{
  Serial.begin(115200);
  Serial1.begin(9600);
  lcd.begin(16,2);
  lcd.setCursor(4,0);
  lcd.print("WELCOME!");
  delay(100);
  if(!SD.begin(4))
  {
    Serial.println("SD Card Initialization Fail");
    lcd.clear();
    delay(100);
    lcd.setCursor(2,0);
    lcd.print("SD FAIL");
    return;
  }
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  attachInterrupt(5, Reset, FALLING);
}

