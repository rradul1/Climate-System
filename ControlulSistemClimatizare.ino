#include <OneWire.h> 
#include <DallasTemperature.h>


#define ONE_WIRE_BUS 8
#define COOL 4
#define HEAT 5
float SensorValue;
float EditTemperature;
OneWire oneWire(ONE_WIRE_BUS);  
DallasTemperature sensors(&oneWire);
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,13,0,12,0,199,0,13,8,0,
  2,0,13,21,23,5,205,24,31,31,
  79,78,0,79,70,70,0,67,4,79,
  29,30,5,31,8,11,129,0,29,9,
  11,5,16,0,129,0,-4,22,14,4,
  31,83,121,115,116,101,109,0,129,0,
  26,5,47,5,16,84,101,109,112,101,
  114,97,116,117,114,101,32,99,111,110,
  116,114,111,108,0,66,130,72,36,24,
  19,205,24,3,3,13,29,10,26,205,
  24,129,0,-4,31,9,4,31,65,117,
  116,111,0,129,0,-4,49,9,4,31,
  67,111,111,108,0,129,0,-4,40,9,
  4,31,72,101,97,116,0,129,0,65,
  23,45,4,31,67,117,114,101,110,116,
  32,116,101,109,112,101,114,97,116,117,
  114,101,40,194,176,67,41,0,7,36,
  37,14,17,5,31,24,205,11,129,0,
  -4,15,14,4,31,83,101,116,32,116,
  101,109,112,101,114,97,116,117,114,101,
  40,194,176,67,41,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t switch_system; // =1 if switch ON and =0 if OFF 
  uint8_t select_mode; // =0 if select position A, =1 if position B, =2 if position C, ... 
  char edit_temperature[11];
  
    // output variables
  char text_temp[11];  // string UTF8 end zero 
  int8_t level_temperature; // =0..100 level position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 
  
} RemoteXY;
#pragma pack(pop)
#define PIN_SWITCH_SYSTEM 12

void setup() 
{
  RemoteXY_Init (); 
  
  pinMode (PIN_SWITCH_SYSTEM, OUTPUT);
  pinMode (HEAT, OUTPUT);
  pinMode (COOL, OUTPUT);
  digitalWrite(HEAT,HIGH);
  digitalWrite(COOL,HIGH);
  dtostrf(0, 0, 2, RemoteXY.edit_temperature);
  sensors.begin(); 
}

void loop() 
{ 
  RemoteXY_Handler ();
  float SensorValue;
  float EditTemperature;
  if (RemoteXY.connect_flag==0) system_off();
  else
  if (RemoteXY.switch_system==1)
    {   
     sensors.requestTemperatures(); 
     SensorValue = sensors.getTempCByIndex(0);
     dtostrf(SensorValue, 0, 2, RemoteXY.text_temp);
     EditTemperature = atof (RemoteXY.edit_temperature);
     RemoteXY.level_temperature = map(SensorValue,0,40,0,80);
     
     if (RemoteXY.select_mode==0)
     {
        if ((SensorValue<EditTemperature)&&(EditTemperature!=0)) 
        {
          heat_on();     
                  
        }
          else 
          if ((SensorValue>EditTemperature)&&(EditTemperature!=0)) 
          {
            cool_on();                     
          }
          else
          system_off();        
     }
  else 
    if (RemoteXY.select_mode==1) 
          {
          heat_on();  
          dtostrf(NULL, 0, 2, RemoteXY.edit_temperature); 
          }
    else 
    if (RemoteXY.select_mode==2)
          {
          cool_on();
          dtostrf(NULL, 0, 2, RemoteXY.edit_temperature);
          }
    }
   else
   system_off();
}

void system_off()
{
  digitalWrite(COOL,HIGH);
  digitalWrite(HEAT,HIGH);
  dtostrf(NULL, 0, 2, RemoteXY.edit_temperature);
  RemoteXY.level_temperature = map(NULL,0,0,0,0);
}

void cool_on()
{
  digitalWrite(HEAT,HIGH);
  digitalWrite(COOL,LOW);
}
void heat_on()
{
  digitalWrite(HEAT,LOW);
  digitalWrite(COOL,HIGH);
}
