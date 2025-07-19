#include <WiFi.h>
  #include <HomeSpan.h>
  // to connect to wifi
  const char* ssid = "Wifi name";
  const char* password = "wifi password";
  

  #define TOUCH_PIN 1
  #define RELAY_PIN 7 
  
//makes sure the button isn't accidentaly spammed
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 50;
  bool lastTouchState = false;
  bool currentState = false;  // Current relay state
  

  struct RelaySwitch : Service::Switch {
    Characteristic::On power{0};
  
    RelaySwitch() {
      power.set([this](bool on) {
        currentState = on;
        digitalWrite(RELAY_PIN, currentState ? HIGH : LOW);
        Serial.printf("HomeKit Switch toggled: %s\n", currentState ? "ON" : "OFF");
        return true;
      });
  
      power.setPermission(HOMEKIT_PERM_READ | HOMEKIT_PERM_WRITE | HOMEKIT_PERM_NOTIFY);
    }
  
    void loop() override {
      //not needed as its handled elswhere
    }
  
    void updateHomeKit() {
      power.setVal(currentState);
    }
  };
  
  RelaySwitch *mySwitch;

  void setup() {
    Serial.begin(115200);
  
    // sets up button and relay
    pinMode(TOUCH_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
  
    // connect to wifi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(not connected);
    }
    Serial.println("Connected!");
  
    // HomeSpan Setup
    homeSpan.begin(Category::Switches, "TouchRelay");
    new SpanAccessory();
      new Service::AccessoryInformation();
        new Characteristic::Name("Jakes lamp");
        new Characteristic::Manufacturer("Jake's Makes");
        new Characteristic::SerialNumber("00001");
        new Characteristic::Model("TouchSwitch");
        new Characteristic::FirmwareRevision("1.0");
        new Characteristic::Identify();
      mySwitch = new RelaySwitch();
  }
  
  void loop() {
    homeSpan.poll();
  
    bool reading = digitalRead(TOUCH_PIN) == HIGH;
  
    if (reading != lastTouchState) {
      lastDebounceTime = millis();
    }
  //updates homekit
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != currentState) {
        currentState = reading;
        digitalWrite(RELAY_PIN, currentState ? HIGH : LOW);
        mySwitch->updateHomeKit();
        Serial.printf("Button Press toggled: %s\n", currentState ? "ON" : "OFF");
      }
    }
  
    lastTouchState = reading;
  }
  
