const int EMG_PIN = 34;

float relaxedLevel;
float weakLevel;
float strongLevel;

float weakThreshold;
float strongThreshold;
float releaseThreshold;

bool flexDetected = false;

unsigned long flexStartTime = 0;
const unsigned long CONFIRM_TIME = 200;

float filteredValue = 0;

// ======================================================
// Wait for Serial Command
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
// Measure Average Sensor Value
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

  // ---------------- RELAXED ----------------

  Serial.println();
  Serial.println("STEP 1");
  Serial.println("Relax your arm.");
  Serial.println("Type R and press Enter.");

  waitForCommand('R');

  relaxedLevel = measureAverage(2000);

  Serial.print("Relaxed Level = ");
  Serial.println(relaxedLevel);

  // ---------------- WEAK FLEX ----------------

  Serial.println();
  Serial.println("STEP 2");
  Serial.println("Hold a WEAK FLEX.");
  Serial.println("Type W and press Enter.");

  waitForCommand('W');

  weakLevel = measureAverage(2000);

  Serial.print("Weak Flex Level = ");
  Serial.println(weakLevel);

  // ---------------- STRONG FLEX ----------------

  Serial.println();
  Serial.println("STEP 3");
  Serial.println("Hold a STRONG FLEX.");
  Serial.println("Type S and press Enter.");

  waitForCommand('S');

  strongLevel = measureAverage(2000);

  Serial.print("Strong Flex Level = ");
  Serial.println(strongLevel);

  // ==================================================
  // Calculate thresholds for INVERTED sensor output
  // Relaxed = highest value
  // Strong flex = lowest value
  // ==================================================

  weakThreshold =
      (relaxedLevel + weakLevel) / 2.0;

  strongThreshold =
      (weakLevel + strongLevel) / 2.0;

  releaseThreshold =
      (relaxedLevel + weakThreshold) / 2.0;

  filteredValue = relaxedLevel;

  Serial.println();
  Serial.println("====================================");
  Serial.println("CALIBRATION COMPLETE");
  Serial.println("====================================");

  Serial.print("Weak Threshold = ");
  Serial.println(weakThreshold);

  Serial.print("Strong Threshold = ");
  Serial.println(strongThreshold);

  Serial.print("Release Threshold = ");
  Serial.println(releaseThreshold);

  Serial.println();
  Serial.println("System Running...");
}

// ======================================================
// Main Loop
// ======================================================
void loop()
{
  int raw = analogRead(EMG_PIN);

  // Smooth sensor signal
  filteredValue =
      (0.8 * filteredValue) +
      (0.2 * raw);

  // Uncomment for debugging
  /*
  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print("  Filtered: ");
  Serial.println(filteredValue);
  */

  if (!flexDetected)
  {
    if (filteredValue < weakThreshold)
    {
      if (flexStartTime == 0)
      {
        flexStartTime = millis();
      }

      if (millis() - flexStartTime >= CONFIRM_TIME)
      {
        if (filteredValue < strongThreshold)
        {
          Serial.println("STRONG FLEX DETECTED");
        }
        else
        {
          Serial.println("WEAK FLEX DETECTED");
        }

        flexDetected = true;
        flexStartTime = 0;
      }
    }
    else
    {
      flexStartTime = 0;
    }
  }

  // Re-arm once the arm relaxes
  if (flexDetected && filteredValue > releaseThreshold)
  {
    Serial.println("READY FOR NEXT FLEX");
    flexDetected = false;
  }

  delay(25);
}