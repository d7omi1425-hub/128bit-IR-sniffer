const int RECV_PIN = 5;
const int MAX_PULSES = 400; 
uint16_t pulses[MAX_PULSES];

void setup() {
  Serial.begin(115200);
  pinMode(RECV_PIN, INPUT);
  Serial.println("--- 128-Bit Binary Matrix Sniffer ---");
  Serial.println("Waiting for remote signal...");
}

void loop() {
  if (digitalRead(RECV_PIN) == LOW) {
    uint16_t pulseCount = 0;
    unsigned long lastTime = micros();
    bool waitingForHigh = true; 

    // 1. CAPTURE THE TIME
    while (pulseCount < MAX_PULSES) {
      bool currentState = digitalRead(RECV_PIN);
      unsigned long now = micros();

      if ((waitingForHigh && currentState == HIGH) || (!waitingForHigh && currentState == LOW)) {
        pulses[pulseCount] = now - lastTime;
        lastTime = now;
        waitingForHigh = !waitingForHigh;
        pulseCount++;
      }

      if (now - lastTime > 15000) {
        break; 
      }
    }

    Serial.print("\n[SUCCESS] Captured ");
    Serial.print(pulseCount);
    Serial.println(" pulses.");

    // 2. PRINT THE RAW BINARY
    Serial.println("\n[DECODED BINARY (LSB First)]");
    
    int bitCount = 0;
    int byteCount = 0;

    // Start at 'i = 2' to skip the massive Header at the beginning
    for (int i = 2; i < pulseCount - 1; i += 2) {
      uint16_t spaceLength = pulses[i + 1]; 
      
      // If the silence is longer than 1000us, it's a '1'
      if (spaceLength > 1000) {
        Serial.print("1");
      } else {
        Serial.print("0");
      }
      
      bitCount++;
      
      // Formatting: Add a space every 8 bits (1 Byte)
      if (bitCount == 8) {
        Serial.print("  "); 
        bitCount = 0;
        byteCount++;
        
        // Formatting: Drop to a new line every 4 Bytes
        if (byteCount == 4) {
          Serial.println();
          byteCount = 0;
        }
      }
    }
    
    Serial.println("\n");
    delay(2000); // Wait 2 seconds before listening again
  }
}
