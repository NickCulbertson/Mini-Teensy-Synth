#include "config.h"

#define NUM_PARAMETERS 31
#define NUM_PRESETS 20
#define VOICES 6

#include "MenuNavigation.h"
#include <Arduino.h>
#include <String.h>
#include <Encoder.h>

#ifdef USE_LCD_DISPLAY
  #include <LiquidCrystal_I2C.h>
#endif

#ifdef USE_OLED_DISPLAY
  #include <U8g2lib.h>
  #include <Wire.h>
#endif

#ifdef USE_LCD_DISPLAY
  extern LiquidCrystal_I2C lcd;
#endif

#ifdef USE_OLED_DISPLAY
  extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
#endif

extern bool inMenu;
extern bool inPresetBrowse;
extern MenuState currentMenuState;
extern int menuIndex;
extern int presetBrowseIndex;
extern float allParameterValues[NUM_PARAMETERS];
extern const char* controlNames[];
extern bool macroMode;
extern int midiChannel;
extern int playMode;
extern float glideTime;
extern int noiseType;
extern bool lfoEnabled;
extern int lfoTarget;
extern int currentPreset;
extern bool parameterChanged;
extern float osc1Range, osc2Range, osc3Range;
const char* parentMenuItems[] = {"Presets", "Oscillator 1", "Oscillator 2", "Oscillator 3", "Noise", "Envelopes", "Filter", "LFO", "Voice Mode", "Settings", "< Exit"};
const char* oscMenuItems[] = {"Range", "Waveform", "Volume", "Fine Tune", "< Back"};
const char* noiseMenuItems[] = {"Volume", "Type", "< Back"};
const char* envelopeMenuItems[] = {"Amp Attack", "Amp Sustain", "Amp Decay", "Filter Attack", "Filter Decay", "Filter Sustain", "< Back"};
const char* filterMenuItems[] = {"Cutoff", "Resonance", "Strength", "< Back"};
const char* lfoMenuItems[] = {"Rate", "Depth", "Toggle", "Target", "< Back"};
const char* voiceModeMenuItems[] = {"Play Mode", "Glide Time", "< Back"};
const char* settingsMenuItems[] = {"Macro Knobs", "MIDI Channel", "< Back"};
extern long encoderValues[];
extern long lastEncoderValues[];
extern Encoder menuEncoder;
extern Encoder enc1, enc2, enc3, enc4, enc5, enc6, enc7, enc8, enc9, enc10, enc11, enc13, enc14, enc15, enc16, enc17, enc18, enc19, enc20;
extern void updateSynthParameter(int paramIndex, float val);
extern int getWaveformIndex(float val, int osc);
extern int getRangeIndex(float val);
extern const char* waveformNames[];
extern const char* rangeNames[];
extern int currentPreset;

