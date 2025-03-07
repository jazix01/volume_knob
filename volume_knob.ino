#include <RotaryEncoder.h>
#include "HID-Project.h"

const bool DEBUG = false;
const int ROTARYPIN1 = 1;
const int ROTARYPIN2 = 3;
const int ROTARYGND = 2;
const int ROTARYBTN = 10;
const int BTNDOUBLETIME = 500;

RotaryEncoder encoder(ROTARYPIN1, ROTARYPIN2, RotaryEncoder::LatchMode::FOUR3);
bool lastBtnState = false;
int btnTime = 0;
int btnCount = 0;

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  pinMode(ROTARYBTN, INPUT_PULLUP); // Button setup
  pinMode(ROTARYGND, OUTPUT); // Fake ground pin setup
  digitalWrite(ROTARYGND, false);

  Consumer.begin();
}

void loop() {
  encoder.tick(); // Fetch new encoder position data
  RotaryEncoder::Direction rotaryDirection = encoder.getDirection();
  bool currentBtnState = !digitalRead(ROTARYBTN);

  // If the encoder has been turned, increase the volume for clockwise turns and decrease it for counter-clockwise turns
  if (rotaryDirection != RotaryEncoder::Direction::NOROTATION) {
    if (DEBUG) {
      Serial.print("Rotary knob rotation value: ");
      Serial.print((int)rotaryDirection);
      Serial.print(" -- ");
    }

    if (rotaryDirection == RotaryEncoder::Direction::CLOCKWISE) {
      if (DEBUG) {
        Serial.println("Increasing volume");
      }

      Consumer.write(MEDIA_VOLUME_UP);
    }
    else {
      if (DEBUG) {
        Serial.println("Decreasing volume");
      }

      Consumer.write(MEDIA_VOLUME_DOWN);
    }
  }

  // If the button was pressed once, and enough time has passed, trigger a play/pause command
  if (btnCount > 0 && millis() - btnTime >= BTNDOUBLETIME) {
    if (DEBUG) {
      Serial.println("Play/pause");
    }

    btnCount = 0;
    Consumer.write(MEDIA_PLAY_PAUSE);
  }

  // If the encoder button is pressed, record the button press
  // If this is the second button press in the alotted time, trigger the double press action
  // Debounce this button press with a 20ms check to prevent duplicate detections
  if (currentBtnState != lastBtnState) {
    if (currentBtnState == false && (millis() - btnTime) > 20) {
      if (DEBUG) {
        Serial.println("button pressed");
      }

      btnCount++;
      btnTime = millis();

      if (btnCount >= 2) {
        if (DEBUG) {
          Serial.println("Next");
          Serial.println(btnCount);
        }

        btnCount = 0;
        Consumer.write(MEDIA_NEXT);
      }
    }

    lastBtnState = currentBtnState;
  }
}