//Pin 18 To Reset
//Pin 19 To Select
//Pin 20 To Scroll Up
//Pin 21 To Scroll Down
//Serial0 for Debugging
//Serial3 For GPS
//Pin 51 For MOSI
//Pin 50 For MISO
//Pin 52 For SCK
//Pin 53 Must be set as Output for Perfect SPI Operation
//Pin 53 For SS
//LCD RS pin to digital pin 12
//LCD Enable pin to digital pin 11
//LCD D4 pin to digital pin 5
//LCD D5 pin to digital pin 4
//LCD D6 pin to digital pin 3
//LCD D7 pin to digital pin 2
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
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float locationdb[100][2];
float present[2];
float borders[2][2];
void Reset();
float distance(float,float,float,float);
int bearing(float,float,float,float);

float slope(float x1,float y1,float x2,float y2)
{
  float m=(y2-y1)/(x2-x1);
  return m;
}

float anglebet(float m1,float m2)
{
  float angle;
  if((m1*m2)==-1)
   return 90;
  else if(m1==m2)
   return 0;
  else
  {
    float num=m1-m2;
    float den=1+(m1*m2);
    angle=(num/den);
    return angle;
  }
}

void select(int datacount)
{
  detachInterrupt(digitalPinToInterrupt(18));
  int count=0;
  while(1)
  {
    if(digitalRead(20)==LOW)
    {
      count++;
      if(count<0)
      count=datacount+count;
      while(digitalRead(20)==LOW);
    }
    if(digitalRead(21)==LOW)
    {
      count--;
      if(count<0)
      count=datacount+count;
      while(digitalRead(21)==LOW);
    }
    float flat,flong,lat,lon;
    unsigned long age;
    gps.f_get_position(&flat,&flong,&age);
    lat=(flat==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
    lon=(flong==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flong);
    float dist=distance(lat,lon,locationdb[count%datacount][0],locationdb[count%datacount][1]);
    int bear=bearing(lat,lon,locationdb[count%datacount][0],locationdb[count%datacount][1]);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(count%datacount);
    lcd.print('.');
    String data=String(dist,6);
    lcd.print(data);
    data=String(bear);
    lcd.setCursor(3,1);
    lcd.print(data);
    Serial.println(dist);
    Serial.println(bear);
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
      attachInterrupt(digitalPinToInterrupt(18), Reset, FALLING);
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
  String data="Dist:";
  data+=String(c,6);
  Serial.println(data);
  return c;
}

int bearing(float olat,float olong, float dlat,float dlong)
{
  dlong=(dlong)*1000/57296;
  dlat=(dlat)*1000/57296;
  olat=(olat)*1000/57296;
  olong=(olong)*1000/57296;
  float x=cos(dlat)*sin((dlong-olong));
  float y=(cos(olat)*sin(dlat))-(sin(olat)*cos(dlat)*cos((dlat-olat)));
  float bear=atan2(x,y);
  float degbearing=bear*57296/1000;
  String data="Bear:";
  data+=String(degbearing,6);
  Serial.println(data);
  return (((int)degbearing+360)%360);
}

void displayupdate()
{
    float flat,flong,lat,lon;
    unsigned long age;
    gps.f_get_position(&flat,&flong,&age);
    lat=(flat==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
    lon=(flong==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flong);
    float dist=distance(lat,lon,present[0],present[1]);
    int bear=bearing(lat,lon,present[0],present[1]);
    lcd.clear();
    lcd.setCursor(5,0);
    String data=String(dist,6);
    lcd.print(data);
    data=String(bear);
    lcd.setCursor(3,1);
    lcd.print(data);
}

void prepare()
{
  Serial.begin(115200);
  Serial3.begin(9600);
  delay(100);
  lcd.begin(16,2);
  delay(100);
  lcd.setCursor(4,0);
  delay(100);
  lcd.print("WELCOME!");
  delay(100);
  if(!SD.begin(53))
  {
    Serial.println("SD Card Initialization Fail");
    lcd.clear();
    delay(100);
    lcd.setCursor(2,0);
    lcd.print("SD FAIL");
    return;
  }
  bool data=false;
  do
  {
    while (Serial3.available())
    {
      char c = Serial3.read();
      Serial.print(c);
      if (gps.encode(c)) 
        data = true;
    }
  }
  while(data!=true);
  select(separatelocation(readlocation()));
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(18), Reset, FALLING);
}