const MiniTeensyPreset presets[] = {
  {"80s Brass", {0.417, 0.417, 0.417, 0.539, 0.445, 0.417, 0.417, 0.417, 1.000, 0.789, 0.594, 0.562, 0.023, 0.039, 0.160, 0.000, 0.000, 0.000, 1.000, 0.026, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Saw Keys", {0.417, 0.417, 0.417, 0.453, 0.539, 0.417, 0.417, 0.250, 0.695, 0.789, 0.789, 0.633, 0.039, 0.000, 0.097, 0.469, 0.000, 0.000, 1.000, 0.039, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.580, 0.000, 0.000, 0.000, 0.000}},
  {"Square Keys", {0.417, 0.250, 0.417, 0.453, 0.547, 0.583, 0.917, 0.750, 0.789, 0.789, 0.789, 0.633, 0.008, 0.000, 0.113, 0.000, 0.000, 0.000, 0.448, 0.039, 0.500, 1.000, 0.008, 0.023, 1.000, 0.040, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"8-Bit Square", {0.417, 0.417, 0.417, 0.500, 0.500, 0.583, 0.417, 0.417, 0.789, 0.000, 0.000, 0.852, 0.000, 0.000, 0.160, 1.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 1.000, 0.000, 0.000, 0.000, 0.000}},
  {"Butter Supersaw", {0.417, 0.417, 0.417, 0.547, 0.453, 0.417, 0.417, 0.417, 0.594, 0.789, 0.789, 0.609, 0.016, 0.000, 0.238, 0.437, 0.020, 0.000, 1.000, 0.008, 0.500, 1.000, 0.008, 0.016, 1.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"West Coast Lead", {0.250, 0.417, 0.417, 0.477, 0.523, 0.417, 0.417, 0.250, 0.000, 1.000, 1.000, 0.711, 0.000, 0.000, 0.238, 1.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.930, 0.000, 0.000, 0.000, 0.000}},
  {"Teensy Lead", {0.250, 0.417, 0.417, 0.477, 0.523, 0.583, 0.583, 0.750, 0.000, 1.000, 1.000, 0.633, 0.000, 0.000, 0.238, 1.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 1.000, 0.150, 0.930, 0.711, 0.000, 0.000, 0.000}},
  {"Analog Bass", {0.417, 0.417, 0.250, 0.531, 0.484, 0.417, 0.417, 0.583, 1.000, 1.000, 1.000, 0.617, 0.047, 0.000, 0.019, 0.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.016, 0.055, 1.000, 0.330, 0.280, 0.812, 0.000, 0.000, 0.000}},
  {"Legato Bass", {0.250, 0.417, 0.417, 0.477, 0.523, 0.583, 0.417, 0.250, 0.953, 1.000, 1.000, 0.711, 0.023, 0.000, 0.160, 0.633, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 1.000, 1.000, 0.000, 0.000, 0.000}},
  {"Funk Bass", {0.250, 0.417, 0.417, 0.539, 0.461, 0.583, 0.417, 0.250, 1.000, 1.000, 1.000, 0.516, 0.172, 0.000, 0.066, 0.000, 0.000, 0.000, 0.828, 0.002, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.230, 0.000, 0.000, 0.000, 0.000}},
  {"8-Bit Harp", {0.750, 0.417, 0.750, 0.523, 0.445, 0.583, 0.750, 0.417, 1.000, 1.000, 0.508, 0.664, 0.000, 0.000, 0.051, 0.000, 0.050, 0.000, 0.000, 0.253, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Love Pad", {0.417, 0.417, 0.417, 0.414, 0.562, 0.417, 0.417, 0.417, 0.789, 0.789, 0.789, 0.609, 0.109, 0.148, 0.504, 0.000, 0.240, 0.070, 0.863, 0.206, 0.500, 0.250, 0.008, 0.023, 1.000, 0.040, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Thoughtful Pad", {0.750, 0.417, 0.250, 0.484, 0.539, 0.583, 0.417, 0.083, 0.227, 0.797, 0.750, 0.445, 0.195, 0.508, 1.000, 0.000, 0.290, 0.141, 1.000, 0.227, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.580, 0.000, 0.000, 0.000, 0.000}},
  {"Saw Pad", {0.417, 0.417, 0.417, 0.578, 0.453, 0.417, 0.417, 0.250, 0.789, 0.789, 0.789, 0.508, 0.023, 0.187, 0.371, 0.508, 0.100, 0.047, 0.820, 0.320, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"5th Pad", {0.500, 0.500, 0.500, 0.895, 0.536, 0.417, 0.417, 0.417, 0.750, 0.750, 0.780, 0.590, 0.060, 0.113, 0.270, 0.230, 0.000, 0.000, 0.800, 0.018, 0.500, 0.500, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Classic Sweep", {0.417, 0.417, 0.417, 0.578, 0.453, 0.417, 0.417, 0.250, 0.789, 0.789, 0.789, 0.453, 0.312, 0.031, 1.000, 0.000, 0.490, 0.000, 1.000, 0.031, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Haunted Organ", {0.417, 0.583, 0.750, 0.476, 0.523, 0.083, 0.083, 0.083, 0.766, 0.594, 0.578, 0.516, 0.000, 0.000, 0.019, 0.625, 0.110, 0.000, 0.687, 0.018, 0.500, 1.000, 0.016, 0.039, 1.000, 0.210, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Synth Drum", {0.417, 0.583, 0.250, 0.891, 0.445, 0.083, 0.083, 0.583, 0.117, 0.047, 0.016, 0.422, 0.093, 0.000, 0.016, 0.000, 1.000, 0.000, 0.000, 0.026, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Noise-scape", {0.417, 0.417, 0.417, 0.539, 0.445, 0.417, 0.417, 0.417, 0.000, 0.000, 0.000, 0.203, 0.273, 0.000, 0.555, 1.000, 1.000, 0.187, 1.000, 0.253, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}},
  {"Init", {0.417, 0.417, 0.417, 0.500, 0.500, 0.417, 0.417, 0.417, 0.789, 0.789, 0.789, 1.000, 0.000, 0.000, 0.160, 1.000, 0.000, 0.000, 1.000, 0.016, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000, 0.000}}
};

void displayText(String line1, String line2) {
#ifdef USE_LCD_DISPLAY
  lcd.clear();
  delayMicroseconds(500);
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);  
  lcd.print(line2);
#endif

#ifdef USE_OLED_DISPLAY
  display.clearBuffer();
  display.setFont(u8g2_font_8x13_tf);
  if (line1.length() > 0) {
    display.drawStr(3, 20, line1.c_str());
  }
  if (line2.length() > 0) {
    display.drawStr(3, 40, line2.c_str());
  }
  display.sendBuffer();
#endif
}

void updateDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  unsigned long now = millis();
  if (now - lastDisplayUpdate < 25) return; // Limit to 40Hz updates
  lastDisplayUpdate = now;
  String line1 = "";
  String line2 = "";
  if (inMenu) {
    if (inPresetBrowse) {
      line1 = "Presets";
      if (presetBrowseIndex == NUM_PRESETS) {
        line2 = "< Back";
      } else {
        line2 = String(presetBrowseIndex + 1) + ". " + String(presets[presetBrowseIndex].name);
      }
    } else {
      switch(currentMenuState) {
        case PARENT_MENU:
          line1 = "Menu";
          line2 = parentMenuItems[menuIndex];
          break;
        case OSC_1:
          line1 = "Oscillator 1";
          line2 = oscMenuItems[menuIndex];
          break;
        case OSC_2:
          line1 = "Oscillator 2";
          line2 = oscMenuItems[menuIndex];
          break;
        case OSC_3:
          line1 = "Oscillator 3";
          line2 = oscMenuItems[menuIndex];
          break;
        case NOISE:
          line1 = "Noise";
          line2 = noiseMenuItems[menuIndex];
          break;
        case ENVELOPES:
          line1 = "Envelopes";
          line2 = envelopeMenuItems[menuIndex];
          break;
        case FILTER:
          line1 = "Filter";
          line2 = filterMenuItems[menuIndex];
          break;
        case LFO:
          line1 = "LFO";
          line2 = lfoMenuItems[menuIndex];
          break;
        case VOICE_MODE:
          line1 = "Voice Mode";
          line2 = voiceModeMenuItems[menuIndex];
          break;
        case SETTINGS:
          line1 = "Settings";
          line2 = settingsMenuItems[menuIndex];
          break;
        case MACRO_KNOBS:
          line1 = "Filter Knobs:";
          line2 = macroMode ? "LFO Controls" : "Filter Env";
          break;
        case MIDI_CHANNEL:
          line1 = "MIDI Channel:";
          line2 = (midiChannel == 0) ? "Omni" : String(midiChannel);
          break;
        default:
          {
            int paramIndex = getParameterIndex(currentMenuState);
            if (paramIndex >= 0) {
              line1 = controlNames[paramIndex];
              if (paramIndex == 3 || paramIndex == 4 || paramIndex == 20) { // Extended fine tuning
                float val = allParameterValues[paramIndex];
                if (val <= 0.25) {
                  float semiRange = val / 0.25;
                  int semitones = (int)(-12 + (semiRange * 11)); // -12 to -1
                  line2 = String(semitones) + "st";
                } else if (val >= 0.75) {
                  float semiRange = (val - 0.75) / 0.25;
                  int semitones = (int)(1 + (semiRange * 11)); // +1 to +12
                  line2 = "+" + String(semitones) + "st";
                } else {
                  int cents = (int)((val - 0.5) * 100); // -25 to +25 cents
                  line2 = (cents >= 0 ? "+" : "") + String(cents) + "c";
                }
              } else if (paramIndex == 22) { // LFO Rate
                float rate = 0.1 + allParameterValues[paramIndex] * 19.9;
                line2 = String(rate, 1) + " Hz";
              } else if (paramIndex == 23) { // LFO Depth
                int depth = (int)(allParameterValues[paramIndex] * 100);
                line2 = String(depth) + "%";
              } else if (paramIndex == 24) { // LFO Toggle
                line2 = lfoEnabled ? "ON" : "OFF"; // Use actual variable instead of parameter
              } else if (paramIndex == 25) { // LFO Target
                if (lfoTarget == 0) line2 = "Pitch";
                else if (lfoTarget == 1) line2 = "Filter";
                else line2 = "Amp";
              } else if (paramIndex == 26) { // Play Mode
                if (playMode == 0) line2 = "Mono";
                else if (playMode == 1) line2 = "Poly";
                else line2 = "Legato";
              } else if (paramIndex == 27) { // Glide Time
                if (glideTime == 0.0) {
                  line2 = "OFF";
                } else {
                  float timeMs = 50 + (glideTime * 950); // 50ms to 1000ms
                  line2 = String((int)timeMs) + "ms";
                }
              } else if (paramIndex == 28) { // Noise Type
                line2 = (noiseType == 0) ? "White" : "Pink";
              } else if (paramIndex == 29) { // Macro Mode
                line2 = macroMode ? "LFO Controls" : "Filter Env";
              } else if (paramIndex == 30) { // MIDI Channel
                line2 = (midiChannel == 0) ? "Omni" : String(midiChannel);
              } else {
                int displayValue = (int)(allParameterValues[paramIndex] * 127);
                line2 = String(displayValue);
              }
            }
          }
          break;
      }
    }
    displayText(line1, line2);
  } else {
    // Only show default message if no parameter was recently changed via MIDI
    if (!parameterChanged) {
      line1 = "MiniTeensy";
      line2 = "Press for menu";
      displayText(line1, line2);
    }
  }
}

int getParameterIndex(MenuState state) {
  switch(state) {
    case OSC1_RANGE: return 0;
    case OSC2_RANGE: return 1;
    case OSC3_RANGE: return 2;
    case OSC2_FINE: return 3;
    case OSC3_FINE: return 4;
    case OSC1_WAVE: return 5;
    case OSC2_WAVE: return 6;
    case OSC3_WAVE: return 7;
    case OSC1_VOLUME: return 8;
    case OSC2_VOLUME: return 9;
    case OSC3_VOLUME: return 10;
    case CUTOFF: return 11;
    case RESONANCE: return 12;
    case FILTER_ATTACK: return 13;
    case FILTER_DECAY: return 14;
    case FILTER_SUSTAIN: return 15;
    case NOISE_VOLUME: return 16;
    case AMP_ATTACK: return 17;
    case AMP_SUSTAIN: return 18;
    case AMP_DECAY: return 19;
    case OSC1_FINE: return 20;  // menu-only parameter
    case FILTER_STRENGTH: return 21;  // menu-only parameter
    case LFO_RATE: return 22;
    case LFO_DEPTH: return 23;
    case LFO_TOGGLE: return 24;
    case LFO_TARGET: return 25;
    case PLAY_MODE: return 26;
    case GLIDE_TIME: return 27;
    case NOISE_TYPE: return 28;
    case MACRO_KNOBS: return 29;
    case MIDI_CHANNEL: return 30;
    default: return -1;
  }
}

void handleEncoder() {
#ifdef USE_OLED_DISPLAY
  long newMenuValue = menuEncoder.read() / 4; // Less sensitive for OLED encoder
#else
  long newMenuValue = menuEncoder.read() / 2; // Standard sensitivity for separate encoder
#endif
  static long oldMenuValue = 0;
  if (newMenuValue != oldMenuValue) {
    // If menu encoder is set to -1 (menu-only mode), auto-enter menu on rotation
    if (!inMenu && MENU_ENCODER_PARAM == -1) {
      inMenu = true;
      currentMenuState = PARENT_MENU;
      menuIndex = 0;
      inPresetBrowse = false;
      printCurrentPresetValues();
      updateDisplay();
    }
    
    if (inMenu) {
      if (inPresetBrowse) {
        if (newMenuValue > oldMenuValue) {
          presetBrowseIndex++;
          if (presetBrowseIndex > NUM_PRESETS) { // NUM_PRESETS = "Back" option
            presetBrowseIndex = 0;
          }
        } else {
          presetBrowseIndex--;
          if (presetBrowseIndex < 0) {
            presetBrowseIndex = NUM_PRESETS; // Wrap to "Back"
          }
        }
        updateDisplay();
      } else if (getParameterIndex(currentMenuState) >= 0) {
        int paramIndex = getParameterIndex(currentMenuState);
        if (newMenuValue > oldMenuValue) {
          if (paramIndex == 24) { // LFO Toggle - instant toggle with single turn
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + 0.5, 0.0, 1.0);
          } else if (paramIndex == 25) { // LFO Target - medium increment for 3 positions 
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + 0.03, 0.0, 1.0);
          } else if (paramIndex == 26) { // Play Mode - large increment for 1-turn switching
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + 0.05, 0.0, 1.0);
          } else if (paramIndex == 28) { // Noise Type - instant toggle with single turn
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + 0.5, 0.0, 1.0);
          } else if (paramIndex == 29) { // Macro Mode - instant toggle with single turn
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + 0.5, 0.0, 1.0);
          } else if (paramIndex == 30) { // MIDI Channel - step through channels
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + (1.0/16.0), 0.0, 1.0);
          } else {
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + 1.0/128.0, 0.0, 1.0);
          }
          updateParameterFromMenu(paramIndex, allParameterValues[paramIndex]);
        } else {
          if (paramIndex == 24) { // LFO Toggle - instant toggle with single turn
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - 0.5, 0.0, 1.0);
          } else if (paramIndex == 25) { // LFO Target - medium increment for 3 positions
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - 0.03, 0.0, 1.0);
          } else if (paramIndex == 26) { // Play Mode - large increment for 1-turn switching
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - 0.05, 0.0, 1.0);
          } else if (paramIndex == 28) { // Noise Type - instant toggle with single turn
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - 0.5, 0.0, 1.0);
          } else if (paramIndex == 29) { // Macro Mode - instant toggle with single turn
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - 0.5, 0.0, 1.0);
          } else if (paramIndex == 30) { // MIDI Channel - step through channels
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - (1.0/16.0), 0.0, 1.0);
          } else {
            allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] - 1.0/128.0, 0.0, 1.0);
          }
          updateParameterFromMenu(paramIndex, allParameterValues[paramIndex]);
        }
        updateDisplay();
      } else {
        // In navigation mode: move through menu options
        if (newMenuValue > oldMenuValue) {
          incrementMenuIndex();
        } else {
          decrementMenuIndex();
        }
        updateDisplay();
      }
    } else {
#ifdef USE_OLED_DISPLAY
      encoderValues[11] = menuEncoder.read() / 4; // Adjusted sensitivity for OLED encoder
#else
      encoderValues[11] = menuEncoder.read() / 2; // Standard sensitivity for separate encoder
#endif
    }
    oldMenuValue = newMenuValue;
  }
  
  static bool lastButtonState = HIGH;
  static unsigned long lastPressTime = 0;
  
  bool currentButtonState = digitalRead(MENU_ENCODER_SW);
  unsigned long now = millis();
  
  // Simple debouncing: only register press if enough time has passed since last press
  if (currentButtonState == LOW && lastButtonState == HIGH && (now - lastPressTime) > 50) {
    lastPressTime = now;
    
    if (!inMenu) {
      inMenu = true;
      currentMenuState = PARENT_MENU;
      menuIndex = 0;
      inPresetBrowse = false;
      printCurrentPresetValues();
      updateDisplay();
    } else if (inPresetBrowse) {
      if (presetBrowseIndex == NUM_PRESETS) {
        inPresetBrowse = false;
      } else {
        loadPreset(presetBrowseIndex);
      }
      updateDisplay();
    } else {
      if (getParameterIndex(currentMenuState) >= 0) {
        backMenuAction();
      } else if (currentMenuState == MACRO_KNOBS) {
        macroMode = !macroMode;
      } else {
        navigateMenuForward();
      }
      updateDisplay();
    }
  }
  
  lastButtonState = currentButtonState;
}

