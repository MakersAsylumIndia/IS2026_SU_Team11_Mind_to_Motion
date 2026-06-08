const int EMG_PIN = 34;

const int WEAK_THRESHOLD = 1800;
const int STRONG_THRESHOLD = 1100;
const int RELEASE_THRESHOLD = 2500;

bool flexDetected = false;

unsigned long flexStartTime = 0;
const unsigned long CONFIRM_TIME = 200;

float filteredValue = 3000;

void setup()
{
  Serial.begin(115200);

  Serial.println("EMG System Ready");
}

void loop()
{
  int raw = analogRead(EMG_PIN);

  // Smooth signal
  filteredValue = 0.8 * filteredValue + 0.2 * raw;

  // Uncomment for debugging
  // Serial.println(filteredValue);

  if (!flexDetected)
  {
    // Strong flex first
    if (filteredValue < STRONG_THRESHOLD)
    {
      if (flexStartTime == 0)
      {
        flexStartTime = millis();
      }

      if (millis() - flexStartTime > CONFIRM_TIME)
      {
        Serial.println("STRONG FLEX DETECTED");
        flexDetected = true;
        flexStartTime = 0;
      }
    }

    // Weak flex
    else if (filteredValue < WEAK_THRESHOLD)
    {
      if (flexStartTime == 0)
      {
        flexStartTime = millis();
      }

      if (millis() - flexStartTime > CONFIRM_TIME)
      {
        Serial.println("WEAK FLEX DETECTED");
        flexDetected = true;
        flexStartTime = 0;
      }
    }

    else
    {
      flexStartTime = 0;
    }
  }

  // Re-arm after relaxation
  if (flexDetected && filteredValue > RELEASE_THRESHOLD)
  {
    Serial.println("READY FOR NEXT FLEX");
    flexDetected = false;
  }

  delay(25);
}