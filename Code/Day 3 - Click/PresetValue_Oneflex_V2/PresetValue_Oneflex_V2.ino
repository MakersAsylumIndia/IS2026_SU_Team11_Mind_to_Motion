const int EMG_PIN = 34;

const int WEAK_THRESHOLD = 100;
const int STRONG_THRESHOLD = 300;

float baseline = 0;
float smoothedEMG = 0;

bool flexDetected = false;

unsigned long flexStartTime = 0;
const unsigned long CONFIRM_TIME = 200; // ms

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

  Serial.print("Baseline: ");
  Serial.println(baseline);

  Serial.println("Ready.");
}

void loop()
{
  int raw = analogRead(EMG_PIN);

  float emg = abs(raw - baseline);

  // Slightly stronger filtering
  smoothedEMG = 0.8 * smoothedEMG + 0.2 * emg;

  if (!flexDetected)
  {
    if (smoothedEMG > WEAK_THRESHOLD)
    {
      if (flexStartTime == 0)
      {
        flexStartTime = millis();
      }

      // Must stay above threshold for 200 ms
      if (millis() - flexStartTime > CONFIRM_TIME)
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

  // Only re-arm after a clear relaxation
  if (flexDetected && smoothedEMG < 30)
  {
    Serial.println("READY FOR NEXT FLEX");
    flexDetected = false;
  }

  delay(25);
}