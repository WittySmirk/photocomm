#define LASER_PIN 21
#define BAUD 300
#define BIT_US (1000000.0 / BAUD)

void setup() {
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); // idle LOW = laser OFF
}

void transmitByte(byte b) {
  // Start bit: laser ON
  digitalWrite(LASER_PIN, HIGH);
  delayMicroseconds(BIT_US);

  // 8 data bits, LSB first
  for (int i = 0; i < 8; i++) {
    digitalWrite(LASER_PIN, (b >> i) & 1 ? HIGH : LOW);
    delayMicroseconds(BIT_US);
  }

  // Stop bit: laser OFF
  digitalWrite(LASER_PIN, LOW);
  delayMicroseconds(BIT_US * 1.5);
}

void transmitString(const char* s) {
  while (*s) transmitByte((unsigned char)*s++);
}

void loop() {
  if (Serial.available() > 0) {
    byte incoming = Serial.read();
    transmitByte(incoming);
  }
}