/*
  B1-Focuser: SVBONY SV48P ESP32-C3 MoonLite focuser

  Based on the protocol behavior of fehlfarbe/arduino-motorfocus:
  https://github.com/fehlfarbe/arduino-motorfocus

  Hardware target:
  - ESP32-C3 board with USB CDC serial
  - 28BYJ-48 5V stepper
  - ULN2003/ULN2003A stepper breakout

  MoonLite commands are read from Serial at 9600 baud. Keep the Serial port
  free of debug output so ASCOM/INDI/Ekos can parse replies reliably.
*/

#include <Arduino.h>
#include <AccelStepper.h>
#include <Preferences.h>
#include "ManualSwitch.h"

// ULN2003 input pins. Wire ESP32-C3 GPIO -> ULN2003 INx as listed here.
static constexpr uint8_t PIN_IN1 = 4;
static constexpr uint8_t PIN_IN2 = 5;
static constexpr uint8_t PIN_IN3 = 6;
static constexpr uint8_t PIN_IN4 = 7;

// Centre-off switch: common to GND, throws to GPIO0 and GPIO1.
static constexpr int PIN_BTN_IN = 0;
static constexpr int PIN_BTN_OUT = 1;
static constexpr float MANUAL_SPEED_STEPS_PER_SEC = 120.0f;
static constexpr long MANUAL_MAX_POSITION = 65535;
ManualSwitch manualSwitch;

// 28BYJ-48 through ULN2003 usually wants IN1, IN3, IN2, IN4 for AccelStepper.
// AccelStepper fixes the step interface at construction time, so keep one
// object for each MoonLite step mode and switch the active object.
AccelStepper fullStepper(AccelStepper::FULL4WIRE, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4, false);
AccelStepper halfStepper(AccelStepper::HALF4WIRE, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4, false);

Preferences prefs;

enum StepMode
{
  STEP_FULL,
  STEP_HALF
};

static constexpr uint32_t SERIAL_BAUD = 9600;
static constexpr float BASE_SPEED_STEPS_PER_SEC = 30.0f;
static constexpr float ACCELERATION_STEPS_PER_SEC2 = 120.0f;
static constexpr uint32_t POSITION_SAVE_IDLE_MS = 5000;
static constexpr uint32_t MOTOR_DISABLE_IDLE_MS = 12000;
static constexpr size_t COMMAND_MAX = 24;

uint8_t speedRaw = 0x04;
float speedFactor = 8.0f;
long targetPosition = 0;
long lastSavedPosition = 0;
uint32_t lastMoveMs = 0;
bool outputsEnabled = false;
StepMode stepMode = STEP_HALF;
StepMode lastSavedStepMode = STEP_HALF;

char commandBuffer[COMMAND_MAX];
size_t commandLength = 0;
bool commandReady = false;

AccelStepper &activeStepper()
{
  return stepMode == STEP_HALF ? halfStepper : fullStepper;
}

long toHalfStepUnits(long position, StepMode mode)
{
  return mode == STEP_HALF ? position : position * 2L;
}

long fromHalfStepUnits(long halfSteps, StepMode mode)
{
  return mode == STEP_HALF ? halfSteps : (halfSteps + 1L) / 2L;
}

void updateSpeed()
{
  speedFactor = 32.0f / max<uint8_t>(speedRaw, 1);
  fullStepper.setMaxSpeed(speedFactor * BASE_SPEED_STEPS_PER_SEC);
  fullStepper.setAcceleration(ACCELERATION_STEPS_PER_SEC2);
  halfStepper.setMaxSpeed(speedFactor * BASE_SPEED_STEPS_PER_SEC);
  halfStepper.setAcceleration(ACCELERATION_STEPS_PER_SEC2);
}

void enableMotor()
{
  if (!outputsEnabled)
  {
    activeStepper().enableOutputs();
    outputsEnabled = true;
  }
  lastMoveMs = millis();
}

void disableMotor()
{
  if (outputsEnabled)
  {
    fullStepper.disableOutputs();
    halfStepper.disableOutputs();
    outputsEnabled = false;
  }
}

long parseHexPosition(const char *text)
{
  return strtol(text, nullptr, 16);
}

void printHexPosition(long value)
{
  char reply[12];
  snprintf(reply, sizeof(reply), "%04lX", max(0L, value));
  Serial.print(reply);
  Serial.print('#');
}

