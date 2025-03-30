#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

const int debounce_delay = 50; 

const char* ssid = "???";     // Replace with your Wi-Fi name
const char* password = "???"; // Replace with your Wi-Fi password
const char* kitchenlightWindowUrl = "http://192.168.178.108/relay/0?turn=toggle";
const char* kitchenlightCounterUrl = "http://192.168.178.105/relay/0?turn=toggle"; 
const char* garagelightUrl = "http://192.168.178.106/relay/0?turn=toggle"; 

class Button {
  private:
    int pin;
    int prev_button_state;
    int button_state;
    unsigned long last_debounce_time; 
    std::function<void()> callback;
  public:
    Button(int pin, std::function<void()> callback) {
      pinMode(pin, INPUT_PULLUP);
      this->pin = pin;
      this->callback = callback;
      prev_button_state = LOW;
      last_debounce_time = millis();  
    }
    void check() {
      int reading = digitalRead(pin);
      if (reading != prev_button_state) {
        last_debounce_time = millis();
      }

      if ((millis() - last_debounce_time) > debounce_delay) {
        if (button_state != reading) {
          button_state = reading;

          if (button_state == LOW) {
            callback();
          }
        }
      }
      prev_button_state = reading;
    }
};

void sendHttpRequest(const char* url) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wifiClient;
    HTTPClient http;
    http.begin(wifiClient, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
    } else {
      Serial.printf("HTTP Request failed: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

Button button1 = Button(D7, []() {sendHttpRequest(kitchenlightWindowUrl);});
Button button2 = Button(D1, []() {sendHttpRequest(kitchenlightCounterUrl);});
Button button3 = Button(D2, []() {sendHttpRequest(garagelightUrl);});
Button button4 = Button(D3, []() {Serial.println("Button 4 pressed");});

void setup() {
  Serial.begin(115200);
}

void loop() {
  button1.check();
  button2.check();
  button3.check();
  button4.check();
}
