const int EMG_PIN = 34;

// Adjust after testing
const int WEAK_THRESHOLD = 100;
const int STRONG_THRESHOLD = 300;

float baseline = 0;
float smoothedEMG = 0;

void setup() {
  Serial.begin(115200);

  // Calibrate baseline while relaxed
  long sum = 0;
  for(int i = 0; i < 500; i++) {
    sum += analogRead(EMG_PIN);
    delay(2);
  }

  baseline = sum / 500.0;

  Serial.println("Calibration Complete");
  Serial.print("Baseline: ");
  Serial.println(baseline);
}

void loop() {

  int raw = analogRead(EMG_PIN);

  // Distance from resting value
  float emg = abs(raw - baseline);

  // Smooth signal
  smoothedEMG = 0.9 * smoothedEMG + 0.1 * emg;

  if(smoothedEMG > STRONG_THRESHOLD) {
    Serial.println("STRONG FLEX");
  }
  else if(smoothedEMG > WEAK_THRESHOLD) {
    Serial.println("WEAK FLEX");
  }
  else {
    Serial.println("RELAXED");
  }

  delay(50);
}