const int EMG_PIN = 34;

// ===============================
// THRESHOLDS
// ===============================
const int WEAK_THRESHOLD = 100;
const int STRONG_THRESHOLD = 300;
const int RELEASE_THRESHOLD = 60;

// ===============================
// VARIABLES
// ===============================
float baseline = 0;
float smoothedEMG = 0;

bool gestureActive = false;

// ===============================
// SETUP
// ===============================
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

  Serial.println("System Ready");
}

// ===============================
// LOOP
// ===============================
void loop()
{
  int raw = analogRead(EMG_PIN);

  // Muscle activity relative to baseline
  float emg = abs(raw - baseline);

  // Smooth signal
  smoothedEMG = 0.8 * smoothedEMG + 0.2 * emg;

  // Plotter event channels
  int leftClickPlot = 0;
  int rightClickPlot = 0;

  // ==========================
  // FLEX DETECTION
  // ==========================
  if (!gestureActive)
  {
    if (smoothedEMG > STRONG_THRESHOLD)
    {
      Serial.println("RIGHT CLICK DETECTED");

      rightClickPlot = 500;

      gestureActive = true;
    }
    else if (smoothedEMG > WEAK_THRESHOLD)
    {
      Serial.println("LEFT CLICK DETECTED");

      leftClickPlot = 500;

      gestureActive = true;
    }
  }

  // ==========================
  // RE-ARM AFTER RELAXATION
  // ==========================
  if (gestureActive && smoothedEMG < RELEASE_THRESHOLD)
  {
    gestureActive = false;

    Serial.println("READY");
  }

  // ==========================
  // SERIAL PLOTTER OUTPUT
  // ==========================
  Serial.print(smoothedEMG);
  Serial.print(",");
  Serial.print(leftClickPlot);
  Serial.print(",");
  Serial.println(rightClickPlot);

  delay(20);
}