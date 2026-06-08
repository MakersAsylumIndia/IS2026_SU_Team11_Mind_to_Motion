#include <HijelHID_BLEMouse.h>

// =====================================
// BLE Mouse
// =====================================

HijelBLEMouse mouse("EMG Mouse", "MakersCode");

// =====================================
// EMG Settings
// =====================================

const int EMG_PIN = 34;

const int WEAK_THRESHOLD = 100;
const int STRONG_THRESHOLD = 300;

float baseline = 0;
float smoothedEMG = 0;

bool flexDetected = false;

unsigned long flexStartTime = 0;
const unsigned long CONFIRM_TIME = 200; // ms

// =====================================
// Setup
// =====================================

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

    mouse.setBatteryLevel(100);
    mouse.begin();

    Serial.println("Bluetooth Mouse Started");
    Serial.println("Pair with: EMG Mouse");
    Serial.println("Ready.");
}

// =====================================
// Loop
// =====================================

void loop()
{
    int raw = analogRead(EMG_PIN);

    float emg = abs(raw - baseline);

    // Filtering
    smoothedEMG = 0.8 * smoothedEMG + 0.2 * emg;

    // Display live EMG readings
    Serial.print("EMG: ");
    Serial.println(smoothedEMG);

    if (!flexDetected)
    {
        if (smoothedEMG > WEAK_THRESHOLD)
        {
            if (flexStartTime == 0)
            {
                flexStartTime = millis();
            }

            if (millis() - flexStartTime > CONFIRM_TIME)
            {
                // ======================
                // STRONG FLEX
                // ======================

                if (smoothedEMG > STRONG_THRESHOLD)
                {
                    Serial.println("STRONG FLEX DETECTED");

                    if (mouse.isPaired())
                    {
                        mouse.click(MouseButton::Right);
                        Serial.println("RIGHT CLICK SENT");
                    }
                    else
                    {
                        Serial.println("MOUSE NOT PAIRED");
                    }
                }

                // ======================
                // WEAK FLEX
                // ======================

                else
                {
                    Serial.println("WEAK FLEX DETECTED");

                    if (mouse.isPaired())
                    {
                        mouse.click(MouseButton::Left);
                        Serial.println("LEFT CLICK SENT");
                    }
                    else
                    {
                        Serial.println("MOUSE NOT PAIRED");
                    }
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

    // Re-arm only after relaxation
    if (flexDetected && smoothedEMG < 90)
    {
        Serial.println("READY FOR NEXT FLEX");
        flexDetected = false;
    }

    delay(500);
}