void savePositionIfNeeded()
{
  const long current = max(0L, activeStepper().currentPosition());
  if (current != lastSavedPosition || stepMode != lastSavedStepMode)
  {
    prefs.putLong("pos", current);
    prefs.putBool("half", stepMode == STEP_HALF);
    lastSavedPosition = current;
    lastSavedStepMode = stepMode;
  }
}

void setStepMode(StepMode newMode)
{
  if (newMode == stepMode)
  {
    return;
  }

  AccelStepper &oldStepper = activeStepper();
  if (oldStepper.distanceToGo() != 0)
  {
    return;
  }

  const long currentHalfSteps = toHalfStepUnits(max(0L, oldStepper.currentPosition()), stepMode);
  const long targetHalfSteps = toHalfStepUnits(max(0L, targetPosition), stepMode);
  const bool shouldEnableOutputs = outputsEnabled;

  oldStepper.disableOutputs();
  stepMode = newMode;

  AccelStepper &newStepper = activeStepper();
  newStepper.setCurrentPosition(fromHalfStepUnits(currentHalfSteps, stepMode));
  targetPosition = fromHalfStepUnits(targetHalfSteps, stepMode);
  newStepper.moveTo(targetPosition);
  updateSpeed();

  if (shouldEnableOutputs)
  {
    newStepper.enableOutputs();
  }
  else
  {
    newStepper.disableOutputs();
  }
  outputsEnabled = shouldEnableOutputs;
  savePositionIfNeeded();
}

void processCommand(char *line)
{
  if (line[0] == '2')
  {
    ++line; // Some dual-focuser clients prefix motor 1 commands with "2".
  }

  char cmd[3] = {0, 0, 0};
  cmd[0] = line[0];
  cmd[1] = line[1] ? line[1] : '\0';
  const char *param = line + min<size_t>(2, strlen(line));

  // Manual control takes priority. Rejected writes are not queued to run
  // when the switch returns to centre; queries and abort remain available.
  if (manualSwitch.blocksRemoteMotion() &&
      (strcasecmp(cmd, "SP") == 0 || strcasecmp(cmd, "SN") == 0 ||
       strcasecmp(cmd, "FG") == 0 || strcasecmp(cmd, "PH") == 0 ||
       strcasecmp(cmd, "SH") == 0 || strcasecmp(cmd, "SF") == 0 ||
       strcasecmp(cmd, "SD") == 0))
  {
    return;
  }

  if (strcasecmp(cmd, "GB") == 0)
  {
    Serial.print("00#");
  }
  else if (strcasecmp(cmd, "GV") == 0)
  {
    Serial.print("10#");
  }
  else if (strcasecmp(cmd, "GP") == 0)
  {
    printHexPosition(activeStepper().currentPosition());
  }
  else if (strcasecmp(cmd, "GN") == 0)
  {
    printHexPosition(targetPosition);
  }
  else if (strcasecmp(cmd, "GI") == 0)
  {
    Serial.print(activeStepper().distanceToGo() == 0 ? "00#" : "01#");
  }
  else if (strcasecmp(cmd, "GD") == 0)
  {
    char reply[5];
    snprintf(reply, sizeof(reply), "%02X#", speedRaw);
    Serial.print(reply);
  }
  else if (strcasecmp(cmd, "SD") == 0)
  {
    speedRaw = constrain(parseHexPosition(param), 0x02, 0x20);
    updateSpeed();
  }
  else if (strcasecmp(cmd, "SP") == 0)
  {
    const long newPosition = max(0L, parseHexPosition(param));
    activeStepper().setCurrentPosition(newPosition);
    targetPosition = newPosition;
    savePositionIfNeeded();
  }
  else if (strcasecmp(cmd, "SN") == 0)
  {
    targetPosition = max(0L, parseHexPosition(param));
  }
  else if (strcasecmp(cmd, "FG") == 0)
  {
    enableMotor();
    activeStepper().moveTo(targetPosition);
  }
  else if (strcasecmp(cmd, "FQ") == 0)
  {
    if (manualSwitch.blocksRemoteMotion())
    {
      activeStepper().setCurrentPosition(activeStepper().currentPosition());
      updateSpeed();
    }
    else
    {
      activeStepper().stop();
    }
    manualSwitch.inhibit(millis());
    targetPosition = activeStepper().targetPosition();
  }
  else if (strcasecmp(cmd, "GH") == 0)
  {
    Serial.print(stepMode == STEP_HALF ? "FF#" : "00#");
  }
  else if (strcasecmp(cmd, "GX") == 0)
  {
    // Project-specific, read-only switch diagnostic, not a MoonLite command.
    const uint8_t input = (digitalRead(PIN_BTN_IN) == LOW ? 1 : 0) |
                          (digitalRead(PIN_BTN_OUT) == LOW ? 2 : 0);
    Serial.print('0');
    Serial.print(input);
    Serial.print('#');
  }
  else if (strcasecmp(cmd, "SH") == 0)
  {
    setStepMode(STEP_HALF);
  }
  else if (strcasecmp(cmd, "SF") == 0)
  {
    setStepMode(STEP_FULL);
  }
  else if (strcasecmp(cmd, "GT") == 0)
  {
    Serial.print("0000#"); // No temperature probe in the basic build.
  }
  else if (strcasecmp(cmd, "GC") == 0)
  {
    Serial.print("00#");
  }
  else if (strcasecmp(cmd, "PH") == 0)
  {
    enableMotor();
    targetPosition = 0;
    activeStepper().moveTo(0);
  }
}

