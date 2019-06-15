#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <DHT.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#define DHTPIN 14
#define DHTTYPE DHT11  

DHT dht(DHTPIN, DHTTYPE);
Servo servo;
WiFiClient client;
IPAddress server_addr(13,124,166,146);

char ssid[] = "arduino";                 
char pass[] = "Test0101";                
char auth[] = "7ce8169c6a894c919bd1f2b5f36afe0a";
byte mac[6];
int mvalue=0;
int flag=0;
char user[] = "root"; 
char password[] = ""; 
char query[] = "SELECT pop,rain,REH,window,hour,temp FROM rss.weather;";

MySQL_Connection conn((Client *)&client);
MySQL_Cursor cur = MySQL_Cursor(&conn);

BLYNK_WRITE(V7)
  {
    int pinValue = param.asInt();
    if(pinValue == 1)
      mvalue = 70;
    else
      mvalue = 0;
  }
 BLYNK_WRITE(V9)
 {
  mvalue = param.asInt();
 }
BLYNK_WRITE(V4)
  {
    int vflag = param.asInt();
    if(vflag == 1)
      flag = 1;
    else
      flag = 0;
  }

void setup() 
{
  Serial.begin(115200);
  servo.attach(5);
  Serial.println("");
  Serial.print("연결중 :  ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("접속성공");
  Blynk.config(auth);
  
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  Serial.println("");
  Serial.print("할당 아이피: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("연결실패");
}
void loop() 
{
  Blynk.run();
  row_values *row = NULL;
  long pop = 0;
  long rain = 0;
  long REH = 0;
  long window = 0;
  long HOUR = 0;
  long temp = 0;
  int h = dht.readHumidity(); 
  delay(30); 
  int t = dht.readTemperature();
  delay(30); 
  if(h == 2147483647 || t== 2147483647)
  {
    h = 0;
    t = 0;
  }
  delay(1000);
    
  Serial.print("Humidity: ");
  Serial.print(h); 
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");
  
  Blynk.virtualWrite(V4,h);
  Blynk.virtualWrite(V5,t);
  
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  column_names *columns = cur_mem->get_columns();

  do {
    row = cur_mem->get_next_row();
    if (row != NULL) 
    {
      pop = atol(row->values[0]);
      rain = atol(row->values[1]);
      REH = atol(row->values[2]);
      window = atol(row->values[3]);
      HOUR = atol(row->values[4]);
      temp = atol(row->values[5]);
     }
  } while (row != NULL);
  
  delete cur_mem;
  Serial.print("FLAG : ");
  Serial.println(flag);
  Serial.print("시간 = ");
  Serial.print(HOUR-3);
  Serial.print(" ~ ");
  Serial.println(HOUR);
  Serial.print("온도 = ");
  Serial.println(temp);
  Serial.print("강수확률 = ");
  Serial.println(pop);
  Serial.print("비 = ");
  if(rain == 1)
    Serial.println("YES");
  else
    Serial.println("NO");
    
  Serial.print("습도 = ");
  Serial.println(REH);
  Serial.print("창문상태 = ");
  
  Blynk.virtualWrite(V1,temp);
  Blynk.virtualWrite(V0,pop);
  Blynk.virtualWrite(V2,REH);
  
  if(mvalue == 0 || rain == 1)
  {
    window = 1;
    {
    if(rain == 1)
      mvalue=0;
    }
  }
  else
    window = 0;
      
  if(window == 1)
    Serial.println("Close");
  else
    Serial.println("Open");
  Serial.println("--------------------------------------------------");
  
  delay(1000);
  
  cur.execute(query);
  cur.get_columns();
  
  do {
    row = cur.get_next_row();
    if (row != NULL) {
      pop = atol(row->values[0]);
      rain = atol(row->values[1]);
      REH = atol(row->values[2]);
      window = atol(row->values[3]);
      HOUR = atol(row->values[4]);
      temp = atol(row->values[5]); 
    }
  } while (row != NULL);
  cur.close();

  if(flag == 0)
  {
    if(pop >= 70 || window == 1)
    {
      mvalue = 0;
    }
  }
  Blynk.virtualWrite(V6, flag);
  Blynk.virtualWrite(V8, mvalue);
  servo.write(mvalue);
}
