// Demo-Code connect an ESP32 to a WiFi-network using stationmode STA

// stationmode is simply join the WiFi-network as your computer or smartphone does it
// the code has three useful functions one for easy identifiying the code in the flash
// one for non-blocking timing
// a heartbeat-blinker function for indicating the state the code is in

// the code is written with two programming rules:
// 1. put EACH functionality into its OWN function
// 2. give EACH function a SELF-explaining name what the function does
// you should follow these programming rules 


#include <WiFi.h>

//const char *ssid     = "ochomovil";
//const char *password = "ochoiphone";

const char *ssid     = "OCHOSCAR";
const char *password = "0303228485";

void scan_networks() 
{
  Serial.println("Scan start");
 
  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d",i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4d", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2d", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i))
      {
        case WIFI_AUTH_OPEN:
          Serial.print("open");
          break;
        case WIFI_AUTH_WEP:
          Serial.print("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.print("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.print("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.print("WPA+WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          Serial.print("WPA2-EAP");
          break;
        case WIFI_AUTH_WPA3_PSK:
          Serial.print("WPA3");
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          Serial.print("WPA2+WPA3");
          break;
        case WIFI_AUTH_WAPI_PSK:
          Serial.print("WAPI");
          break;
        default:
          Serial.print("unknown");
        }
        Serial.println();
        delay(10);
    }
  }
}

void PrintFileNameDateTime() {
  Serial.println( F("Code running comes from file ") );
  Serial.println( F(__FILE__));
  Serial.print( F("  compiled ") );
  Serial.print(F(__DATE__));
  Serial.print( F(" ") );
  Serial.println(F(__TIME__));
}


boolean TimePeriodIsOver (unsigned long &periodStartTime, unsigned long TimePeriod) {
  unsigned long currentMillis  = millis();
  if ( currentMillis - periodStartTime >= TimePeriod )
  {
    periodStartTime = currentMillis; // set new expireTime
    return true;                // more time than TimePeriod) has elapsed since last time if-condition was true
  }
  else return false;            // not expired
}

unsigned long MyTestTimer = 0;                   // variables MUST be of type unsigned long
const byte    OnBoard_LED = 2;

uint8_t actualWiFiStatus;
uint8_t lastWiFiStatus;


void BlinkHeartBeatLED(int IO_Pin, int BlinkPeriod) {
  static unsigned long MyBlinkTimer;
  pinMode(IO_Pin, OUTPUT);
  if ( TimePeriodIsOver(MyBlinkTimer, BlinkPeriod) ) {
    digitalWrite(IO_Pin, !digitalRead(IO_Pin) );
  }
}

void printWiFiStatus(uint8_t p_WiFiStatus) {
  switch (p_WiFiStatus) {
    
    case WL_IDLE_STATUS:
      Serial.println("WL_IDLE_STATUS temporary status assigned when WiFi.begin() is called");
      break;
      
    case WL_NO_SSID_AVAIL:
      Serial.println("WL_NO_SSID_AVAIL   when no SSID are available");
      break;
      
    case WL_SCAN_COMPLETED:
      Serial.println("WL_SCAN_COMPLETED scan networks is completed");
      break;
      
    case WL_CONNECTED:
      Serial.println("WL_CONNECTED  when connected to a WiFi network");
      break;
      
    case WL_CONNECT_FAILED:
      Serial.println("WL_CONNECT_FAILED when the connection fails for all the attempts");
      break;
      
    case WL_CONNECTION_LOST:
      Serial.println("WL_CONNECTION_LOST  when the connection is lost");
      break;
      
    case WL_DISCONNECTED:
      Serial.println("WL_DISCONNECTED when disconnected from a network");
      break;
  }
  Serial.println();
  Serial.println();
}

void ConnectToWiFi() {
  scan_networks();
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  delay(1000);
  //WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
  Serial.println("WiFi.mode(WIFI_STA)");
  int myCount = 0;
  Serial.print("trying to connect to #");
  Serial.print(ssid);
  Serial.println("#");
  WiFi.begin(ssid, password);
  Serial.println("WiFi.begin() done");
  actualWiFiStatus = WiFi.status();
  
  #define maxCount 120
  // Wait for connection
  while (actualWiFiStatus != WL_CONNECTED && myCount < maxCount) {
    actualWiFiStatus = WiFi.status();
    if (lastWiFiStatus != actualWiFiStatus) {
      Serial.println("WiFiStatus changed from");
      printWiFiStatus(lastWiFiStatus);
      Serial.println("to new status");
      printWiFiStatus(actualWiFiStatus);
      lastWiFiStatus = actualWiFiStatus;
    }

    BlinkHeartBeatLED(OnBoard_LED, 50); // blink LED fast during attempt to connect
    yield();
    if ( TimePeriodIsOver(MyTestTimer, 500) ) { // once every 500 miliseconds
      Serial.print(".");                        // print a dot
      myCount++;
      if (myCount > maxCount) { // after maxCount dots restart
        Serial.println();
        Serial.print("not yet connected executing ESP.restart();");
        ESP.restart();
      }
    }
  }
  
  if (WiFi.status() == WL_CONNECTED ) {
    Serial.println("");
    Serial.print("Connected to #");
    Serial.print(ssid);
    Serial.print("# IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println( F("Setup-Start") );
  PrintFileNameDateTime();
  ConnectToWiFi();
}

void PrintHelloMsg() {
  Serial.print( F("Hi there I'm the demo-code my IP address is: ") );
  Serial.println(WiFi.localIP());
}


void loop() {
  BlinkHeartBeatLED(OnBoard_LED, 500); // change blinking to a lower frequency indicating beeing connected
  if ( TimePeriodIsOver(MyTestTimer, 10000) ) {
    PrintHelloMsg();
  }
}
