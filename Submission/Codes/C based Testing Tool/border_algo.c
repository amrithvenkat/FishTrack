#include<stdio.h>
#include<math.h>

float slope(float y1,float x1,float y2,float x2)
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
  return (((int)degbearing+360)%360);
}

int borderselect(float o_lat,float o_long,float d_lat,float d_long,float alt_lat1,float alt_long1,float alt_lat2,float alt_long2)
{
 float current,prevpoint,nextpoint,prevangle,nextangle;
 current = slope(d_lat,d_long,o_lat,o_long);
 nextpoint = slope(alt_lat1,alt_long1,o_lat,o_long);
 prevpoint = slope(alt_lat2,alt_long2,o_lat,o_long);
 prevangle = anglebet(current,prevpoint);
 nextangle = anglebet(current,nextpoint);

 if(nextangle<0)
 {
  return 2;
 }
 else
 {
  return 1;
 }

}

void main()
{
 int choice,i;
 float o_lat,o_long,d_lat,d_long;
 float temp_lat1,temp_long1,temp_lat2,temp_long2;

 printf("1. Distance Algorithm\n2. Bearing Algorithm\n3. Slope Calculation\n4. Angle between two Slopes\n5. Correct Border Selection\nEnter the Choice:");
 scanf("%d",&choice);

 switch(choice)
 {
  case 1:printf("Enter Origin Latitude\n");
         scanf("%f",&o_lat);
         printf("Enter Origin Longitude\n");
         scanf("%f",&o_long);
         printf("Enter Destination Latitude\n");
         scanf("%f",&d_lat);
         printf("Enter Destination Longitude\n");
         scanf("%f",&d_long);
         printf("The distance is:%f\n kms",distance(o_lat,o_long,d_lat,d_long));
         break;

  case 2:printf("Enter Origin Latitude\n");
          scanf("%f",&o_lat);
          printf("Enter Origin Longitude\n");
          scanf("%f",&o_long);
          printf("Enter Destination Latitude\n");
          scanf("%f",&d_lat);
          printf("Enter Destination Longitude\n");
          scanf("%f",&d_long);
          printf("The bearing is %d\n",bearing(o_lat,o_long,d_lat,d_long));
          break;

  case 3:printf("Enter Latitude 1\n");
         scanf("%f",&o_lat);
         printf("Enter Longitude 1\n");
         scanf("%f",&o_long);
         printf("Enter Latitude 2\n");
         scanf("%f",&d_lat);
         printf("Enter Longitude 2\n");
         scanf("%f",&d_long);
         printf("The slope is %f\n",slope(o_lat,o_long,d_lat,d_long));
         break;

  case 4:printf("Enter Slope 1\n");
         scanf("%f",&o_lat);
         printf("Enter Slope 2\n");
         scanf("%f",&o_long);
         printf("The angle between the slopes %f\n",anglebet(o_lat,o_long));
         break;

  case 5:printf("Enter Current Latitude\n");
         scanf("%f",&o_lat);
         printf("Enter Current Longitude\n");
         scanf("%f",&o_long);
         printf("Enter Nearest Latitude\n");
         scanf("%f",&d_lat);
         printf("Enter Nearest Longitude\n");
         scanf("%f",&d_long);
         printf("Enter Alternate Latitude 1\n");
         scanf("%f",&temp_lat1);
         printf("Enter Alternate Longitude 1\n");
         scanf("%f",&temp_long1);
         printf("Enter Alternate Latitude 2\n");
         scanf("%f",&temp_lat2);
         printf("Enter Alternate Longitude 2\n");
         scanf("%f",&temp_long2);
         printf("The correct border is %d\n",borderselect(o_lat,o_long,d_lat,d_long,temp_lat1,temp_lat2,temp_long1,temp_long2));
         break;

  default:printf("Invalid Option\n");
 }
}

