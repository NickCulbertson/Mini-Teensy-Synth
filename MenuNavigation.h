#ifndef MENU_NAVIGATION_H
#define MENU_NAVIGATION_H

#include "config.h"
#include <Arduino.h>

// ============================================================================
// Menu State Enums
// ============================================================================

enum MenuState {
  PARENT_MENU,
  OSC_1, 
  OSC_2,
  OSC_3,
  NOISE,
  ENVELOPES,
  FILTER,
  LFO,
  VOICE_MODE,
  SETTINGS,
  
  // Oscillator 1 sub-menus
  OSC1_RANGE,
  OSC1_WAVE,
  OSC1_VOLUME,
  OSC1_FINE,
  
  // Oscillator 2 sub-menus  
  OSC2_RANGE,
  OSC2_WAVE,
  OSC2_VOLUME,
  OSC2_FINE,
  
  // Oscillator 3 sub-menus
  OSC3_RANGE,
  OSC3_WAVE,
  OSC3_VOLUME,
  OSC3_FINE,
  
  // Noise sub-menus
  NOISE_VOLUME,
  NOISE_TYPE,
  
  // Envelope sub-menus
  AMP_ATTACK,
  AMP_SUSTAIN,
  AMP_DECAY,
  FILTER_ATTACK,
  FILTER_DECAY,
  FILTER_SUSTAIN,
  
  // Filter sub-menus
  CUTOFF,
  RESONANCE,
  FILTER_STRENGTH,
  
  // LFO sub-menus
  LFO_RATE,
  LFO_DEPTH,
  LFO_TOGGLE,
  LFO_TARGET,
  
  // Voice Mode sub-menus
  PLAY_MODE,
  GLIDE_TIME,
  
  // Settings sub-menus
  MACRO_KNOBS,
  MIDI_CHANNEL
};

// ============================================================================
// Data Structures
// ============================================================================

struct MiniTeensyPreset {
  const char* name;
  float parameters[31]; // Fixed size to match presets
};

// ============================================================================
// External Variables
// ============================================================================
extern const int encoderMapping[20];

// ============================================================================
// Menu Navigation Function Declarations
// ============================================================================

// Parameter mapping
int getParameterIndex(MenuState state);

// Display functions
void displayText(String line1, String line2);
void updateDisplay();

// Menu navigation functions
void handleEncoder();
void navigateMenuForward();
void navigateMenuBackward();
void incrementMenuIndex();
void decrementMenuIndex();
void backMenuAction();

// Parameter handling functions
void updateParameterFromMenu(int paramIndex, float val);
void updateEncoderParameter(int paramIndex, int change);

// Utility functions
void resetEncoderBaselines();

// Preset functions
void loadPreset(int presetIndex);
void printCurrentPresetValues();
const char* getPresetName(int presetIndex);

#endif // MENU_NAVIGATION_H