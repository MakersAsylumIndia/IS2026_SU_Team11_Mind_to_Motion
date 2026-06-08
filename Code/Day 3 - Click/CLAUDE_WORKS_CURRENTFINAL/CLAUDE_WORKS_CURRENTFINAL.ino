#include <HijelHID_BLEMouse.h>

// ╔══════════════════════════════════════════╗
// ║        EMG BLE Mouse  —  ESP32           ║
// ╚══════════════════════════════════════════╝

// ── Hardware ───────────────────────────────
HijelBLEMouse mouse("EMG Mouse", "MakersCode");
constexpr int EMG_PIN = 34;

// ── Sampling ───────────────────────────────
constexpr unsigned long LOOP_MS = 10;   // 100 Hz  (was 2 Hz with delay(500))

// ── Thresholds (rectified EMG above baseline) ──
constexpr float WEAK_THRESHOLD   = 200.0f;
constexpr float STRONG_THRESHOLD = 400.0f;
constexpr float RELAX_THRESHOLD  =  80.0f;  // must be < WEAK_TH4ESHOLD

// ── Timing ───────────────────────────────
constexpr unsigned long CONFIRM_MS  = 200;  // hold above threshold to confirm flex
constexpr unsigned long COOLDOWN_MS = 400;  // enforced rest after each click

// ── Filter ─────────────────────────────────
// EMA weight on the newest sample.
// Gives ~28 ms time-constant at 100 Hz — good for EMG envelope.
constexpr float EMA_ALPHA = 0.30f;

// ── Debug ──────────────────────────────────
// Set true to stream live EMG values over Serial (e.g. for threshold tuning).
// Set false in production — Serial.print() at 100 Hz costs real loop time.
constexpr bool SERIAL_STREAM = false;

// ═══════════════════════════════════════════

enum class State { IDLE, DETECTING, COOLDOWN };

State state       = State::IDLE;
float baseline    = 0.0f;
float smoothedEMG = 0.0f;
float peakEMG     = 0.0f;          // peak during confirmation window

unsigned long flexStart   = 0;
unsigned long cooldownEnd = 0;

// ── Calibration ────────────────────────────
void calibrate()
{
    Serial.println("Calibrating — keep your arm relaxed...");

    constexpr int N = 1000;
    long sum        = 0;

    for (int i = 0; i < N; i++)
    {
        sum += analogRead(EMG_PIN);
        delay(1);                  // 1 second total, same as before
    }

    baseline = sum / static_cast<float>(N);
    Serial.print("Baseline: ");
    Serial.println(baseline, 1);
}

// ── Click helper ───────────────────────────
void sendClick(MouseButton btn)
{
    if (!mouse.isPaired())
    {
        Serial.println("Not paired — click skipped.");
        return;
    }
    mouse.click(btn);
}

// ── Setup ──────────────────────────────────
void setup()
{
    Serial.begin(115200);
    calibrate();

    mouse.setBatteryLevel(100);
    mouse.begin();

    Serial.println("BLE Mouse ready. Pair with: EMG Mouse");
}

// ── Loop ───────────────────────────────────
void loop()
{
    static unsigned long lastRun = 0;
    const unsigned long  now     = millis();

    if (now - lastRun < LOOP_MS) return;   // enforce 100 Hz cadence
    lastRun = now;

    // ── Sample & filter ────────────────────
    const float raw = static_cast<float>(analogRead(EMG_PIN));
    const float emg = fabsf(raw - baseline);
    smoothedEMG     = EMA_ALPHA * emg + (1.0f - EMA_ALPHA) * smoothedEMG;

    if (SERIAL_STREAM)
    {
        Serial.println(smoothedEMG, 1);    // use Serial Plotter to visualise
    }

    // ── State machine ──────────────────────
    switch (state)
    {
        // ─ Waiting for any flex ──────────────
        case State::IDLE:
            if (smoothedEMG >= WEAK_THRESHOLD)
            {
                state     = State::DETECTING;
                flexStart = now;
                peakEMG   = smoothedEMG;
            }
            break;

        // ─ Confirming flex duration ──────────
        case State::DETECTING:
            // Dropped off before CONFIRM_MS → false positive, ignore
            if (smoothedEMG < RELAX_THRESHOLD)
            {
                state = State::IDLE;
                break;
            }

            // Track the strongest point of this flex
            if (smoothedEMG > peakEMG) peakEMG = smoothedEMG;

            if (now - flexStart >= CONFIRM_MS)
            {
                // Classify by peak strength during the window
                if (peakEMG >= STRONG_THRESHOLD)
                {
                    Serial.println("STRONG FLEX -> RIGHT CLICK");
                    sendClick(MouseButton::Right);
                }
                else
                {
                    Serial.println("WEAK FLEX  -> LEFT CLICK");
                    sendClick(MouseButton::Left);
                }

                state       = State::COOLDOWN;
                cooldownEnd = now + COOLDOWN_MS;
            }
            break;

        // ─ Enforced rest after click ─────────
        // Waits for both muscle relaxation AND a minimum rest period.
        // Prevents a still-tense muscle from triggering a second click.
        case State::COOLDOWN:
            if (smoothedEMG < RELAX_THRESHOLD && now >= cooldownEnd)
            {
                Serial.println("Ready.");
                state = State::IDLE;
            }
            break;
    }
}