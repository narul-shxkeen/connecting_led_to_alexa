#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------
// ADD WIFI CREDENTIALS HERE
#define WIFI_SSID           "RedBrick IoT"
#define WIFI_PASS           "testbricks"

#define SERIAL_BAUDRATE     115200

#define LED_PIN             10

// Device names for Alexa
#define ID_MY_LED           "The Genius"    // Main device
#define ID_BLINK_LED        "The Genius Blink"  // Device for blinking control

// Variables to control LED behavior
bool ledState = false;
bool blinkMode = false;
unsigned char brightness = 255;
unsigned long lastBlinkTime = 0;
const int blinkInterval = 500;  // Blinking interval in milliseconds

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

// Function to handle PWM for dimming that works on both ESP8266 and ESP32
void setLedBrightness(int pin, int brightness) {
    #ifdef ESP32
        // Use standard analogWrite for ESP32
        analogWrite(pin, brightness);
    #else
        // Use standard analogWrite for ESP8266
        analogWrite(pin, brightness);
    #endif
}

// Function to update LED based on current state and brightness
void updateLED() {
    if (!ledState) {
        setLedBrightness(LED_PIN, 0);  // LED off
    } else if (blinkMode) {
        // Don't change LED here, let the loop handle blinking
    } else {
        // Regular dimming mode
        setLedBrightness(LED_PIN, brightness);
    }
}

void setup() {
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // PINs setup
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Wifi
    wifiSetup();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // Add virtual devices
    fauxmo.addDevice(ID_MY_LED);      // Main device for on/off and dimming
    fauxmo.addDevice(ID_BLINK_LED);   // Device for blinking control

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        // Callback when a command from Alexa is received.
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        if (strcmp(device_name, ID_MY_LED) == 0) {
            ledState = state;
            
            // If value is provided (for dimming), use it
            if (value > 0) {
                brightness = value;
            }
            
            // If turning on the main device, turn off blink mode
            if (state) {
                blinkMode = false;
            }
            
            updateLED();
        }
        else if (strcmp(device_name, ID_BLINK_LED) == 0) {
            // Control blinking mode
            if (state) {
                blinkMode = true;
                ledState = true;  // Ensure LED is on for blinking
            } else {
                blinkMode = false;
                // When turning off blink, maintain the regular LED state
                updateLED();
            }
        }
    });
}

void loop() {
    // Handle fauxmoESP
    fauxmo.handle();

    // Handle LED blinking if in blink mode
    if (ledState && blinkMode) {
        unsigned long currentTime = millis();
        if (currentTime - lastBlinkTime >= blinkInterval) {
            lastBlinkTime = currentTime;
            
            // Toggle LED
            static bool blinkState = false;
            blinkState = !blinkState;
            
            if (blinkState) {
                setLedBrightness(LED_PIN, brightness);
            } else {
                setLedBrightness(LED_PIN, 0);
            }
        }
    }

    // Memory reporting
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }
}