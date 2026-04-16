#include <Arduino.h>

// pins that drive the ten LEDs in the order you listed
const uint8_t ledPins[] = {4, 13, 18, 19, 21, 25, 26, 27, 32, 33};
const size_t NUM_LEDS = sizeof(ledPins) / sizeof(ledPins[0]);

// helper that updates the LED bar according to a 0–100 percentage
void updateLEDs(int percentage) {
  // constrain percentage just in case
  percentage = constrain(percentage, 0, 100);

  // map 0..100 to 0..NUM_LEDS
  int ledsToLight = map(percentage, 0, 100, 0, NUM_LEDS);

  for (size_t i = 0; i < NUM_LEDS; ++i) {
    if (i < ledsToLight) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
}

// when charging is true we briefly fill the remaining LEDs and then return
// to the normal percentage display
// performs a single "fill" of the remaining LEDs; returns early if new
// serial data arrives so that the state can be updated immediately.
void chargingAnimation(int percentage) {
  int start = map(constrain(percentage, 0, 100), 0, 100, 0, NUM_LEDS);
  for (int i = start; i < (int)NUM_LEDS; ++i) {
    digitalWrite(ledPins[i], HIGH);
    // small delay between steps
    delay(1000);
    // if new data has arrived, bail out so the main loop can process it
    if (Serial.available()) {
      return;
    }
  }
  delay(200);
  updateLEDs(percentage);
}

void setup() {
  // configure serial so we can receive the battery percentage
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial monitor to open
  }

  // configure all of the LED pins as outputs and turn them off
  for (size_t i = 0; i < NUM_LEDS; ++i) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  Serial.println("LED Battery Level Monitor ready");
  Serial.println("Send a number between 0 and 100 to light the LEDs");
}

void loop() {
  static String incoming;
  static int lastPercentage = 0;
  static bool charging = false;

  // consume any available serial data and update state
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (incoming.length() > 0) {
        String s = incoming;
        incoming = "";
        int percentage = 0;
        bool ch = false;
        int comma = s.indexOf(',');
        if (comma >= 0) {
          percentage = s.substring(0, comma).toInt();
          ch = s.substring(comma + 1).toInt() != 0;
        } else {
          percentage = s.toInt();
        }
        lastPercentage = percentage;
        charging = ch;
        Serial.print("battery=");
        Serial.print(percentage);
        if (charging) Serial.print(" (charging)");
        Serial.print(" -> ");
        Serial.print(map(percentage, 0, 100, 0, NUM_LEDS));
        Serial.println(" LEDs on");
      }
    } else if (isDigit(c) || c == ',') {
      incoming += c;
    }
    // ignore other characters
  }

  updateLEDs(lastPercentage);

  // display or animate according to the most recent state
  if (charging) {
    chargingAnimation(lastPercentage);
  }
}