// Fast button-only check (can be called more frequently)
void checkEncoderButton() {
  static bool lastButtonState = HIGH;
  static unsigned long lastPressTime = 0;
  
  bool currentButtonState = digitalRead(MENU_ENCODER_SW);
  unsigned long now = millis();
  
  // Simple debouncing: only register press if enough time has passed since last press
  if (currentButtonState == LOW && lastButtonState == HIGH && (now - lastPressTime) > 50) {
    lastPressTime = now;
    
    if (!inMenu) {
      inMenu = true;
      currentMenuState = PARENT_MENU;
      menuIndex = 0;
      inPresetBrowse = false;
      printCurrentPresetValues();
      updateDisplay();
    } else if (inPresetBrowse) {
      if (presetBrowseIndex == NUM_PRESETS) {
        inPresetBrowse = false;
      } else {
        loadPreset(presetBrowseIndex);
      }
      updateDisplay();
    } else {
      if (getParameterIndex(currentMenuState) >= 0) {
        backMenuAction();
      } else if (currentMenuState == MACRO_KNOBS) {
        macroMode = !macroMode;
      } else {
        navigateMenuForward();
      }
      updateDisplay();
    }
  }
  lastButtonState = currentButtonState;
}

void navigateMenuForward() {
  switch(currentMenuState) {
    case PARENT_MENU:
      if (menuIndex == 0) {
        inPresetBrowse = true;
        presetBrowseIndex = 0;
        return; 
      }
      else if (menuIndex == 1) currentMenuState = OSC_1;
      else if (menuIndex == 2) currentMenuState = OSC_2;
      else if (menuIndex == 3) currentMenuState = OSC_3;
      else if (menuIndex == 4) currentMenuState = NOISE;
      else if (menuIndex == 5) currentMenuState = ENVELOPES;
      else if (menuIndex == 6) currentMenuState = FILTER;
      else if (menuIndex == 7) currentMenuState = LFO;
      else if (menuIndex == 8) currentMenuState = VOICE_MODE;
      else if (menuIndex == 9) currentMenuState = SETTINGS;
      else if (menuIndex == 10) {
        inMenu = false;
        inPresetBrowse = false;
        return;
      }
      menuIndex = 0;
      break;
    case OSC_1:
      if (menuIndex == 0) currentMenuState = OSC1_RANGE;
      else if (menuIndex == 1) currentMenuState = OSC1_WAVE;
      else if (menuIndex == 2) currentMenuState = OSC1_VOLUME;
      else if (menuIndex == 3) currentMenuState = OSC1_FINE;
      else if (menuIndex == 4) {
        currentMenuState = PARENT_MENU;
        menuIndex = 1; 
        return;
      }
      break;
    case OSC_2:
      if (menuIndex == 0) currentMenuState = OSC2_RANGE;
      else if (menuIndex == 1) currentMenuState = OSC2_WAVE;
      else if (menuIndex == 2) currentMenuState = OSC2_VOLUME;
      else if (menuIndex == 3) currentMenuState = OSC2_FINE;
      else if (menuIndex == 4) {
        currentMenuState = PARENT_MENU;
        menuIndex = 2;
        return;
      }
      break;
    case OSC_3:
      if (menuIndex == 0) currentMenuState = OSC3_RANGE;
      else if (menuIndex == 1) currentMenuState = OSC3_WAVE;
      else if (menuIndex == 2) currentMenuState = OSC3_VOLUME;
      else if (menuIndex == 3) currentMenuState = OSC3_FINE;
      else if (menuIndex == 4) {
        currentMenuState = PARENT_MENU;
        menuIndex = 3;
        return;
      }
      break;
    case NOISE:
      if (menuIndex == 0) currentMenuState = NOISE_VOLUME;
      else if (menuIndex == 1) currentMenuState = NOISE_TYPE;
      else if (menuIndex == 2) {
        currentMenuState = PARENT_MENU;
        menuIndex = 4;
        return;
      }
      break;
    case ENVELOPES:
      if (menuIndex == 0) currentMenuState = AMP_ATTACK;
      else if (menuIndex == 1) currentMenuState = AMP_SUSTAIN;
      else if (menuIndex == 2) currentMenuState = AMP_DECAY;
      else if (menuIndex == 3) currentMenuState = FILTER_ATTACK;
      else if (menuIndex == 4) currentMenuState = FILTER_DECAY;
      else if (menuIndex == 5) currentMenuState = FILTER_SUSTAIN;
      else if (menuIndex == 6) {
        currentMenuState = PARENT_MENU;
        menuIndex = 5;
        return;
      }
      break;
    case FILTER:
      if (menuIndex == 0) currentMenuState = CUTOFF;
      else if (menuIndex == 1) currentMenuState = RESONANCE;
      else if (menuIndex == 2) currentMenuState = FILTER_STRENGTH;
      else if (menuIndex == 3) {
        currentMenuState = PARENT_MENU;
        menuIndex = 6;
        return;
      }
      break;
    case LFO:
      if (menuIndex == 0) currentMenuState = LFO_RATE;
      else if (menuIndex == 1) currentMenuState = LFO_DEPTH;
      else if (menuIndex == 2) currentMenuState = LFO_TOGGLE;
      else if (menuIndex == 3) currentMenuState = LFO_TARGET;
      else if (menuIndex == 4) {
        currentMenuState = PARENT_MENU;
        menuIndex = 7;
        return;
      }
      break;
    case VOICE_MODE:
      if (menuIndex == 0) currentMenuState = PLAY_MODE;
      else if (menuIndex == 1) currentMenuState = GLIDE_TIME;
      else if (menuIndex == 2) {
        currentMenuState = PARENT_MENU;
        menuIndex = 8;
        return;
      }
      break;
    case SETTINGS:
      if (menuIndex == 0) currentMenuState = MACRO_KNOBS;
      else if (menuIndex == 1) currentMenuState = MIDI_CHANNEL;
      else if (menuIndex == 2) {
        currentMenuState = PARENT_MENU;
        menuIndex = 9;
        return;
      }
      break;
    default:
      break;
  }
}

