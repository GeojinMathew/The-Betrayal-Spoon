/*
  THE BETRAYAL SPOON
  ESP32 + HC-SR04 + SG90 Servo + FN-M16P MP3 module

  Requires the "ESP32Servo" library (Sketch > Include Library > Manage Libraries > search "ESP32Servo").
  Do NOT use the built-in Servo.h (it does not support ESP32) or the DFRobot DFPlayer library.
*/

#include <ESP32Servo.h>

// ---------------- Pin definitions ----------------
#define TRIG_PIN   5
#define ECHO_PIN   18
#define SERVO_PIN  19

#define MP3_RX     16   // ESP32 RX2  <- FN-M16P TX
#define MP3_TX     17   // ESP32 TX2  -> FN-M16P RX

// ---------------- Servo angles ----------------
#define SERVO_START_ANGLE   0
#define SERVO_SPILL_ANGLE   180
#define SERVO_FAKE_ANGLE    40    // small jerk only, never reaches spill

// ---------------- Trigger distance ----------------
#define TRIGGER_DISTANCE_CM  10

Servo spoonServo;
HardwareSerial mp3Serial(2);   // UART2

// ---------------- Audio track mapping ----------------
const uint8_t completeSpillTracks[] = {1, 4, 5, 6, 10, 12};
const uint8_t noSpillTracks[]       = {9, 13};
const uint8_t fakeSpillTracks[]     = {2, 7, 8, 11};

bool personPresent = false;

// ================= FN-M16P UART command helpers =================

void mp3SendCommand(uint8_t cmd, uint16_t param) {
  uint8_t frame[10];
  frame[0] = 0x7E;
  frame[1] = 0xFF;
  frame[2] = 0x06;
  frame[3] = cmd;
  frame[4] = 0x00;                   // no feedback requested
  frame[5] = (param >> 8) & 0xFF;    // param high byte
  frame[6] = param & 0xFF;           // param low byte

  uint16_t checksum = 0 - (frame[1] + frame[2] + frame[3] + frame[4] + frame[5] + frame[6]);
  frame[7] = (checksum >> 8) & 0xFF;
  frame[8] = checksum & 0xFF;
  frame[9] = 0xEF;

  mp3Serial.write(frame, 10);
}

void mp3Reset()        { mp3SendCommand(0x0C, 0x0000); }
void mp3SelectSDCard() { mp3SendCommand(0x09, 0x0002); }   // 2 = TF/microSD
void mp3SetVolume(uint8_t vol) { mp3SendCommand(0x06, vol); } // 0-30
void mp3PlayTrack(uint16_t trackNum) { mp3SendCommand(0x03, trackNum); }

// ================= Distance measurement =================

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout (~5 m)
  if (duration == 0) return -1;                   // no echo / out of range

  return duration * 0.0343 / 2;                   // cm
}

// ================= Servo helper =================

void moveServoFast(int fromAngle, int toAngle, int stepDelayMs) {
  if (toAngle > fromAngle) {
    for (int a = fromAngle; a <= toAngle; a += 5) {
      spoonServo.write(a);
      delay(stepDelayMs);
    }
  } else {
    for (int a = fromAngle; a >= toAngle; a -= 5) {
      spoonServo.write(a);
      delay(stepDelayMs);
    }
  }
  spoonServo.write(toAngle);
}

// ================= Outcome actions =================

void doCompleteSpill() {
  moveServoFast(SERVO_START_ANGLE, SERVO_SPILL_ANGLE, 5);   // fast spill
  uint8_t track = completeSpillTracks[random(0, sizeof(completeSpillTracks))];
  mp3PlayTrack(track);
  delay(1500);
  moveServoFast(SERVO_SPILL_ANGLE, SERVO_START_ANGLE, 8);   // reset for next round
}

void doNoSpill() {
  // servo stays put — no movement at all
  uint8_t track = noSpillTracks[random(0, sizeof(noSpillTracks))];
  mp3PlayTrack(track);
}

void doFakeSpill() {
  moveServoFast(SERVO_START_ANGLE, SERVO_FAKE_ANGLE, 4);    // small jerk
  uint8_t track = fakeSpillTracks[random(0, sizeof(fakeSpillTracks))];
  mp3PlayTrack(track);
  delay(300);
  moveServoFast(SERVO_FAKE_ANGLE, SERVO_START_ANGLE, 4);    // back to upright
}

void triggerOutcome() {
  int r = random(0, 100); // 0-99

  if (r < 80) {
    doCompleteSpill();       // 80%
  } else if (r < 90) {
    doNoSpill();              // 10%
  } else {
    doFakeSpill();             // 10%
  }
}

// ================= Setup / Loop =================

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  spoonServo.attach(SERVO_PIN);
  spoonServo.write(SERVO_START_ANGLE);

  mp3Serial.begin(9600, SERIAL_8N1, MP3_RX, MP3_TX);
  delay(1500); // init delay after powering the MP3 module

  mp3Reset();
  delay(1000);
  mp3SelectSDCard();
  delay(300);
  mp3SetVolume(30); // maximum volume
  delay(300);

  randomSeed(analogRead(0));
}

void loop() {
  long distance = readDistanceCM();

  if (distance > 0 && distance <= TRIGGER_DISTANCE_CM) {
    if (!personPresent) {
      personPresent = true;
      triggerOutcome();
    }
  } else if (distance > TRIGGER_DISTANCE_CM || distance == -1) {
    personPresent = false;
  }

  delay(50);
}
