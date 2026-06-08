const int EMG_PIN = 34;

float relaxedBaseline;
float weakLevel;
float strongLevel;

float weakThreshold;
float strongThreshold;

float filteredActivity = 0;

// ======================================================
// Wait for a specific Serial command
// ======================================================
void waitForCommand(char command)
{
  while (true)
  {
    if (Serial.available())
    {
      char received = toupper(Serial.read());

      if (received == command)
      {
        while (Serial.available())
        {
          Serial.read();
        }

        return;
      }
    }
  }
}

// ======================================================
// Measure average raw EMG value
// ======================================================
float measureAverage(int durationMs)
{
  unsigned long startTime = millis();

  long count = 0;
  float total = 0;

  while (millis() - startTime < durationMs)
  {
    total += analogRead(EMG_PIN);
    count++;

    delay(5);
  }

  return total / count;
}

// ======================================================
// Measure average muscle activity
// ======================================================
float measureActivity(int durationMs)
{
  unsigned long startTime = millis();

  long count = 0;
  float total = 0;

  while (millis() - startTime < durationMs)
  {
    int raw = analogRead(EMG_PIN);

    total += abs(raw - relaxedBaseline);

    count++;

    delay(5);
  }

  return total / count;
}

// ======================================================
// Setup
// ======================================================
void setup()
{
  Serial.begin(115200);

  delay(2000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("EMG FLEX DETECTION SYSTEM");
  Serial.println("Type T and press Enter to begin.");
  Serial.println("====================================");

  waitForCommand('T');

  Serial.println();
  Serial.println("Starting calibration...");
  delay(1000);

  // ---------------- RELAXED ----------------

  Serial.println();
  Serial.println("STEP 1");
  Serial.println("Relax your arm.");
  Serial.println("Type R and press Enter.");

  waitForCommand('R');

  relaxedBaseline = measureAverage(2000);

  Serial.print("Relaxed Baseline = ");
  Serial.println(relaxedBaseline);

  // ---------------- WEAK FLEX ----------------

  Serial.println();
  Serial.println("STEP 2");
  Serial.println("Hold a WEAK FLEX.");
  Serial.println("Type W and press Enter.");

  waitForCommand('W');

  weakLevel = measureActivity(2000);

  Serial.print("Weak Flex Level = ");
  Serial.println(weakLevel);

  // ---------------- STRONG FLEX ----------------

  Serial.println();
  Serial.println("STEP 3");
  Serial.println("Hold a STRONG FLEX.");
  Serial.println("Type S and press Enter.");

  waitForCommand('S');

  strongLevel = measureActivity(2000);

  Serial.print("Strong Flex Level = ");
  Serial.println(strongLevel);

  // ---------------- THRESHOLDS ----------------

  weakThreshold = weakLevel * 0.5;
  strongThreshold = (weakLevel + strongLevel) / 2.0;

  Serial.println();
  Serial.println("====================================");
  Serial.println("CALIBRATION COMPLETE");
  Serial.println("====================================");

  Serial.print("Weak Threshold = ");
  Serial.println(weakThreshold);

  Serial.print("Strong Threshold = ");
  Serial.println(strongThreshold);

  Serial.println();
  Serial.println("System Running...");
}

// ======================================================
// Main Loop
// ======================================================
void loop()
{
  int raw = analogRead(EMG_PIN);

  float activity = abs(raw - relaxedBaseline);

  filteredActivity =
      (0.9 * filteredActivity) +
      (0.1 * activity);

  if (filteredActivity < weakThreshold)
  {
    Serial.println("RELAXED");
  }
  else if (filteredActivity < strongThreshold)
  {
    Serial.println("WEAK FLEX");
  }
  else
  {
    Serial.println("STRONG FLEX");
  }

  delay(500);
}