void incrementMenuIndex() {
  switch(currentMenuState) {
    case PARENT_MENU:
      menuIndex++;
      if (menuIndex > 10) menuIndex = 0; // Wrap to first item (now 10 items: 0-9)
      break;
    case OSC_1:
      menuIndex++;
      if (menuIndex > 4) menuIndex = 0; // OSC1 has 5 items (0-4) including Back
      break;
    case OSC_2:
      menuIndex++;
      if (menuIndex > 4) menuIndex = 0; // OSC2 has 5 items (0-4) including Back
      break;
    case OSC_3:
      menuIndex++;
      if (menuIndex > 4) menuIndex = 0; // OSC3 has 5 items (0-4) including Back
      break;
    case NOISE:
      menuIndex++;
      if (menuIndex > 2) menuIndex = 0; // Noise has 3 items (0-2) including Back
      break;
    case ENVELOPES:
      menuIndex++;
      if (menuIndex > 6) menuIndex = 0; // Envelopes has 7 items (0-6) including Back
      break;
    case FILTER:
      menuIndex++;
      if (menuIndex > 3) menuIndex = 0; // Filter has 4 items (0-3) including Back
      break;
    case LFO:
      menuIndex++;
      if (menuIndex > 4) menuIndex = 0; // LFO has 5 items (0-4) including Back
      break;
    case VOICE_MODE:
      menuIndex++;
      if (menuIndex > 2) menuIndex = 0; // Voice Mode has 3 items (0-2) including Back
      break;
    case SETTINGS:
      menuIndex++;
      if (menuIndex > 2) menuIndex = 0; // Settings has 3 items (0-2) including Back
      break;
    default:
      break;
  }
}

