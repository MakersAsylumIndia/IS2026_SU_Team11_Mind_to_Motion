const int EMG_PIN = 34;

float relaxedLevel = 0;
float weakLevel = 0;
float strongLevel = 0;

float weakThreshold = 0;
float strongThreshold = 0;

float filteredActivity = 0;

// Measure average activity over a period
float measureActivity(int durationMs, float baseline)
{
  long startTime = millis();
  long samples = 0;
  float total = 0;

  while (millis() - startTime < durationMs)
  {
    int raw = analogRead(EMG_PIN);

    float activity = abs(raw - baseline);

    total += activity;
    samples++;

    delay(5);
  }

  return total / samples;
}

void setup()
{
  Serial.begin(115200);

  Serial.println("=== EMG CALIBRATION ===");

  // ---------- RELAXED ----------
  Serial.println("Keep arm RELAXED for 3 seconds...");
  delay(3000);

  long sum = 0;

  for(int i = 0; i < 500; i++)
  {
    sum += analogRead(EMG_PIN);
    delay(5);
  }

  relaxedLevel = sum / 500.0;

  Serial.print("Relaxed baseline = ");
  Serial.println(relaxedLevel);

  // ---------- WEAK FLEX ----------
  Serial.println("Perform a WEAK FLEX for 3 seconds...");
  delay(3000);

  weakLevel = measureActivity(3000, relaxedLevel);

  Serial.print("Weak flex level = ");
  Serial.println(weakLevel);

  // ---------- STRONG FLEX ----------
  Serial.println("Perform a STRONG FLEX for 3 seconds...");
  delay(3000);

  strongLevel = measureActivity(3000, relaxedLevel);

  Serial.print("Strong flex level = ");
  Serial.println(strongLevel);

  // ---------- THRESHOLDS ----------
  weakThreshold =
      (weakLevel * 0.5);

  strongThreshold =
      (weakLevel + strongLevel) / 2.0;

  Serial.println("Calibration Complete!");

  Serial.print("Weak Threshold = ");
  Serial.println(weakThreshold);

  Serial.print("Strong Threshold = ");
  Serial.println(strongThreshold);
}

void loop()
{
  int raw = analogRead(EMG_PIN);

  float activity = abs(raw - relaxedLevel);

  // Smooth signal
  filteredActivity =
      (0.9 * filteredActivity) +
      (0.1 * activity);

  if(filteredActivity < weakThreshold)
  {
    Serial.println("RELAXED");
  }
  else if(filteredActivity < strongThreshold)
  {
    Serial.println("WEAK FLEX");
  }
  else
  {
    Serial.println("STRONG FLEX");
  }

  delay(100);
}