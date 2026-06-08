const int EMG_PIN = 34;

// Adjust after testing
const int WEAK_THRESHOLD = 100;
const int STRONG_THRESHOLD = 300;

// Relaxation threshold
const int RELEASE_THRESHOLD = 60;

float baseline = 0;
float smoothedEMG = 0;

bool flexDetected = false;

unsigned long flexStartTime = 0;
const unsigned long CONFIRM_TIME = 200; // milliseconds

void setup()
{
  Serial.begin(115200);

  Serial.println("Calibrating... Keep arm relaxed.");

  long sum = 0;

  for (int i = 0; i < 500; i++)
  {
    sum += analogRead(EMG_PIN);
    delay(2);
  }

  baseline = sum / 500.0;

  Serial.println("Calibration Complete");
  Serial.print("Baseline: ");
  Serial.println(baseline);

  Serial.println("Ready.");
}

void loop()
{
  int raw = analogRead(EMG_PIN);

  // Difference from resting level
  float emg = abs(raw - baseline);

  // Faster response while still filtering noise
  smoothedEMG = (0.7 * smoothedEMG) + (0.3 * emg);

  // Uncomment for debugging
  /*
  Serial.print("EMG: ");
  Serial.println(smoothedEMG);
  */

  // ===============================
  // FLEX DETECTION
  // ===============================
  if (!flexDetected)
  {
    if (smoothedEMG > WEAK_THRESHOLD)
    {
      if (flexStartTime == 0)
      {
        flexStartTime = millis();
      }

      // Signal must remain above threshold
      if (millis() - flexStartTime >= CONFIRM_TIME)
      {
        if (smoothedEMG > STRONG_THRESHOLD)
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

  // ===============================
  // RELAXATION DETECTION
  // ===============================
  if (flexDetected && smoothedEMG < RELEASE_THRESHOLD)
  {
    Serial.println("READY FOR NEXT FLEX");
    flexDetected = false;
  }

  delay(25);
}