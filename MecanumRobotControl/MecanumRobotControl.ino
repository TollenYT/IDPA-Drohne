#include <SoftwareSerial.h>

// Front Right Motor Pins
#define FR_PWM 10  // PWM control for speed
#define FR_IN1 11  // Direction control 1

// Front Left Motor Pins
#define FL_PWM 9  // PWM control for speed
#define FL_IN1 8  // Direction control 1

// Back Left Motor Pins
#define RL_PWM 5  // PWM control for speed
#define RL_IN1 4  // Direction control 1

// Back Right Motor Pins
#define RR_PWM 6  // PWM control for speed
#define RR_IN1 7  // Direction control 1

#define espSerial Serial1  // Use hardware serial

struct JoystickData {
  int x;
  int y;
  int rotation;
  int speed;
};

float motorCalibration[4] = { 1.0, 1.0, 1.0, 1.0 };  // FL, FR, RL, RR

JoystickData currentCommand;

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);

  // Initialize motor pins
  pinMode(FL_PWM, OUTPUT);
  pinMode(FL_IN1, OUTPUT);
  pinMode(FR_PWM, OUTPUT);
  pinMode(FR_IN1, OUTPUT);
  pinMode(RL_PWM, OUTPUT);
  pinMode(RL_IN1, OUTPUT);
  pinMode(RR_PWM, OUTPUT);
  pinMode(RR_IN1, OUTPUT);

  Serial.println("Arduino Mega Mecanum Wheel Controller Ready");
}

void loop() {
  if (espSerial.available() > 0) {
    String data = espSerial.readStringUntil('\n');
    data.trim();  // Remove any whitespace or newlines
    if (data.length() > 0) {
      processCommand(data);
    } else {
      Serial.println("Warning: Received empty line");
    }
  }
}

void processCommand(String data) {
  Serial.println("\n📥 Received Command -------------------------");
  Serial.print("Raw: '");
  Serial.print(data);
  Serial.println("'");

  // Check if it's a ROT command
  if (data.startsWith("ROT:")) {
    int colon1 = data.indexOf(':');
    int colon2 = data.indexOf(':', colon1 + 1);

    if (colon1 != -1 && colon2 != -1) {
      String rStr = data.substring(colon1 + 1, colon2);
      String sStr = data.substring(colon2 + 1);
      rStr.trim();
      sStr.trim();

      currentCommand.rotation = rStr.toInt();
      currentCommand.speed = sStr.toInt();

      // Default x and y to 0 for rotation-only
      currentCommand.x = 0;
      currentCommand.y = 0;

      if (abs(currentCommand.speed) < 20) currentCommand.speed = 0;

      Serial.println("✅ Parsed ROTATION Command:");
      Serial.print("  R: ");
      Serial.println(currentCommand.rotation);
      Serial.print("  S: ");
      Serial.println(currentCommand.speed);

      calculateMecanumWheels();
      return;
    } else {
      Serial.println("❌ Error: Invalid ROT format. Expected ROT:R:S");
      return;
    }
  }

  // Expected format: "X:Y:R:S" (e.g., "0:100:0:255")
  int parts[4] = { 0 };
  int partIndex = 0;
  int lastColon = -1;

  for (int i = 0; i <= data.length(); i++) {
    if (i == data.length() || data.charAt(i) == ':') {
      String part = data.substring(lastColon + 1, i);
      part.trim();
      if (part.length() > 0) {
        parts[partIndex] = part.toInt();
      }
      partIndex++;
      lastColon = i;
      if (partIndex >= 4) break;
    }
  }

  if (partIndex == 4) {
    currentCommand.x = parts[0];
    currentCommand.y = parts[1];
    currentCommand.rotation = parts[2];
    currentCommand.speed = parts[3];

    // Add deadzone near center
    if (abs(currentCommand.speed) < 20) currentCommand.speed = 0;

    Serial.println("✅ Parsed Joystick Values:");
    Serial.print("  X: ");
    Serial.println(currentCommand.x);
    Serial.print("  Y: ");
    Serial.println(currentCommand.y);
    Serial.print("  R: ");
    Serial.println(currentCommand.rotation);
    Serial.print("  S: ");
    Serial.println(currentCommand.speed);

    calculateMecanumWheels();
  } else {
    Serial.println("❌ Error: Invalid command format. Expected X:Y:R:S");
  }
}

void calculateMecanumWheels() {
  // Normalize inputs (-100 to 100 → -1.0 to 1.0)
  float x = currentCommand.x / 100.0;
  float y = currentCommand.y / 100.0;
  float r = currentCommand.rotation / 100.0;  // Rotation input

  // Calculate speed
  float speed = currentCommand.speed / 255.0;

  float fl = speed * (+1 * x + 1 * y + r);
  float fr = speed * (-1 * x + 1 * y - r);
  float rl = speed * (-1 * x + 1 * y + r);
  float rr = speed * (+1 * x + 1 * y - r);

  // Normalize to maintain ratios while keeping within [-1.0, 1.0]
  float maxVal = max(max(abs(fl), abs(fr)), max(abs(rl), abs(rr)));
  if (maxVal > 1.0) {
    fl /= maxVal;
    fr /= maxVal;
    rl /= maxVal;
    rr /= maxVal;
  }

  // Scale to PWM range (-255 to 255)
  int flPWM = round(fl * 255 * motorCalibration[0]);
  int frPWM = round(fr * 255 * motorCalibration[1]);
  int rlPWM = round(rl * 255 * motorCalibration[2]);
  int rrPWM = round(rr * 255 * motorCalibration[3]);

  // Debug output
  Serial.println("Motor Speeds:");
  Serial.print("FL: ");
  Serial.print(flPWM);
  Serial.print("\t");
  Serial.print("FR: ");
  Serial.print(frPWM);
  Serial.print("\t");
  Serial.print("RL: ");
  Serial.print(rlPWM);
  Serial.print("\t");
  Serial.print("RR: ");
  Serial.println(rrPWM);

  // Apply to motors
  setMotor(FL_PWM, FL_IN1, flPWM);
  setMotor(FR_PWM, FR_IN1, frPWM);
  setMotor(RL_PWM, RL_IN1, rlPWM);
  setMotor(RR_PWM, RR_IN1, rrPWM);
}

void setMotor(int pwmPin, int in1, int speed) {
  // Constrain the speed to be within the valid range
  speed = constrain(speed, -255, 255);

  // Set direction for forward and reverse motion
  if (speed > 0) {
    digitalWrite(in1, HIGH);  // Forward direction (IN1 high)
    speed = map(abs(speed), 0, 255, 255, 0);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);  // Reverse direction (IN1 low)
  } else {
    digitalWrite(in1, LOW);  // Stop the motor (IN1 low)
  }

  Serial.print("speed: ");
  Serial.print(speed);

  // Set the PWM signal for speed control
  analogWrite(pwmPin, abs(speed));  // Use the absolute value of speed
}
