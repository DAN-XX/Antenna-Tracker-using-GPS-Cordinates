//BAUF RATE 56700
#include<math.h>
#include<TinyGPS.h>
#include <SoftwareSerial.h>
#include<Servo.h>

TinyGPS gps;
Servo servo;
float distGndToMav,distMavToHead;
double headLat,headLong;
double baseLat,baseLong;
bool initialised=false;
float angles[2];
float baseAlt;
int prevRot;
float lat,lon,alt;
unsigned long age;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  Serial1.begin(57600);
  servo.attach(8);
  baseLat=12.969085927706127;
  baseLong=79.15576457977296;
  baseAlt=100;  
  servo.write(90);
  prevRot=90;
}

//lat:75:lon:9387:alt:3837
int rot;

String val;
String parsed1,parsed2,parsed3;
int index1,index2,index3,index4,index5;
int dir;
void loop(){
  if(Serial.available()>0){
    val=Serial.readStringUntil("\n");    
    Serial.print(val);
    index1=val.indexOf(":");
    index2=val.indexOf(":",index1+1);
    parsed1=val.substring(index1+1,index2);
    index3=val.indexOf(":",index2+1);
    index4=val.indexOf(":",index3+1);
    parsed2=val.substring(index3+1,index4);
    index5=val.indexOf(":",index4+1);
    parsed3=val.substring(index5+1,val.length()-1);
    Serial.print(parsed1);
    Serial.print(parsed2);
    Serial.print(parsed3);
    lat=parsed1.toFloat() ;
    lon=parsed2.toFloat();
    alt=parsed3.toFloat();
    if(initialised){
        float dist1=getDistanceBWPoints(headLat,headLong,lat,lon);
        float dist2=getDistanceBWPoints(baseLat,baseLong,lat,lon);
        angles[0]=atan(dist1/dist2)*180/PI;
        angles[1]=atan(alt/dist2)*180/PI;
        if(headLong>lon){
          dir=1;
          Serial.println("Opposite direction");
        }
        else{
          dir=0;
          Serial.println("Same direction");
        }
        Serial.println(angles[0]);
        Serial.println(prevRot);
        rot=angles[0]-prevRot;
        Serial.println(rot);
        
        if(dir){
          for(int i=prevRot;i>prevRot-rot;i-=5){
            servo.write(i);
            delay(15);
          }
          prevRot=-rot+prevRot;
        }else{
          for(int i=prevRot;i<prevRot+rot;i+=5){
            servo.write(i);
            delay(15);
          }
          prevRot=rot+prevRot;
        }
        
        headLat=lat;
        headLong=lon;
        Serial.println("****");    
      } else{
        prevRot=0;
        headLat=lat;
        headLong=lon;
        initialised=true;
        Serial.println("Initialised");
      }
  }
}

float getDistanceBWPoints(float lat1,float long1,float lat2,float long2){
  float a1=sin((lat2*PI/180-lat1*PI/180)/2)*sin((lat2*PI/180-lat1*PI/180)/2)+cos(lat1*PI/180)*cos(lat2*PI/180)*sin((long2*PI/180-long1*PI/180)/2);
  float a2=1-sin((lat2*PI/180-lat1*PI/180)/2)*sin((lat2*PI/180-lat1*PI/180)/2)+cos(lat1*PI/180)*cos(lat2*PI/180)*sin((long2*PI/180-long1*PI/180)/2);
  if(a1<0){
    a1=-a1;
  }
  if(a2<0){
    a2=-a2;
  }
  float dist=2*atan2(sqrt(a1),sqrt(a2))*63781000;
  return dist;
}

