#define LASER_PIN 13
#define BAUD 1200
#define BIT_US (1000000.0 / BAUD)

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  Serial.begin(9600);
}

/*
Potential speed update:

void transmitByte(byte b) {
    for (int i = 7; i >= 0; i--) {
        if ((b >> i) & 1)
            PORTB |= (1 << 1);   // pin 9 HIGH (PB1)
        else
            PORTB &= ~(1 << 1);  // pin 9 LOW
        delayMicroseconds(BIT_DELAY_US);
    }
    PORTB &= ~(1 << 1);
}
*/

void transmitByte(byte b) {
  digitalWrite(LASER_PIN, HIGH);
  delayMicroseconds(BIT_US);
  for (int i = 0; i < 8; i++) {
    bool bit = (b >> i) & 1;
    digitalWrite(LASER_PIN, (b >> i) & 1 ? HIGH : LOW);
    delayMicroseconds(BIT_US);
  }
  digitalWrite(LASER_PIN, LOW);
  delayMicroseconds(BIT_US*1.5);
}

void loop() {
  if (Serial.available() > 0) {
    byte incoming = Serial.read();
    transmitByte(incoming);
  }
}