void decrementMenuIndex() {
  switch(currentMenuState) {
    case PARENT_MENU:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 10; // Wrap to last item (now 10 items: 0-9)
      break;
    case OSC_1:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // OSC1 has 5 items (0-4) including Back
      break;
    case OSC_2:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // OSC2 has 5 items (0-4) including Back
      break;
    case OSC_3:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // OSC3 has 5 items (0-4) including Back
      break;
    case NOISE:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2; // Noise has 3 items (0-2) including Back
      break;
    case ENVELOPES:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 6; // Envelopes has 7 items (0-6) including Back
      break;
    case FILTER:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 3; // Filter has 4 items (0-3) including Back
      break;
    case LFO:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // LFO has 5 items (0-4) including Back
      break;
    case VOICE_MODE:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2; // Voice Mode has 3 items (0-2) including Back
      break;
    case SETTINGS:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2; // Settings has 3 items (0-2) including Back
      break;
    default:
      break;
  }
}

void navigateMenuBackward() {
  switch(currentMenuState) {
    case OSC_1:
      currentMenuState = PARENT_MENU;
      menuIndex = 1; // OSC_1 position
      break;
    case OSC_2:
      currentMenuState = PARENT_MENU;
      menuIndex = 2; // OSC_2 position
      break;
    case OSC_3:
      currentMenuState = PARENT_MENU;
      menuIndex = 3; // OSC_3 position
      break;
    case NOISE:
      currentMenuState = PARENT_MENU;
      menuIndex = 4; // NOISE position
      break;
    case ENVELOPES:
      currentMenuState = PARENT_MENU;
      menuIndex = 5; // ENVELOPES position
      break;
    case FILTER:
      currentMenuState = PARENT_MENU;
      menuIndex = 6; // FILTER position
      break;
    case LFO:
      currentMenuState = PARENT_MENU;
      menuIndex = 7; // LFO position
      break;
    case VOICE_MODE:
      currentMenuState = PARENT_MENU;
      menuIndex = 8; // VOICE_MODE position
      break;
    case SETTINGS:
      currentMenuState = PARENT_MENU;
      menuIndex = 9; // SETTINGS position
      break;
    default:
      break;
  }
}

