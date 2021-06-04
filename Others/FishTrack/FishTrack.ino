#include "WORK.H"

void setup() {
  // put your setup code here, to run once:
prepare();
}

void loop() {
  bool newData=false;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    logs=SD.open("NMEAF.txt",FILE_WRITE);
    while (Serial1.available())
    {
      char c = Serial1.read();
      logs.print(c);
      if (gps.encode(c)) 
        newData = true;
    }
    logs.close();
  }
  if(newData)
  {
   String data= datanew();
   writelog(data);
   test();
  }
 displayupdate();
}

