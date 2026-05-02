#define LASER_PIN 21
#define BAUD 300
#define BIT_US (1000000.0 / BAUD)

void setup() {
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); // idle LOW = laser OFF
  transmitByte(0xAA);
}

void transmitByte(byte b) {
  unsigned long t = micros();
  
  digitalWrite(LASER_PIN, HIGH); // start bit
  t += BIT_US;
  while(micros() < t);
  
  for (int i = 0; i < 8; i++) {
    digitalWrite(LASER_PIN, (b >> i) & 1 ? HIGH : LOW);
    t += BIT_US;
    while(micros() < t);
  }

  digitalWrite(LASER_PIN, LOW); // stop bit
  t += BIT_US * 1.5;
  while(micros() < t);
}

void transmitString(const char* s) {
  while (*s) transmitByte((unsigned char)*s++);
}

void loop() {
  // if (Serial.available() > 0) {
  //   byte incoming = Serial.read();
  //   transmitByte(0xFF);
  //   delay(500);
  // }
}