void backMenuAction() {
  switch(currentMenuState) {
    case OSC1_RANGE:
    case OSC1_WAVE:
    case OSC1_VOLUME:
    case OSC1_FINE:
      currentMenuState = OSC_1;
      if (currentMenuState == OSC1_RANGE) menuIndex = 0;
      else if (currentMenuState == OSC1_WAVE) menuIndex = 1;
      else if (currentMenuState == OSC1_VOLUME) menuIndex = 2;
      else if (currentMenuState == OSC1_FINE) menuIndex = 3;
      break;
    case OSC2_RANGE:
    case OSC2_WAVE:
    case OSC2_VOLUME:
    case OSC2_FINE:
      currentMenuState = OSC_2;
      if (currentMenuState == OSC2_RANGE) menuIndex = 0;
      else if (currentMenuState == OSC2_WAVE) menuIndex = 1;
      else if (currentMenuState == OSC2_VOLUME) menuIndex = 2;
      else if (currentMenuState == OSC2_FINE) menuIndex = 3;
      break;
    case OSC3_RANGE:
    case OSC3_WAVE:
    case OSC3_VOLUME:
    case OSC3_FINE:
      currentMenuState = OSC_3;
      if (currentMenuState == OSC3_RANGE) menuIndex = 0;
      else if (currentMenuState == OSC3_WAVE) menuIndex = 1;
      else if (currentMenuState == OSC3_VOLUME) menuIndex = 2;
      else if (currentMenuState == OSC3_FINE) menuIndex = 3;
      break;
    case NOISE_VOLUME:
    case NOISE_TYPE:
      currentMenuState = NOISE;
      if (currentMenuState == NOISE_VOLUME) menuIndex = 0;
      else if (currentMenuState == NOISE_TYPE) menuIndex = 1;
      break;
    case CUTOFF:
    case RESONANCE:
    case FILTER_STRENGTH:
      currentMenuState = FILTER;
      if (currentMenuState == CUTOFF) menuIndex = 0;
      else if (currentMenuState == RESONANCE) menuIndex = 1;
      else if (currentMenuState == FILTER_STRENGTH) menuIndex = 2;
      break;
    case LFO_RATE:
    case LFO_DEPTH:
    case LFO_TOGGLE:
    case LFO_TARGET:
      currentMenuState = LFO;
      if (currentMenuState == LFO_RATE) menuIndex = 0;
      else if (currentMenuState == LFO_DEPTH) menuIndex = 1;
      else if (currentMenuState == LFO_TOGGLE) menuIndex = 2;
      else if (currentMenuState == LFO_TARGET) menuIndex = 3;
      break;
    case AMP_ATTACK:
    case AMP_SUSTAIN:
    case AMP_DECAY:
    case FILTER_ATTACK:
    case FILTER_DECAY:
    case FILTER_SUSTAIN:
      currentMenuState = ENVELOPES;
      if (currentMenuState == AMP_ATTACK) menuIndex = 0;
      else if (currentMenuState == AMP_SUSTAIN) menuIndex = 1;
      else if (currentMenuState == AMP_DECAY) menuIndex = 2;
      else if (currentMenuState == FILTER_ATTACK) menuIndex = 3;
      else if (currentMenuState == FILTER_DECAY) menuIndex = 4;
      else if (currentMenuState == FILTER_SUSTAIN) menuIndex = 5;
      break;
    case PLAY_MODE:
    case GLIDE_TIME:
      currentMenuState = VOICE_MODE;
      if (currentMenuState == PLAY_MODE) menuIndex = 0;
      else if (currentMenuState == GLIDE_TIME) menuIndex = 1;
      break;
    case MACRO_KNOBS:
    case MIDI_CHANNEL:
      currentMenuState = SETTINGS;
      if (currentMenuState == MACRO_KNOBS) menuIndex = 0;
      else if (currentMenuState == MIDI_CHANNEL) menuIndex = 1;
      break;
    default:
      navigateMenuBackward();
      break;
  }
}

