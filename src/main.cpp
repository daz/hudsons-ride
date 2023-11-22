#include <Arduino.h>

#define TEST_MODE 1

#define THROTTLE_INPUT_PIN 7
#define THROTTLE_OUTPUT_PIN 4

#define MAX_SPEED_POT_PIN A0
#define THROTTLE_EXPONENT_POT_PIN A1

#define THROTTLE_INPUT_MIN 0
#define THROTTLE_INPUT_MAX 1023

#define MAX_SPEED_POT_MIN 0
#define MAX_SPEED_POT_MAX 1023

#define THROTTLE_EXPONENT_POT_MIN 0
#define THROTTLE_EXPONENT_POT_MAX 1023

int exponentialMap(int x, int in_min, int in_max, int out_min, int out_max, double exponent);
double floatMap(double x, double in_min, double in_max, double out_min, double out_max);
void drawExponentCurve(double exponent);

unsigned long prevNow = millis();

void setup() {
  Serial.begin(115200);

  pinMode(THROTTLE_INPUT_PIN, INPUT);
  pinMode(THROTTLE_OUTPUT_PIN, OUTPUT);
  pinMode(MAX_SPEED_POT_PIN, INPUT);
  pinMode(THROTTLE_EXPONENT_POT_PIN, INPUT);
}

void loop() {
  // Read max speed from potentiometer
  int throttleMax = analogRead(MAX_SPEED_POT_PIN);
  // Map throttleMax to the max motor speed
  throttleMax = map(throttleMax, MAX_SPEED_POT_MIN, MAX_SPEED_POT_MAX, THROTTLE_INPUT_MIN, THROTTLE_INPUT_MAX);

  // Read throttle input
  int throttleInput = analogRead(THROTTLE_INPUT_PIN);

  // Read throttle exponent from potentiometer
  int throttleExponent = analogRead(THROTTLE_EXPONENT_POT_PIN);

  // Get current time in milliseconds
  unsigned long now = millis();

#ifdef TEST_MODE

  // Draw graph of throttle map every second and add 1 to the exponent to see how it affects the curve
  if (now - prevNow >= 1000) {
    static int mockThrottleExponent = 0;
    static bool mockThrottleExponentIncreasing = true;
    if (mockThrottleExponentIncreasing) {
      mockThrottleExponent++;
      if (mockThrottleExponent >= 10) {
        mockThrottleExponentIncreasing = false;
      }
    } else {
      mockThrottleExponent--;
      if (mockThrottleExponent <= 1) {
        mockThrottleExponentIncreasing = true;
      }
    }
    drawExponentCurve(mockThrottleExponent);
    Serial.print("Throttle exponent: ");
    Serial.println(mockThrottleExponent);
    Serial.println();
    Serial.println();

    prevNow = now;
  }

#endif
}

int exponentialMap(int x, int in_min, int in_max, int out_min, int out_max, double exponent) {
  return (int)(out_min + (out_max - out_min) * pow((x - in_min) / (double)(in_max - in_min), exponent));
}

double floatMap(double x, double in_min, double in_max, double out_min, double out_max) {
  return (out_min + (out_max - out_min) * (x - in_min) / (in_max - in_min));
}

void drawExponentCurve(double exponent) {
  int gridWidth = 87;
  int gridHeight = 40;
  int maxVal = 1023;

  int gridYValues[gridWidth];
  for (int i = 0; i < gridWidth; i++) {
    int xVal = map(i, 0, gridWidth, 0, maxVal);
    gridYValues[i] = exponentialMap(xVal, 0, maxVal, 0, gridHeight, exponent);
  }

  for (int y = gridHeight; y >= 0; y--) {
    for (int x = 0; x < gridWidth; x++) {
      if (gridYValues[x] >= y) {
        Serial.print("#");
      } else {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
}
