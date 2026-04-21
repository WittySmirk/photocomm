void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
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
  for (int i = 7; i >= 0; i--) {
    bool bit = (b >> i) & 1;
    if (bit) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void loop() {
  if (Serial.available() > 0) {
    byte incoming = Serial.read();
    transmitByte(incoming);
  }
}