void loadPreset(int presetIndex) {
  if (presetIndex >= 0 && presetIndex < NUM_PRESETS) {
    currentPreset = presetIndex;
    for (int i = 0; i < NUM_PARAMETERS; i++) {
      allParameterValues[i] = presets[presetIndex].parameters[i];
      updateSynthParameter(i, allParameterValues[i]);
    }
    updateDisplay();
  }
}

void updateParameterFromMenu(int paramIndex, float val) {
  updateSynthParameter(paramIndex, val);
  // Display update removed - now handled once per loop in main()
}

void updateEncoderParameter(int paramIndex, int change) {
  float increment = 0.01; // Base increment
  switch (paramIndex) {
    case 0: case 1: case 2: // Range controls - discrete steps
      increment = 0.16; // 6 ranges
      break;
    case 3: case 4: // Fine tuning controls - 128 steps (enc4, enc5 /4)
      increment = 1.0/128.0; // = 0.0078125 - exact 128-step resolution
      break;
    case 5: case 6: case 7: // Waveform controls - discrete steps
      increment = 0.16; // 6 waveforms
      break;
    case 8: case 9: case 10: // Volume controls - 128 steps (enc9, enc10, enc11 /4)
      increment = 1.0/128.0; // = 0.0078125 - exact 128-step resolution
      break;
    case 11: // Filter cutoff - consistent 128-step feel (menuEncoder /2)
      increment = 1.0/128.0; // = 0.0078125 - consistent with all other controls
      break;
    case 12: // Filter resonance - optimized for 128 steps (enc13 /4) 
      increment = 1.0/128.0; // = 0.0078125 - exact 128-step resolution
      break;
    case 13: case 14: case 15: case 17: case 18: case 19: // Envelope controls - 128 steps (enc14-enc20 /4)
      increment = 1.0/128.0; // = 0.0078125 - exact 128-step resolution
      break;
    case 24: case 25: case 26: case 28: case 29: case 30: // Toggle/discrete controls (LFO Toggle, LFO Target, Play Mode, Noise Type, Macro Mode, MIDI Channel)
      increment = 0.5; // Large steps for immediate toggle response
      break;
    default:
      increment = 0.01; // Standard increment
      break;
  }
  
  allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + (change * increment), 0.0, 1.0);
  float val = allParameterValues[paramIndex];
  if (paramIndex >= 0 && paramIndex <= 2) { // Range controls
    if (val < 0.167) val = 0.083;        // 32' center
    else if (val < 0.333) val = 0.25;    // 16' center  
    else if (val < 0.5) val = 0.417;     // 8' center
    else if (val < 0.667) val = 0.583;   // 4' center
    else if (val < 0.833) val = 0.75;    // 2' center
    else val = 0.917;                    // LO center
    allParameterValues[paramIndex] = val; // Store snapped value
  } 
  else if (paramIndex >= 5 && paramIndex <= 7) { // Waveform controls
    if (val < 0.167) val = 0.083;        // Triangle center
    else if (val < 0.333) val = 0.25;    // Reverse Saw center
    else if (val < 0.5) val = 0.417;     // Sawtooth center  
    else if (val < 0.667) val = 0.583;   // Square center
    else if (val < 0.833) val = 0.75;    // Pulse center
    else val = 0.917;                    // Pulse center
    allParameterValues[paramIndex] = val; // Store snapped value
  }
  
  updateSynthParameter(paramIndex, val);
  if (!inMenu) {
    String line1 = controlNames[paramIndex];
    String line2 = "";
    if (paramIndex >= 5 && paramIndex <= 7) { // Waveform controls
      int waveIndex = getWaveformIndex(val, (paramIndex == 5) ? 1 : ((paramIndex == 6) ? 2 : 3));
      line2 = waveformNames[waveIndex];
    }
    else if (paramIndex >= 0 && paramIndex <= 2) { // Range controls
      int rangeIndex = getRangeIndex(val);
      line2 = rangeNames[rangeIndex];
    }
    else if (paramIndex == 3 || paramIndex == 4) { // Extended fine tuning controls
      if (val <= 0.25) {
        float semiRange = val / 0.25;
        int semitones = (int)(-12 + (semiRange * 11)); // -12 to -1
        line2 = String(semitones) + "st";
      } else if (val >= 0.75) {
        float semiRange = (val - 0.75) / 0.25;
        int semitones = (int)(1 + (semiRange * 11)); // +1 to +12
        line2 = "+" + String(semitones) + "st";
      } else {
        int cents = (int)((val - 0.5) * 100); // -25 to +25 cents
        if (cents >= 0) {
          line2 = "+" + String(cents) + "c";
        } else {
          line2 = String(cents) + "c";
        }
      }
    }
    else {
      int displayValue = (int)(val * 127); // 0-127 MIDI scale
      line2 = String(displayValue);
    }
    
    displayText(line1, line2);
  }
}

