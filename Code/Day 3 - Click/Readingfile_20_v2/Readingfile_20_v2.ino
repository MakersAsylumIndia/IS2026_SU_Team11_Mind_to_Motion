const int EMG_PIN = 34;

// ======================================
// Wait for Serial Command
// ======================================
void waitForCommand(char command)
{
  while (true)s
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

// ======================================
// Record readings for a state
// ======================================
void recordState(const char* stateName)
{
  Serial.println();
  Serial.print("Starting ");
  Serial.print(stateName);
  Serial.println(" test...");

  Serial.println("Recording for 5 seconds:");

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {
    int raw = analogRead(EMG_PIN);

    Serial.println(raw);

    delay(100);
  }

  Serial.println();
  Serial.print(stateName);
  Serial.println(" test complete.");
}

void setup()
{
  Serial.begin(115200);

  delay(2000);

  Serial.println();
  Serial.println("================================");
  Serial.println("EMG TEST MODE");
  Serial.println("================================");
  Serial.println("Type T and press Enter to begin.");
  Serial.println();

  waitForCommand('T');

  Serial.println();
  Serial.println("Test Mode Started");
  Serial.println();
  Serial.println("Type R for RELAXED test");
  Serial.println("Type W for WEAK FLEX test");
  Serial.println("Type S for STRONG FLEX test");
  Serial.println();
}

void loop()
{
  if (Serial.available())
  {
    char cmd = toupper(Serial.read());

    while (Serial.available())
    {
      Serial.read();
    }

    if (cmd == 'R')
    {
      recordState("RELAXED");
    }
    else if (cmd == 'W')
    {
      recordState("WEAK FLEX");
    }
    else if (cmd == 'S')
    {
      recordState("STRONG FLEX");
    }

    Serial.println();
    Serial.println("Type R, W, or S for another test.");
  }
}