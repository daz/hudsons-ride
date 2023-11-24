#include <Arduino.h>

#define THROTTLE_INPUT_PIN A0
// This is the first pin we found that would write analog properly
#define THROTTLE_OUTPUT_PIN 3
#define THROTTLE_MAX_POT_PIN A2
#define THROTTLE_RAMP_DELAY_POT_PIN A3

// These are the values we get between min and max on the throttle, which works out as 0.88-4.23v
#define THROTTLE_INPUT_MIN 180
#define THROTTLE_INPUT_MAX 866

// Motor controller takes a 1-4v signal
// Matching the throttle's 0.88-4.23v gives us 45-204 as 8 bits
// 45 to 216 is gives the exact voltage output as we read from the throttle input
#define THROTTLE_OUTPUT_MIN 45
#define THROTTLE_OUTPUT_MAX 216

#define THROTTLE_MAX_POT_MIN 0
#define THROTTLE_MAX_POT_MAX 1023

#define THROTTLE_RAMP_DELAY_POT_MIN 0
#define THROTTLE_RAMP_DELAY_POT_MAX 1023

#define THROTTLE_RAMP_DELAY_MIN 0
#define THROTTLE_RAMP_DELAY_MAX 50

// Declare possible ramp states
enum RampState {
  Accelerating,
  Decelerating
};

int currentSpeed = 0;
unsigned long prevNow = millis();

void setup() {
  Serial.begin(115200);

  pinMode(THROTTLE_INPUT_PIN, INPUT);
  pinMode(THROTTLE_OUTPUT_PIN, OUTPUT);
  pinMode(THROTTLE_MAX_POT_PIN, INPUT);
  pinMode(THROTTLE_RAMP_DELAY_POT_PIN, INPUT);
}

void loop() {
  // Read max speed from potentiometer
  int throttleMax = analogRead(THROTTLE_MAX_POT_PIN);

  // Map throttleMax to the max motor speed
  throttleMax = map(throttleMax, THROTTLE_MAX_POT_MIN, THROTTLE_MAX_POT_MAX, THROTTLE_OUTPUT_MIN, THROTTLE_OUTPUT_MAX);

  // Read throttle input
  int throttleInput = analogRead(THROTTLE_INPUT_PIN);

  // Read throttle ramp delay
  int throttleRampDelayVal = analogRead(THROTTLE_RAMP_DELAY_POT_PIN);
  int throttleRampDelay = map(throttleRampDelayVal, THROTTLE_RAMP_DELAY_POT_MIN, THROTTLE_RAMP_DELAY_POT_MAX, THROTTLE_RAMP_DELAY_MIN, THROTTLE_RAMP_DELAY_MAX);

  // Determine if we're accelerating or decelerating
  RampState rampState;
  if (throttleInput >= currentSpeed) {
    rampState = Accelerating;
  } else {
    rampState = Decelerating;
  }

  // Ramp up/down the speed
  unsigned long now = millis();
  if (now - prevNow >= throttleRampDelay) {
    if (rampState == Accelerating) {
      currentSpeed++;
    } else {
      currentSpeed--;
    }

    prevNow = now;
  }

  // Map throttle input to the max motor speed
  throttleInput = map(throttleInput, THROTTLE_INPUT_MIN, THROTTLE_INPUT_MAX, THROTTLE_INPUT_MIN, throttleMax);

  // Write throttle input to throttle output
  analogWrite(THROTTLE_OUTPUT_PIN, currentSpeed);
}