void resetEncoderBaselines() {
  for (int i = 0; i < 20; i++) {
    int paramIndex = encoderMapping[i]; // Use configurable mapping
    
    // Only reset encoders that are mapped to valid parameters
    if (paramIndex != -1 && paramIndex >= 0 && paramIndex < NUM_PARAMETERS) {
      long targetEncoderValue = (long)(allParameterValues[paramIndex] * 100);
      switch(i) {
        case 0: enc1.write(targetEncoderValue * 4); break;
        case 1: enc2.write(targetEncoderValue * 4); break;
        case 2: enc3.write(targetEncoderValue * 4); break;
        case 3: enc4.write(targetEncoderValue * 4); break;
        case 4: enc5.write(targetEncoderValue * 4); break;
        case 5: enc6.write(targetEncoderValue * 4); break;
        case 6: enc7.write(targetEncoderValue * 4); break;
        case 7: enc8.write(targetEncoderValue * 4); break;
        case 8: enc9.write(targetEncoderValue * 4); break;
        case 9: enc10.write(targetEncoderValue * 4); break;
        case 10: enc11.write(targetEncoderValue * 4); break;
        case 11: /* menuEncoder handled by MenuNavigation.cpp */ break;
        case 12: enc13.write(targetEncoderValue * 4); break;
        case 13: enc14.write(targetEncoderValue * 4); break;
        case 14: enc15.write(targetEncoderValue * 4); break;
        case 15: enc16.write(targetEncoderValue * 4); break;
        case 16: enc17.write(targetEncoderValue * 4); break;
        case 17: enc18.write(targetEncoderValue * 4); break;
        case 18: enc19.write(targetEncoderValue * 4); break;
        case 19: enc20.write(targetEncoderValue * 4); break;
      }
      encoderValues[i] = targetEncoderValue;
      lastEncoderValues[i] = targetEncoderValue;
    }
  }
}

void printCurrentPresetValues() {
  Serial.println("\n=== CURRENT PRESET DEBUG ===");
  Serial.print("Active Preset: ");
  Serial.print(currentPreset + 1);
  Serial.print(" (");
  Serial.print(presets[currentPreset].name);
  Serial.println(")");
  
  Serial.println("\nCurrent Parameter Values:");
  Serial.print("{");
  for (int i = 0; i < NUM_PARAMETERS; i++) {
    Serial.print(allParameterValues[i], 3); // 3 decimal places
    if (i < NUM_PARAMETERS - 1) Serial.print(", ");
  }
  Serial.println("}");
  Serial.println("Copy this line into your preset array!");
  
  Serial.println("\nKey Parameters:");
  Serial.print("Osc1 Range: "); Serial.print(allParameterValues[0], 3);
  Serial.print(" | Osc2 Range: "); Serial.print(allParameterValues[1], 3);
  Serial.print(" | Osc3 Range: "); Serial.println(allParameterValues[2], 3);
  Serial.print("Osc1 Wave: "); Serial.print(allParameterValues[5], 3);
  Serial.print(" | Osc2 Wave: "); Serial.print(allParameterValues[6], 3);
  Serial.print(" | Filter: "); Serial.println(allParameterValues[11], 3);
  
  Serial.println("\nEncoder Raw Values:");
  for (int i = 0; i < 5; i++) {
    Serial.print("Enc");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(encoderValues[i]);
    Serial.print(" -> ");
    int paramIndex = encoderMapping[i];
    if (paramIndex != -1 && paramIndex >= 0 && paramIndex < NUM_PARAMETERS) {
      Serial.println(allParameterValues[paramIndex], 3);
    } else {
      Serial.println("[DISABLED]");
    }
  }
  Serial.println("=============================\n");
  
  Serial.println("Type 'r' in Serial Monitor to reset encoder baselines to current values");
}

const char* getPresetName(int presetIndex) {
  if (presetIndex >= 0 && presetIndex < NUM_PRESETS) {
    return presets[presetIndex].name;
  }
  return "Unknown";
}