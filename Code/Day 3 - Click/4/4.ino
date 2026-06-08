const int EMG_PIN = 34;

const int WEAK_THRESHOLD = 100;
const int STRONG_THRESHOLD = 300;

float baseline = 0;
float smoothedEMG = 0;

bool flexDetected = false;

void setup()
{
  Serial.begin(115200);

  Serial.println("Calibrating...");

  long sum = 0;

  for(int i = 0; i < 500; i++)
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

  smoothedEMG = 0.5 * smoothedEMG + 0.5 * emg;

  // Detect new flex
  if(!flexDetected)
  {
    if(smoothedEMG > STRONG_THRESHOLD)
    {
      Serial.println("STRONG FLEX DETECTED");
      flexDetected = true;
    }
    else if(smoothedEMG > WEAK_THRESHOLD)
    {
      Serial.println("WEAK FLEX DETECTED");
      flexDetected = true;
    }
  }

  // Reset after relaxation
  if(flexDetected && smoothedEMG < (WEAK_THRESHOLD * 0.5))
  {
    flexDetected = false;
    Serial.println("READY FOR NEXT FLEX");
  }

  delay(25);
}