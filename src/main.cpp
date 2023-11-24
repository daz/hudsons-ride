#include <Arduino.h>

#define THROTTLE_INPUT_PIN A0
// This is the first pin we found that would write analog properly
#define THROTTLE_OUTPUT_PIN 3
#define THROTTLE_MAX_POT_PIN A2
#define THROTTLE_EXPONENT_POT_PIN A3

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

#define THROTTLE_EXPONENT_POT_MIN 0
#define THROTTLE_EXPONENT_POT_MAX 1023

#define THROTTLE_EXPONENT_MIN 1
#define THROTTLE_EXPONENT_MAX 10

int exponentialMap(int x, int in_min, int in_max, int out_min, int out_max, double exponent);
double floatMap(double x, double in_min, double in_max, double out_min, double out_max);
void drawExponentCurve(double exponent);
void drawMockThrottleCurve();

void setup() {
  Serial.begin(115200);

  pinMode(THROTTLE_INPUT_PIN, INPUT);
  pinMode(THROTTLE_OUTPUT_PIN, OUTPUT);
  pinMode(THROTTLE_MAX_POT_PIN, INPUT);
  pinMode(THROTTLE_EXPONENT_POT_PIN, INPUT);
}

void loop() {
  // Read max speed from potentiometer
  int throttleMax = analogRead(THROTTLE_MAX_POT_PIN);

  // Map throttleMax to the max motor speed
  throttleMax = map(throttleMax, THROTTLE_MAX_POT_MIN, THROTTLE_MAX_POT_MAX, THROTTLE_OUTPUT_MIN, THROTTLE_OUTPUT_MAX);

  // Read throttle input
  int throttleInput = analogRead(THROTTLE_INPUT_PIN);

  // Read throttle exponent from potentiometer
  int throttleExponentPot = analogRead(THROTTLE_EXPONENT_POT_PIN);
  double throttleExponent = floatMap(throttleExponentPot, THROTTLE_EXPONENT_POT_MIN, THROTTLE_EXPONENT_POT_MAX, THROTTLE_EXPONENT_MIN, THROTTLE_EXPONENT_MAX);

  // Map throttle input to throttle output
  int throttleOutput = exponentialMap(throttleInput, THROTTLE_INPUT_MIN, THROTTLE_INPUT_MAX, THROTTLE_OUTPUT_MIN, throttleMax, throttleExponent);

  // Write throttle output to motor
  analogWrite(THROTTLE_OUTPUT_PIN, throttleOutput);
  // analogWrite(THROTTLE_OUTPUT_PIN, throttleInput);

#ifdef TEST_MODE
  drawMockThrottleCurve();
#else
  Serial.print("Max: ");
  Serial.println(throttleMax);
  Serial.print("Exp: ");
  Serial.println(throttleExponent);
  Serial.print("In:  ");
  Serial.println(throttleInput);
  Serial.print("Out: ");
  Serial.println(throttleOutput);
  Serial.println();
  Serial.println();
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
  int maxVal = THROTTLE_INPUT_MAX;

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

void drawMockThrottleCurve() {
  // Draw graph of throttle map every second and add 1 to the exponent to see how it affects the curve
  unsigned long now = millis();
  static unsigned long prevNow = now;

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
}