void readSerialCommands()
{
  while (Serial.available() && !commandReady)
  {
    const char c = static_cast<char>(Serial.read());
    if (c == ':')
    {
      commandLength = 0;
    }
    else if (c == '#')
    {
      commandBuffer[commandLength] = '\0';
      commandReady = commandLength > 0;
    }
    else if (commandLength < COMMAND_MAX - 1)
    {
      commandBuffer[commandLength++] = c;
    }
  }
}

bool buttonEnabled(int pin)
{
  return pin >= 0;
}

void handleManualButtons()
{
  const uint8_t input = (digitalRead(PIN_BTN_IN) == LOW ? 1 : 0) |
                        (digitalRead(PIN_BTN_OUT) == LOW ? 2 : 0);
  const ManualSwitch::Action action = manualSwitch.update(input, millis());
  if (action == ManualSwitch::NONE) return;

  // Cancel any previous move and reset velocity without changing the counter.
  AccelStepper &stepper = activeStepper();
  stepper.setCurrentPosition(stepper.currentPosition());
  targetPosition = stepper.currentPosition();
  updateSpeed();
  if (action == ManualSwitch::STOP)
  {
    lastMoveMs = millis();
    return;
  }
  stepper.setMaxSpeed(MANUAL_SPEED_STEPS_PER_SEC);
  // IN decreases the logical position. Actual mechanical direction depends
  // on the coupling and must be checked before labelling the switch.
  targetPosition = action == ManualSwitch::IN ? 0 :
                   max(stepper.currentPosition(), MANUAL_MAX_POSITION);
  enableMotor();
  stepper.moveTo(targetPosition);
}

void setup()
{
  Serial.begin(SERIAL_BAUD);

  prefs.begin("focuser", false);
  stepMode = prefs.getBool("half", true) ? STEP_HALF : STEP_FULL;
  const long savedPosition = max(0L, prefs.getLong("pos", 0));

  activeStepper().setCurrentPosition(savedPosition);
  targetPosition = savedPosition;
  lastSavedPosition = savedPosition;
  lastSavedStepMode = stepMode;
  updateSpeed();
  disableMotor();

  if (buttonEnabled(PIN_BTN_IN))
  {
    pinMode(PIN_BTN_IN, INPUT_PULLUP);
  }
  if (buttonEnabled(PIN_BTN_OUT))
  {
    pinMode(PIN_BTN_OUT, INPUT_PULLUP);
  }

#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  lastMoveMs = millis();
}

void loop()
{
  handleManualButtons();
  readSerialCommands();
  if (commandReady)
  {
    processCommand(commandBuffer);
    commandLength = 0;
    commandReady = false;
  }

  if (activeStepper().distanceToGo() != 0)
  {
    enableMotor();
    activeStepper().run();
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, HIGH);
#endif
  }
  else
  {
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, LOW);
#endif
    if (millis() - lastMoveMs > POSITION_SAVE_IDLE_MS)
    {
      savePositionIfNeeded();
    }
    if (millis() - lastMoveMs > MOTOR_DISABLE_IDLE_MS)
    {
      disableMotor();
    }
  }
}
