#include <USBHost_t36.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

const int MENU_ENCODER_CLK = 14, MENU_ENCODER_DT = 13, MENU_ENCODER_SW = 15;

// Encoder definitions
Encoder enc1(4, 5);
Encoder enc2(2, 3);
Encoder enc3(0, 1);
Encoder enc4(8, 9);
Encoder enc5(6, 7);
Encoder enc6(25, 27);
Encoder enc7(12, 24);
Encoder enc8(10, 11);
Encoder enc9(29, 30);
Encoder enc10(28, 26);
Encoder enc11(21, 20);
Encoder enc13(34, 33);
Encoder enc14(50, 41);
Encoder enc15(23, 22);
Encoder enc16(36, 35);
Encoder enc17(31, 32);
Encoder enc18(17, 16);
Encoder enc19(38, 37);
Encoder enc20(40, 39);
Encoder menuEncoder(MENU_ENCODER_CLK, MENU_ENCODER_DT);

long encoderValues[20] = {0};
long lastEncoderValues[20] = {0};
// Default parameter values - matches "Init" preset
float allParameterValues[29] = {
  0.417, 0.417, 0.417, 0.500, 0.500, 0.417, 0.417, 0.417, 0.789, 0.789,
  0.789, 1.000, 0.000, 0.000, 0.160, 1.000, 0.000, 0.000, 1.000, 0.016,
  0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000
};

// Audio synthesis
const int VOICES = 6;
AudioSynthWaveform       osc1[VOICES], osc2[VOICES], osc3[VOICES];
AudioSynthNoiseWhite     noise1;        // White noise source
AudioSynthNoisePink      noisePink;    // Pink noise source
AudioMixer4              noiseMix;    // Mix white/pink noise
AudioSynthWaveformDc     dcFilter[VOICES]; // DC source for filter envelope per voice
AudioSynthWaveformSine   lfo;             // LFO for modulation
AudioMixer4              oscMix[VOICES]; // Mix 3 oscs + noise per voice
AudioFilterLadder        filter1[VOICES]; // Filter per voice
AudioEffectEnvelope      ampEnv[VOICES], filtEnv[VOICES]; // Envelopes per voice
AudioMixer4              voiceMix1, voiceMix2, finalMix; // Mix voices together
// AudioOutputI2S           i2s1;
AudioOutputUSB           usb1;

// Audio connections - Polyphonic chain
// Voice 0 connections
AudioConnection patchCord1_0(osc1[0], 0, oscMix[0], 0);
AudioConnection patchCord2_0(osc2[0], 0, oscMix[0], 1);
AudioConnection patchCord3_0(osc3[0], 0, oscMix[0], 2);
AudioConnection patchCord4_0(noiseMix, 0, oscMix[0], 3);
AudioConnection patchCord5_0(oscMix[0], 0, ampEnv[0], 0);
AudioConnection patchCord6_0(ampEnv[0], 0, filter1[0], 0);
AudioConnection patchCord7_0(dcFilter[0], filtEnv[0]);
AudioConnection patchCord12_0(filtEnv[0], 0, filter1[0], 1);

// Voice 1 connections
AudioConnection patchCord1_1(osc1[1], 0, oscMix[1], 0);
AudioConnection patchCord2_1(osc2[1], 0, oscMix[1], 1);
AudioConnection patchCord3_1(osc3[1], 0, oscMix[1], 2);
AudioConnection patchCord4_1(noiseMix, 0, oscMix[1], 3);
AudioConnection patchCord5_1(oscMix[1], 0, ampEnv[1], 0);
AudioConnection patchCord6_1(ampEnv[1], 0, filter1[1], 0);
AudioConnection patchCord7_1(dcFilter[1], filtEnv[1]);
AudioConnection patchCord12_1(filtEnv[1], 0, filter1[1], 1);

// Voice 2 connections
AudioConnection patchCord1_2(osc1[2], 0, oscMix[2], 0);
AudioConnection patchCord2_2(osc2[2], 0, oscMix[2], 1);
AudioConnection patchCord3_2(osc3[2], 0, oscMix[2], 2);
AudioConnection patchCord4_2(noiseMix, 0, oscMix[2], 3);
AudioConnection patchCord5_2(oscMix[2], 0, ampEnv[2], 0);
AudioConnection patchCord6_2(ampEnv[2], 0, filter1[2], 0);
AudioConnection patchCord7_2(dcFilter[2], filtEnv[2]);
AudioConnection patchCord12_2(filtEnv[2], 0, filter1[2], 1);

// Voice 3 connections
AudioConnection patchCord1_3(osc1[3], 0, oscMix[3], 0);
AudioConnection patchCord2_3(osc2[3], 0, oscMix[3], 1);
AudioConnection patchCord3_3(osc3[3], 0, oscMix[3], 2);
AudioConnection patchCord4_3(noiseMix, 0, oscMix[3], 3);
AudioConnection patchCord5_3(oscMix[3], 0, ampEnv[3], 0);
AudioConnection patchCord6_3(ampEnv[3], 0, filter1[3], 0);
AudioConnection patchCord7_3(dcFilter[3], filtEnv[3]);
AudioConnection patchCord12_3(filtEnv[3], 0, filter1[3], 1);

// Voice 4 connections
AudioConnection patchCord1_4(osc1[4], 0, oscMix[4], 0);
AudioConnection patchCord2_4(osc2[4], 0, oscMix[4], 1);
AudioConnection patchCord3_4(osc3[4], 0, oscMix[4], 2);
AudioConnection patchCord4_4(noiseMix, 0, oscMix[4], 3);
AudioConnection patchCord5_4(oscMix[4], 0, ampEnv[4], 0);
AudioConnection patchCord6_4(ampEnv[4], 0, filter1[4], 0);
AudioConnection patchCord7_4(dcFilter[4], filtEnv[4]);
AudioConnection patchCord12_4(filtEnv[4], 0, filter1[4], 1);

// Voice 5 connections
AudioConnection patchCord1_5(osc1[5], 0, oscMix[5], 0);
AudioConnection patchCord2_5(osc2[5], 0, oscMix[5], 1);
AudioConnection patchCord3_5(osc3[5], 0, oscMix[5], 2);
AudioConnection patchCord4_5(noiseMix, 0, oscMix[5], 3);
AudioConnection patchCord5_5(oscMix[5], 0, ampEnv[5], 0);
AudioConnection patchCord6_5(ampEnv[5], 0, filter1[5], 0);
AudioConnection patchCord7_5(dcFilter[5], filtEnv[5]);
AudioConnection patchCord12_5(filtEnv[5], 0, filter1[5], 1);

AudioConnection patchCordNoiseWhite(noise1, 0, noiseMix, 0);
AudioConnection patchCordNoisePink(noisePink, 0, noiseMix, 1);

// Mix all voices together
AudioConnection patchCordMix1(filter1[0], 0, voiceMix1, 0);
AudioConnection patchCordMix2(filter1[1], 0, voiceMix1, 1);
AudioConnection patchCordMix3(filter1[2], 0, voiceMix1, 2);
AudioConnection patchCordMix4(filter1[3], 0, voiceMix2, 0);
AudioConnection patchCordMix5(filter1[4], 0, voiceMix2, 1);
AudioConnection patchCordMix6(filter1[5], 0, voiceMix2, 2);

// Combine both voice mixers into final mono mix
AudioConnection patchCordFinal1(voiceMix1, 0, finalMix, 0);
AudioConnection patchCordFinal2(voiceMix2, 0, finalMix, 1);

// Final mono output to both channels
AudioConnection patchCordOut1(finalMix, 0, usb1, 0);
AudioConnection patchCordOut2(finalMix, 0, usb1, 1);
// AudioConnection patchCordOut3(finalMix, 0, i2s1, 0);
// AudioConnection patchCordOut4(finalMix, 0, i2s1, 1);

// AudioControlSGTL5000     sgt15000_1;

// ===== SYNTH PARAMETERS =====
// Polyphonic voice state
struct PolyVoice {
  int note;
  bool active;
  unsigned long noteOnTime;
};

PolyVoice voices[VOICES];
int currentVoice = 0;  // Round-robin voice allocation

// Mono mode note stack for proper note priority
int monoNoteStack[16];  // Stack of held notes in mono mode
int monoStackSize = 0;

// Control values
float osc1Range = 1.0, osc2Range = 1.0, osc3Range = 1.0;
float osc1Fine = 1.0, osc2Fine = 1.0, osc3Fine = 1.0;
int osc1Wave = WAVEFORM_BANDLIMIT_SAWTOOTH, osc2Wave = WAVEFORM_BANDLIMIT_SAWTOOTH, osc3Wave = WAVEFORM_BANDLIMIT_SAWTOOTH;
float vol1 = 0.3, vol2 = 0.3, vol3 = 0.3, noiseVol = 0.0;
float ampAttack = 0, ampSustain = 0.8, ampDecay = 100;
float filtAttack = 100, filtSustain = 0.5, filtDecay = 2500; // Better default filter envelope
float cutoff = 1000, resonance = 0.0; // Changed resonance default to 0.0
float filterStrength = 0.5; // Filter envelope strength (0-1)
float lfoRate = 5.0; // LFO rate in Hz (0.1 - 20Hz)
float lfoDepth = 0.0; // LFO depth (0-1)
bool lfoEnabled = false; // LFO on/off toggle
int lfoTarget = 1; // 0=Pitch, 1=Filter, 2=Amp
float modWheelValue = 0.0; // MIDI mod wheel (CC#1) 0-1
float pitchWheelValue = 0.0; // MIDI pitch wheel -1 to +1
float lastPitchWheelValue = 0.0; // Track changes to prevent unnecessary updates
unsigned long lastMidiTime = 0; // For MIDI throttling
int playMode = 1; // 0=Mono, 1=Poly, 2=Legato
float glideTime = 0.0; // Glide/portamento time (0 = off, 0.1-1.0 = 100ms to 10s)
int noiseType = 0; // 0 = White, 1 = Pink
float targetFreq[VOICES]; // Target frequencies for glide
float currentFreq[VOICES]; // Current frequencies during glide
bool gliding[VOICES]; // Whether each voice is gliding

// Control system
LiquidCrystal_I2C lcd(0x27, 16, 2);
int menuIndex = 0;
bool inMenu = false;

// Control names
const char* controlNames[] = {
  "Osc1 Range", "Osc2 Range", "Osc3 Range", "Osc2 Fine", "Osc3 Fine",
  "Osc1 Wave", "Osc2 Wave", "Osc3 Wave", "Volume 1", "Volume 2", 
  "Volume 3", "Cutoff", "Resonance", "Filt Attack", "Filt Decay",
  "Filt Sustain", "Noise Vol", "Amp Attack", "Amp Sustain", "Amp Decay",
  "Osc1 Fine", "Filt Strength", "LFO Rate", "LFO Depth", "LFO Toggle", "LFO Target", "Play Mode", "Glide Time", "Noise Type"
};


// Preset system
struct MiniTeensyPreset {
  const char* name;
  float parameters[29]; // All 29 parameter values (0.0 to 1.0)
};

// Define 12 classic analog synth presets (iconic synth sounds + blanks for expansion)
// Parameter mapping: 0-2:Osc1-3Range, 
// 3-4:Osc2-3Fine,
// 5-7:Osc1-3Wave,
// 8-10:Osc1-3Vol,
// 11:Cutoff,
// 12:Resonance,
// 13-15:FilterADSR,
// 16:Noise,
// 17-19:AmpADSR,
// 20:Osc1Fine,
// 21:FilterStrength
const MiniTeensyPreset presets[] = {
  {"80s Brass", {0.417, 0.417, 0.417, 0.539, 0.445, 0.417, 0.417, 0.417, 1.000, 0.789, 0.594, 0.562, 0.023, 0.039, 0.160, 0.000, 0.000, 0.000, 1.000, 0.026, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"Saw Keys", {0.417, 0.417, 0.417, 0.453, 0.539, 0.417, 0.417, 0.250, 0.695, 0.789, 0.789, 0.633, 0.039, 0.000, 0.097, 0.469, 0.000, 0.000, 1.000, 0.039, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.580, 0.000}},
  {"Square Keys", {0.417, 0.250, 0.417, 0.453, 0.547, 0.583, 0.917, 0.750, 0.789, 0.789, 0.789, 0.633, 0.008, 0.000, 0.113, 0.000, 0.000, 0.000, 0.448, 0.039, 0.500, 1.000, 0.008, 0.023, 1.000, 0.040, 0.330, 0.000}},
  {"8-Bit Square", {0.417, 0.417, 0.417, 0.500, 0.500, 0.583, 0.417, 0.417, 0.789, 0.000, 0.000, 0.852, 0.000, 0.000, 0.160, 1.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 1.000, 0.000}},
  {"Butter Supersaw", {0.417, 0.417, 0.417, 0.547, 0.453, 0.417, 0.417, 0.417, 0.594, 0.789, 0.789, 0.609, 0.016, 0.000, 0.238, 0.437, 0.020, 0.000, 1.000, 0.008, 0.500, 1.000, 0.008, 0.016, 1.000, 0.330, 0.330, 0.000}},
  {"West Coast Lead", {0.250, 0.417, 0.417, 0.477, 0.523, 0.417, 0.417, 0.250, 0.000, 1.000, 1.000, 0.711, 0.000, 0.000, 0.238, 1.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.930, 0.000}},
  {"Teensy Lead", {0.250, 0.417, 0.417, 0.477, 0.523, 0.583, 0.583, 0.750, 0.000, 1.000, 1.000, 0.633, 0.000, 0.000, 0.238, 1.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 1.000, 0.150, 0.930, 0.711}},
  {"Analog Bass", {0.417, 0.417, 0.250, 0.531, 0.484, 0.417, 0.417, 0.583, 1.000, 1.000, 1.000, 0.617, 0.047, 0.000, 0.019, 0.000, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.016, 0.055, 1.000, 0.330, 0.280, 0.812}},
  {"Legato Bass", {0.250, 0.417, 0.417, 0.477, 0.523, 0.583, 0.417, 0.250, 0.953, 1.000, 1.000, 0.711, 0.023, 0.000, 0.160, 0.633, 0.000, 0.000, 1.000, 0.000, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 1.000, 1.000}},
  {"Funk Bass", {0.250, 0.417, 0.417, 0.539, 0.461, 0.583, 0.417, 0.250, 1.000, 1.000, 1.000, 0.516, 0.172, 0.000, 0.066, 0.000, 0.000, 0.000, 0.828, 0.002, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.230, 0.000}},
  {"8-Bit Harp", {0.750, 0.417, 0.750, 0.523, 0.445, 0.583, 0.750, 0.417, 1.000, 1.000, 0.508, 0.664, 0.000, 0.000, 0.051, 0.000, 0.050, 0.000, 0.000, 0.253, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"Love Pad", {0.417, 0.417, 0.417, 0.414, 0.562, 0.417, 0.417, 0.417, 0.789, 0.789, 0.789, 0.609, 0.109, 0.148, 0.504, 0.000, 0.240, 0.070, 0.863, 0.206, 0.500, 0.250, 0.008, 0.023, 1.000, 0.040, 0.330, 0.000}},
  {"Thoughtful Pad", {0.750, 0.417, 0.250, 0.484, 0.539, 0.583, 0.417, 0.083, 0.227, 0.797, 0.750, 0.445, 0.195, 0.508, 1.000, 0.000, 0.290, 0.141, 1.000, 0.227, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.580, 0.000}},
  {"Saw Pad", {0.417, 0.417, 0.417, 0.578, 0.453, 0.417, 0.417, 0.250, 0.789, 0.789, 0.789, 0.508, 0.023, 0.187, 0.371, 0.508, 0.100, 0.047, 0.820, 0.320, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"5th Pad", {0.500, 0.500, 0.500, 0.895, 0.536, 0.417, 0.417, 0.417, 0.750, 0.750, 0.780, 0.590, 0.060, 0.113, 0.270, 0.230, 0.000, 0.000, 0.800, 0.018, 0.500, 0.500, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"Classic Sweep", {0.417, 0.417, 0.417, 0.578, 0.453, 0.417, 0.417, 0.250, 0.789, 0.789, 0.789, 0.453, 0.312, 0.031, 1.000, 0.000, 0.490, 0.000, 1.000, 0.031, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"Haunted Organ", {0.417, 0.583, 0.750, 0.476, 0.523, 0.083, 0.083, 0.083, 0.766, 0.594, 0.578, 0.516, 0.000, 0.000, 0.019, 0.625, 0.110, 0.000, 0.687, 0.018, 0.500, 1.000, 0.016, 0.039, 1.000, 0.210, 0.330, 0.000}},
  {"Synth Drum", {0.417, 0.583, 0.250, 0.891, 0.445, 0.083, 0.083, 0.583, 0.117, 0.047, 0.016, 0.422, 0.093, 0.000, 0.016, 0.000, 1.000, 0.000, 0.000, 0.026, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"Noise-scape", {0.417, 0.417, 0.417, 0.539, 0.445, 0.417, 0.417, 0.417, 0.000, 0.000, 0.000, 0.203, 0.273, 0.000, 0.555, 1.000, 1.000, 0.187, 1.000, 0.253, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}},
  {"Init", {0.417, 0.417, 0.417, 0.500, 0.500, 0.417, 0.417, 0.417, 0.789, 0.789, 0.789, 1.000, 0.000, 0.000, 0.160, 1.000, 0.000, 0.000, 1.000, 0.016, 0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000}}
};

const int NUM_PRESETS = sizeof(presets) / sizeof(presets[0]); // Dynamic based on presets array
int currentPreset = 0;

// Hierarchical menu system like TeensySynthesizer
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
  // Noise sub-menu
  NOISE_VOLUME,
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
  NOISE_TYPE
};

MenuState currentMenuState = PARENT_MENU;
bool inPresetBrowse = false; // When browsing individual presets
int presetBrowseIndex = 0; // Which preset we're browsing

// Map menu states to parameter indices
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
    case OSC1_FINE: return 20;  // New menu-only parameter
    case FILTER_STRENGTH: return 21;  // New menu-only parameter
    case LFO_RATE: return 22;
    case LFO_DEPTH: return 23;
    case LFO_TOGGLE: return 24;
    case LFO_TARGET: return 25;
    case PLAY_MODE: return 26;
    case GLIDE_TIME: return 27;
    case NOISE_TYPE: return 28;
    default: return -1;
  }
}

// Waveform names for display
const char* waveformNames[] = {
  "Triangle", "Shark Tooth", "Sawtooth", "Wide Pulse", "Med Pulse", "Narrow Pulse"
};

// Range names for display  
const char* rangeNames[] = {
  "32'", "16'", "8'", "4'", "2'", "LO"
};

void updateLFOModulation() {
  // Throttle LFO updates to reduce CPU load
  static unsigned long lastLFOUpdate = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastLFOUpdate < 2) return; // Very fast updates for responsive mod/pitch wheels
  lastLFOUpdate = currentTime;
  
  // Apply pitch wheel to all active voices first
  float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
  
  // Calculate LFO output - authentic analog synth behavior
  float lfoOut = 0.0;
  
  // Authentic vintage behavior: mod wheel always works, LFO is independent
  float phase = (currentTime * lfoRate * 2 * PI) / 1000.0;
  float lfoSignal = sin(phase);
  
  // Calculate total modulation depth
  float totalDepth = 0.0;
  
  // Internal LFO: only active when enabled AND has depth
  if (lfoEnabled && lfoDepth > 0.01) {
    totalDepth += lfoDepth;
  }
  
  // Mod wheel: always works when moved (authentic vintage behavior)
  if (modWheelValue > 0.01) {
    totalDepth += modWheelValue;
  }
  
  // Apply modulation if any source is active
  if (totalDepth > 0.01) {
    lfoOut = lfoSignal * totalDepth;
  }
  
  // Apply LFO based on target
  if (lfoTarget == 0) {
    // Pitch target - apply LFO to oscillators
    float pitchLFOMultiplier = 1.0 + (lfoOut * 0.1); // ±10% frequency change
    for (int v = 0; v < VOICES; v++) {
      if (voices[v].active) {
        // Use current glide frequency if gliding, otherwise calculate from note
        float baseFreq;
        if (gliding[v]) {
          baseFreq = currentFreq[v];
        } else {
          baseFreq = 440.0 * pow(2.0, (voices[v].note - 69) / 12.0);
        }
        float totalPitchMultiplier = pitchWheelMultiplier * pitchLFOMultiplier;
        osc1[v].frequency(baseFreq * osc1Range * osc1Fine * totalPitchMultiplier);
        osc2[v].frequency(baseFreq * osc2Range * osc2Fine * totalPitchMultiplier);
        osc3[v].frequency(baseFreq * osc3Range * osc3Fine * totalPitchMultiplier);
      }
    }
    // Keep filter at base cutoff
    for (int v = 0; v < VOICES; v++) {
      filter1[v].frequency(cutoff);
    }
  } 
  else if (lfoTarget == 1) {
    // Filter target - apply LFO to filter cutoff
    float filterModulation = lfoOut * 1000.0; // ±1000 Hz
    float modulatedCutoff = constrain(cutoff + filterModulation, 20.0, 20000.0);
    for (int v = 0; v < VOICES; v++) {
      filter1[v].frequency(modulatedCutoff);
    }
    // Keep oscillators at base pitch (with pitch wheel, using glide if active)
    for (int v = 0; v < VOICES; v++) {
      if (voices[v].active) {
        // Use current glide frequency if gliding, otherwise calculate from note
        float baseFreq;
        if (gliding[v]) {
          baseFreq = currentFreq[v];
        } else {
          baseFreq = 440.0 * pow(2.0, (voices[v].note - 69) / 12.0);
        }
        osc1[v].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
        osc2[v].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
        osc3[v].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
      }
    }
  }
  else if (lfoTarget == 2) {
    // Amplitude target - apply LFO to mixer gains
    float ampMultiplier = 1.0 + (lfoOut * 0.5); // ±50% amplitude
    for (int v = 0; v < VOICES; v++) {
      oscMix[v].gain(0, vol1 * ampMultiplier);
      oscMix[v].gain(1, vol2 * ampMultiplier);
      oscMix[v].gain(2, vol3 * ampMultiplier);
    }
    // Keep oscillators at base pitch (with pitch wheel, using glide if active)
    for (int v = 0; v < VOICES; v++) {
      if (voices[v].active) {
        // Use current glide frequency if gliding, otherwise calculate from note
        float baseFreq;
        if (gliding[v]) {
          baseFreq = currentFreq[v];
        } else {
          baseFreq = 440.0 * pow(2.0, (voices[v].note - 69) / 12.0);
        }
        osc1[v].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
        osc2[v].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
        osc3[v].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
      }
    }
    // Keep filter at base cutoff
    for (int v = 0; v < VOICES; v++) {
      filter1[v].frequency(cutoff);
    }
  }
}

void updateGlide() {
  if (glideTime == 0.0) return; // Glide is off
  
  static unsigned long lastGlideUpdate = 0;
  unsigned long currentTime = millis();
  
  // Update glide every 5ms for smooth transitions
  if (currentTime - lastGlideUpdate < 3) return; // Very smooth glide transitions
  lastGlideUpdate = currentTime;
  
  float glideTimeMs = 50 + (glideTime * 950); // 50ms to 1000ms (1 second max)
  float glideRate = 10.0 / glideTimeMs; // Much more aggressive rate
  
  for (int v = 0; v < VOICES; v++) {
    if (voices[v].active && gliding[v]) {
      // Calculate frequency delta
      float freqDelta = targetFreq[v] - currentFreq[v];
      
      // If we're very close, snap to target and stop gliding
      if (abs(freqDelta) < 0.1) {
        currentFreq[v] = targetFreq[v];
        gliding[v] = false;
      } else {
        // Move towards target frequency
        currentFreq[v] += freqDelta * glideRate * 50.0; // Much faster transition
      }
      
      // Apply the current frequency to oscillators with all multipliers
      // Only set frequency directly if LFO is not targeting pitch (LFO will handle it)
      if (lfoTarget != 0) {
        float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
        osc1[v].frequency(currentFreq[v] * osc1Range * osc1Fine * pitchWheelMultiplier);
        osc2[v].frequency(currentFreq[v] * osc2Range * osc2Fine * pitchWheelMultiplier);
        osc3[v].frequency(currentFreq[v] * osc3Range * osc3Fine * pitchWheelMultiplier);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  AudioMemory(48); // Reduced from 60 to minimize latency
  
  // USB Device MIDI is automatically initialized
  
  // Initialize Audio
  
  // Initialize LFO
  lfo.frequency(lfoRate);
  lfo.amplitude(1.0); // Full amplitude - we'll control depth in software
  
  
  // Other encoders work fine, only menu encoder needs explicit setup
  
  // Menu encoder setup - let Encoder library handle pin modes like other encoders
  pinMode(MENU_ENCODER_SW, INPUT_PULLUP);
  
  // Initialize encoder values
  for (int i = 0; i < 20; i++) {
    encoderValues[i] = 0;
    lastEncoderValues[i] = 0;
  }
  
  // Initialize all parameter values to their current states
  for (int i = 0; i < 29; i++) {
    updateSynthParameter(i, allParameterValues[i]);
  }
  
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MiniTeensy Synth");
  lcd.setCursor(0, 1);
  lcd.print("6-Voice Poly");
  
  // Initialize all voice arrays
  for (int v = 0; v < VOICES; v++) {
    // Initialize oscillators
    osc1[v].begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    osc2[v].begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    osc3[v].begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    
    // Set initial amplitudes - oscillators always on, controlled by mixer
    osc1[v].amplitude(0.8); // Reduced amplitude
    osc2[v].amplitude(0.8);
    osc3[v].amplitude(0.8);
    
    // Configure mixer gains - reduced to prevent clipping
    oscMix[v].gain(0, vol1 * 0.8); // Reduced gain
    oscMix[v].gain(1, vol2 * 0.8); 
    oscMix[v].gain(2, vol3 * 0.8); 
    oscMix[v].gain(3, 0.0);  // Noise (controlled by noiseVol)
    
    // Configure DC source for filter envelope
    dcFilter[v].amplitude(filterStrength);
    
    // Configure filter (ladder filter like working script)
    filter1[v].frequency(cutoff);
    filter1[v].resonance(0.0); // Start with no resonance
    filter1[v].octaveControl(3.0); // Back to 3.0 like working script
    
    // Configure envelopes
    ampEnv[v].attack(ampAttack);
    ampEnv[v].sustain(ampSustain);
    ampEnv[v].decay(ampDecay);
    ampEnv[v].release(ampDecay);
    
    filtEnv[v].attack(filtAttack);
    filtEnv[v].sustain(filtSustain);
    filtEnv[v].decay(filtDecay);
    filtEnv[v].release(filtDecay);
    
    // Initialize voice state
    voices[v].note = 0;
    voices[v].active = false;
    voices[v].noteOnTime = 0;
    
    // Initialize glide state
    targetFreq[v] = 0.0;
    currentFreq[v] = 0.0;
    gliding[v] = false;
  }
  
  // Configure noise (shared across all voices)
  noise1.amplitude(0.5); // Reduced noise amplitude
  noisePink.amplitude(0.5); // Pink noise amplitude
  // Initialize noise mixer - start with white noise
  noiseMix.gain(0, 1.0); // White noise
  noiseMix.gain(1, 0.0); // Pink noise off initially
  
  // Configure voice mixers with higher gain
  voiceMix1.gain(0, 1.0); // Maximum gain for better volume
  voiceMix1.gain(1, 1.0);
  voiceMix1.gain(2, 1.0);
  voiceMix1.gain(3, 0.0);
  
  voiceMix2.gain(0, 1.0);
  voiceMix2.gain(1, 1.0);
  voiceMix2.gain(2, 1.0);
  voiceMix2.gain(3, 0.0);
  
  // Configure final mono mixer with reduced master output
  finalMix.gain(0, 0.6); // Reduced master output gain
  finalMix.gain(1, 0.6); // Reduced master output gain
  finalMix.gain(2, 0.0);
  finalMix.gain(3, 0.0);
  
  // Control values already initialized above
  
// sgt15000_1.enable();
//     sgt15000_1.volume(1);

  delay(2000);
  updateDisplay();
  Serial.println("Monophonic MiniTeensy Synthesizer Ready!");


    
}


// Read all direct encoders
void readDirectEncoders() {
  encoderValues[0] = enc1.read() / 4;     // Osc1 Range
  encoderValues[1] = enc2.read() / 4;     // Osc2 Range
  encoderValues[2] = enc3.read() / 4;     // Osc3 Range
  encoderValues[3] = enc4.read() / 4;     // Osc2 Fine
  encoderValues[4] = enc5.read() / 4;     // Osc3 Fine
  encoderValues[5] = enc6.read() / 4;     // Osc1 Wave
  encoderValues[6] = enc7.read() / 4;     // Osc2 Wave
  encoderValues[7] = enc8.read() / 4;     // Osc3 Wave
  encoderValues[8] = enc9.read() / 4;     // Volume 1
  encoderValues[9] = enc10.read() / 4;    // Volume 2
  encoderValues[10] = enc11.read() / 4;   // Volume 3
  // encoderValues[11] handled separately for cutoff (uses menuEncoder hardware)
  encoderValues[12] = enc13.read() / 4;   // Resonance
  encoderValues[13] = enc14.read() / 4;   // Filter Attack
  encoderValues[14] = enc15.read() / 4;   // Filter Decay/Release
  encoderValues[15] = enc16.read() / 4;   // Filter Sustain
  encoderValues[16] = enc17.read() / 4;   // Noise Volume
  encoderValues[17] = enc18.read() / 4;   // Amp Attack
  encoderValues[18] = enc19.read() / 4;   // Amp Sustain
  encoderValues[19] = enc20.read() / 4;   // Amp Decay
}


void readAllControls() {
  // Read all encoders
  readDirectEncoders();
  
  // Check for encoder changes and update parameters
  for (int i = 0; i < 20; i++) {
    if (encoderValues[i] != lastEncoderValues[i]) {
      // If any physical knob is turned, exit menu mode
      if (inMenu) {
        inMenu = false;
      }
      
      int change = encoderValues[i] - lastEncoderValues[i];
      updateEncoderParameter(i, change);
      lastEncoderValues[i] = encoderValues[i];
    }
  }
}

// Core synthesis parameter update function
void updateSynthParameter(int paramIndex, float val) {
  // Convert to appropriate ranges and update parameters
  switch (paramIndex) {
    case 0: // Osc1 Range
      osc1Range = getOscillatorRange(val);
      updateOscillatorFrequencies();
      break;
    case 1: // Osc2 Range  
      osc2Range = getOscillatorRange(val);
      updateOscillatorFrequencies();
      break;
    case 2: // Osc3 Range
      osc3Range = getOscillatorRange(val);
      updateOscillatorFrequencies();
      break;
    case 3: {// Osc2 Fine
      // Extended fine tuning: ±25 cents, then ±12 semitones
      float totalCents;
      if (val <= 0.25) {
        // -12 to -1 semitones (left range)
        float semiRange = val / 0.25; // 0.0 to 1.0
        totalCents = -1200 + (semiRange * 1175); // -1200 to -25 cents
      } else if (val >= 0.75) {
        // +1 to +12 semitones (right range)
        float semiRange = (val - 0.75) / 0.25; // 0.0 to 1.0
        totalCents = 25 + (semiRange * 1175); // +25 to +1200 cents
      } else {
        // ±25 cents fine tuning (center range)
        totalCents = (val - 0.5) * 100.0; // -25 to +25 cents
      }
      osc2Fine = pow(2.0, totalCents / 1200.0);
      updateOscillatorFrequencies();
      break;}
    case 4: {// Osc3 Fine
      // Extended fine tuning: ±25 cents, then ±12 semitones
      float totalCents;
      if (val <= 0.25) {
        // -12 to -1 semitones (left range)
        float semiRange = val / 0.25; // 0.0 to 1.0
        totalCents = -1200 + (semiRange * 1175); // -1200 to -25 cents
      } else if (val >= 0.75) {
        // +1 to +12 semitones (right range)
        float semiRange = (val - 0.75) / 0.25; // 0.0 to 1.0
        totalCents = 25 + (semiRange * 1175); // +25 to +1200 cents
      } else {
        // ±25 cents fine tuning (center range)
        totalCents = (val - 0.5) * 100.0; // -25 to +25 cents
      }
      osc3Fine = pow(2.0, totalCents / 1200.0);
      updateOscillatorFrequencies();
      break;}
    case 5: // Osc1 Wave
      osc1Wave = getMiniTeensyWaveform(val, 1);
      updateWaveforms();
      break;
    case 6: // Osc2 Wave
      osc2Wave = getMiniTeensyWaveform(val, 2);
      updateWaveforms();
      break;
    case 7: // Osc3 Wave
      osc3Wave = getMiniTeensyWaveform(val, 3);
      updateWaveforms();
      break;
    case 8: // Volume 1
      vol1 = val * 0.8; // Increased gain from 0.4 to 0.8
      updateMixerLevels();
      break;
    case 9: // Volume 2
      vol2 = val * 0.8; // Increased gain from 0.4 to 0.8
      updateMixerLevels();
      break;
    case 10: // Volume 3
      vol3 = val * 0.8; // Increased gain from 0.4 to 0.8
      updateMixerLevels();
      break;
    case 11: // Cutoff
      // Logarithmic frequency response like analog synth (20Hz to 20kHz)
      cutoff = 20 * pow(1000.0, val); // 20Hz to 20kHz logarithmic
      for (int v = 0; v < VOICES; v++) {
        filter1[v].frequency(cutoff);
      }
      break;
    case 12: // Resonance
      resonance = val * 3.0;
      for (int v = 0; v < VOICES; v++) {
        filter1[v].resonance(resonance);
      }
      break;
    case 13: // Filter Attack
      filtAttack = 1 + val * 3000;
      updateEnvelopes();
      break;
    case 14: // Filter Decay/Release
      filtDecay = 10 + val * 5000;
      updateEnvelopes();
      break;
    case 15: // Filter Sustain
      filtSustain = val;
      updateEnvelopes();
      break;
    case 16: // Noise Volume (menu-only)
      noiseVol = val * 0.6; // Increased gain from 0.3 to 0.6
      updateNoiseLevel();
      break;
    case 17: // Amp Attack (menu-only)
      ampAttack = 1 + val * 3000;
      updateEnvelopes();
      break;
    case 18: // Amp Sustain (menu-only)
      ampSustain = val;
      updateEnvelopes();
      break;
    case 19: // Amp Decay (menu-only)
      ampDecay = 10 + val * 5000;
      updateEnvelopes();
      break;
    case 20: { // Osc1 Fine Tune (menu-only) - Extended fine tuning: ±25 cents, then ±12 semitones
      float totalCents;
      if (val <= 0.25) {
        // -12 to -1 semitones (left range)
        float semiRange = val / 0.25; // 0.0 to 1.0
        totalCents = -1200 + (semiRange * 1175); // -1200 to -25 cents
      } else if (val >= 0.75) {
        // +1 to +12 semitones (right range)
        float semiRange = (val - 0.75) / 0.25; // 0.0 to 1.0
        totalCents = 25 + (semiRange * 1175); // +25 to +1200 cents
      } else {
        // ±25 cents fine tuning (center range)
        totalCents = (val - 0.5) * 100.0; // -25 to +25 cents
      }
      osc1Fine = pow(2.0, totalCents / 1200.0);
      updateOscillatorFrequencies();
      break;
    }
    case 21: // Filter Strength (menu-only)
      filterStrength = val; // 0.0 to 1.0 envelope modulation amount
      // Update all voices immediately
      for (int v = 0; v < VOICES; v++) {
        dcFilter[v].amplitude(filterStrength);
      }
      break;
    case 22: // LFO Rate (menu-only)
      lfoRate = 0.1 + val * 19.9; // 0.1 to 20 Hz
      lfo.frequency(lfoRate);
      break;
    case 23: // LFO Depth (menu-only)
      lfoDepth = val; // 0.0 to 1.0
      break;
    case 24: // LFO Toggle (menu-only)
      lfoEnabled = (val > 0.5); // Toggle at 50%
      break;
    case 25: // LFO Target (menu-only)
      if (val < 0.33) lfoTarget = 0; // Pitch
      else if (val < 0.66) lfoTarget = 1; // Filter
      else lfoTarget = 2; // Amp
      break;
    case 26: // Play Mode (menu-only)
      if (val < 0.33) playMode = 0; // Mono
      else if (val < 0.66) playMode = 1; // Poly
      else playMode = 2; // Legato
      break;
    case 27: // Glide Time (menu-only)
      glideTime = val; // 0.0 to 1.0 (0 = off, 0.1-1.0 = 100ms to 10s)
      break;
    case 28: // Noise Type (menu-only)
      noiseType = (val > 0.5) ? 1 : 0; // 0 = White, 1 = Pink
      if (noiseType == 0) {
        noiseMix.gain(0, 1.0); // White noise on
        noiseMix.gain(1, 0.0); // Pink noise off
      } else {
        noiseMix.gain(0, 0.0); // White noise off
        noiseMix.gain(1, 1.0); // Pink noise on
      }
      break;
  }
}

// Menu-based parameter update function
void updateParameterFromMenu(int paramIndex, float val) {
  updateSynthParameter(paramIndex, val);
  
  // Update display for real-time feedback
  updateDisplay();
}

// Encoder-based parameter update function  
void updateEncoderParameter(int paramIndex, int change) {
  // Different increments for different parameter types
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
    case 24: case 25: case 26: case 28: // Toggle/discrete controls (LFO Toggle, LFO Target, Play Mode, Noise Type)
      increment = 0.5; // Large steps for immediate toggle response
      break;
    default:
      increment = 0.01; // Standard increment
      break;
  }
  
  // Update the global parameter array
  allParameterValues[paramIndex] = constrain(allParameterValues[paramIndex] + (change * increment), 0.0, 1.0);
  float val = allParameterValues[paramIndex];
  
  // Snap discrete controls to exact threshold values
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
  
  
  // Update the synthesis parameter
  updateSynthParameter(paramIndex, val);
  
  // Update display
  if (!inMenu) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(controlNames[paramIndex]);
    lcd.setCursor(0, 1);
    
    if (paramIndex >= 5 && paramIndex <= 7) { // Waveform controls
      int waveIndex = getWaveformIndex(val, (paramIndex == 5) ? 1 : ((paramIndex == 6) ? 2 : 3));
      lcd.print(waveformNames[waveIndex]);
    }
    else if (paramIndex >= 0 && paramIndex <= 2) { // Range controls
      int rangeIndex = getRangeIndex(val);
      lcd.print(rangeNames[rangeIndex]);
    }
    else if (paramIndex == 3 || paramIndex == 4) { // Extended fine tuning controls
      // Calculate display value based on range
      if (val <= 0.25) {
        // Semitone range (negative)
        float semiRange = val / 0.25;
        int semitones = (int)(-12 + (semiRange * 11)); // -12 to -1
        lcd.print(semitones);
        lcd.print("st");
      } else if (val >= 0.75) {
        // Semitone range (positive)
        float semiRange = (val - 0.75) / 0.25;
        int semitones = (int)(1 + (semiRange * 11)); // +1 to +12
        lcd.print("+");
        lcd.print(semitones);
        lcd.print("st");
      } else {
        // Cents range (±25 cents)
        int cents = (int)((val - 0.5) * 100); // -25 to +25 cents
        if (cents >= 0) lcd.print("+");
        lcd.print(cents);
        lcd.print("c");
      }
    }
    else {
      // Show raw parameter value for other controls
      int displayValue = (int)(val * 127); // 0-127 MIDI scale
      lcd.print(displayValue);
    }
  }
}

float getOscillatorRange(float val) {
  if (val < 0.167) return 0.25;        // 32'
  else if (val < 0.333) return 0.5;    // 16'
  else if (val < 0.5) return 1.0;      // 8'
  else if (val < 0.667) return 2.0;    // 4'
  else if (val < 0.833) return 4.0;    // 2'
  else return 0.0625;                  // LO
}

int getRangeIndex(float val) {
  if (val < 0.167) return 0;
  else if (val < 0.333) return 1;
  else if (val < 0.5) return 2;
  else if (val < 0.667) return 3;
  else if (val < 0.833) return 4;
  else return 5;
}

int getWaveformIndex(float val, int osc) {
  if (osc == 1 || osc == 2) {
    if (val < 0.167) return 0;
    else if (val < 0.333) return 1;
    else if (val < 0.5) return 2;
    else if (val < 0.667) return 3;
    else if (val < 0.833) return 4;
    else return 5;
  } else {
    if (val < 0.167) return 0;
    else if (val < 0.333) return 2;
    else if (val < 0.5) return 2;
    else if (val < 0.667) return 3;
    else if (val < 0.833) return 4;
    else return 5;
  }
}

int getMiniTeensyWaveform(float val, int osc) {
  if (osc == 1 || osc == 2) {
    if (val < 0.167) return WAVEFORM_TRIANGLE;
    else if (val < 0.333) return WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE;
    else if (val < 0.5) return WAVEFORM_BANDLIMIT_SAWTOOTH;
    else if (val < 0.667) return WAVEFORM_BANDLIMIT_SQUARE;
    else if (val < 0.833) return WAVEFORM_BANDLIMIT_PULSE;
    else return WAVEFORM_BANDLIMIT_PULSE;
  } else {
    if (val < 0.167) return WAVEFORM_TRIANGLE;
    else if (val < 0.333) return WAVEFORM_BANDLIMIT_SAWTOOTH;
    else if (val < 0.5) return WAVEFORM_BANDLIMIT_SAWTOOTH;
    else if (val < 0.667) return WAVEFORM_BANDLIMIT_SQUARE;
    else if (val < 0.833) return WAVEFORM_BANDLIMIT_PULSE;
    else return WAVEFORM_BANDLIMIT_PULSE;
  }
}

void updateOscillatorFrequencies() {
  // Update all active voices
  for (int v = 0; v < VOICES; v++) {
    if (voices[v].active) {
      float baseFreq = 440.0 * pow(2.0, (voices[v].note - 69) / 12.0);
      osc1[v].frequency(baseFreq * osc1Range * osc1Fine);
      osc2[v].frequency(baseFreq * osc2Range * osc2Fine);
      osc3[v].frequency(baseFreq * osc3Range * osc3Fine);
    }
  }
}

void updateWaveforms() {
  // Update waveforms for all voices
  for (int v = 0; v < VOICES; v++) {
    osc1[v].begin(osc1Wave);
    osc2[v].begin(osc2Wave);
    osc3[v].begin(osc3Wave);
  }
}

void updateMixerLevels() {
  // Update mixer levels for all voices
  for (int v = 0; v < VOICES; v++) {
    oscMix[v].gain(0, vol1 * 1.0); // Full volume levels
    oscMix[v].gain(1, vol2 * 1.0);
    oscMix[v].gain(2, vol3 * 1.0);
  }
}

void updateNoiseLevel() {
  // Update noise level for all voices
  for (int v = 0; v < VOICES; v++) {
    oscMix[v].gain(3, noiseVol);
  }
}

void updateEnvelopes() {
  // Update envelopes for all voices
  for (int v = 0; v < VOICES; v++) {
    ampEnv[v].attack(ampAttack);
    ampEnv[v].sustain(ampSustain);
    ampEnv[v].decay(ampDecay);
    ampEnv[v].release(ampDecay);
    
    filtEnv[v].attack(filtAttack);
    filtEnv[v].sustain(filtSustain);
    filtEnv[v].decay(filtDecay);
    filtEnv[v].release(filtDecay);
  }
}

// Find next voice using round-robin allocation
int findAvailableVoice() {
  // Start from current voice and look for next available
  for (int i = 0; i < VOICES; i++) {
    int v = (currentVoice + i) % VOICES;
    if (!voices[v].active) {
      currentVoice = (v + 1) % VOICES; // Set next voice for next time
      return v;
    }
  }
  
  // If all voices are active, use round-robin for voice stealing
  int voiceToSteal = currentVoice;
  currentVoice = (currentVoice + 1) % VOICES; // Advance for next time
  return voiceToSteal;
}

// Find voice playing a specific note
int findVoiceForNote(int note) {
  for (int v = 0; v < VOICES; v++) {
    if (voices[v].active && voices[v].note == note) {
      return v;
    }
  }
  return -1; // Note not found
}

// Mono mode note stack management
void addToMonoStack(int note) {
  // Don't add if already in stack
  for (int i = 0; i < monoStackSize; i++) {
    if (monoNoteStack[i] == note) return;
  }
  // Add to top of stack
  if (monoStackSize < 16) {
    monoNoteStack[monoStackSize] = note;
    monoStackSize++;
  }
}

void removeFromMonoStack(int note) {
  // Find and remove note from stack
  for (int i = 0; i < monoStackSize; i++) {
    if (monoNoteStack[i] == note) {
      // Shift remaining notes down
      for (int j = i; j < monoStackSize - 1; j++) {
        monoNoteStack[j] = monoNoteStack[j + 1];
      }
      monoStackSize--;
      break;
    }
  }
}

int getTopMonoNote() {
  if (monoStackSize > 0) {
    return monoNoteStack[monoStackSize - 1]; // Return most recent note
  }
  return -1; // No notes in stack
}

void noteOn(int note, int velocity) {
  int voiceNum = -1;
  
  if (playMode == 0) {
    // Mono mode - use note stack but retrigger envelope for every new note
    addToMonoStack(note);
    
    // Turn off other voices if they're somehow active
    for (int v = 1; v < VOICES; v++) {
      if (voices[v].active) {
        ampEnv[v].noteOff();
        filtEnv[v].noteOff();
        voices[v].active = false;
      }
    }
    
    // Always use voice 0 for mono mode
    voiceNum = 0;
    
    // If this is not the top note, don't trigger it yet
    if (getTopMonoNote() != note) {
      return; // Wait until this becomes the top note
    }
    
    // Stop current envelope if active
    if (voices[0].active) {
      ampEnv[0].noteOff();
      filtEnv[0].noteOff();
    }
    
    // Check if voice was active before setting up new note (for glide)
    bool wasActive = voices[0].active;
    
    // Set up the voice
    voices[0].note = note;
    voices[0].active = true;
    voices[0].noteOnTime = millis();
    
    // Calculate and set frequencies
    float baseFreq = 440.0 * pow(2.0, (note - 69) / 12.0);
    float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
    
    if (glideTime > 0.0 && wasActive) {
      // Glide enabled and voice was already active - start glide to new frequency
      targetFreq[0] = baseFreq;
      gliding[0] = true;
      // currentFreq[0] keeps its current value to glide from
    } else {
      // No glide or first note - set frequency directly
      currentFreq[0] = baseFreq;
      targetFreq[0] = baseFreq;
      gliding[0] = false;
      osc1[0].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
      osc2[0].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
      osc3[0].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
    }
    
    // Always trigger envelopes in mono mode (retrigger for every note)
    ampEnv[0].noteOn();
    filtEnv[0].noteOn();
  } 
  else if (playMode == 2) {
    // Legato mode - use note stack for smooth transitions without envelope retrigger
    addToMonoStack(note);
    
    // Always use voice 0 for legato mode
    voiceNum = 0;
    
    // Turn off other voices if they're somehow active
    for (int v = 1; v < VOICES; v++) {
      if (voices[v].active) {
        ampEnv[v].noteOff();
        filtEnv[v].noteOff();
        voices[v].active = false;
      }
    }
    
    // If this is not the top note, don't trigger it yet
    if (getTopMonoNote() != note) {
      return; // Wait until this becomes the top note
    }
    
    // Don't retrigger envelope if voice 0 is already active (legato behavior)
    bool wasActive = voices[0].active;
    voices[0].note = note;
    voices[0].active = true;
    voices[0].noteOnTime = millis();
    
    // Calculate and set frequencies
    float baseFreq = 440.0 * pow(2.0, (note - 69) / 12.0);
    float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
    
    if (glideTime > 0.0 && wasActive) {
      // Glide enabled and voice was already active - start glide to new frequency
      targetFreq[0] = baseFreq;
      gliding[0] = true;
      // currentFreq[0] keeps its current value to glide from
    } else {
      // No glide or first note - set frequency directly
      currentFreq[0] = baseFreq;
      targetFreq[0] = baseFreq;
      gliding[0] = false;
      osc1[0].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
      osc2[0].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
      osc3[0].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
    }
    
    // Only trigger envelopes if no note was previously active
    if (!wasActive) {
      ampEnv[0].noteOn();
      filtEnv[0].noteOn();
    }
  } 
  else {
    // Poly mode - normal polyphonic behavior
    voiceNum = findAvailableVoice();
    
    // Check if voice was active before stealing (for glide)
    bool wasActive = voices[voiceNum].active;
    
    // If voice stealing, turn off the old note
    if (voices[voiceNum].active) {
      ampEnv[voiceNum].noteOff();
      filtEnv[voiceNum].noteOff();
    }
    
    // Set up the voice
    voices[voiceNum].note = note;
    voices[voiceNum].active = true;
    voices[voiceNum].noteOnTime = millis();
    
    // Calculate and set frequencies
    float baseFreq = 440.0 * pow(2.0, (note - 69) / 12.0);
    float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
    
    if (glideTime > 0.0 && wasActive) {
      // Glide enabled and voice was being reused - start glide to new frequency
      targetFreq[voiceNum] = baseFreq;
      gliding[voiceNum] = true;
      // currentFreq[voiceNum] keeps its current value to glide from
    } else {
      // No glide or new voice - set frequency directly
      currentFreq[voiceNum] = baseFreq;
      targetFreq[voiceNum] = baseFreq;
      gliding[voiceNum] = false;
      osc1[voiceNum].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
      osc2[voiceNum].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
      osc3[voiceNum].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
    }
    
    // Always trigger envelopes in poly mode
    ampEnv[voiceNum].noteOn();
    filtEnv[voiceNum].noteOn();
  }
}

void noteOff(int note) {
  if (playMode == 0) {
    // Mono mode - use note stack and retrigger envelope for next note
    removeFromMonoStack(note);
    
    // If the released note was the currently playing note
    if (voices[0].active && voices[0].note == note) {
      // Stop current envelope
      ampEnv[0].noteOff();
      filtEnv[0].noteOff();
      
      int nextNote = getTopMonoNote();
      if (nextNote != -1) {
        // Play the next note in the stack WITH envelope retrigger (mono behavior)
        voices[0].note = nextNote;
        float baseFreq = 440.0 * pow(2.0, (nextNote - 69) / 12.0);
        float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
        
        if (glideTime > 0.0) {
          // Start glide to the next note
          targetFreq[0] = baseFreq;
          gliding[0] = true;
          // currentFreq[0] keeps its current value to glide from
        } else {
          // No glide - set frequency directly
          currentFreq[0] = baseFreq;
          targetFreq[0] = baseFreq;
          gliding[0] = false;
          osc1[0].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
          osc2[0].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
          osc3[0].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
        }
        
        // Retrigger envelopes for the next note (mono behavior)
        ampEnv[0].noteOn();
        filtEnv[0].noteOn();
      } else {
        // No more notes - voice stays off
        voices[0].active = false;
      }
    }
  } 
  else if (playMode == 2) {
    // Legato mode - use note stack for proper priority
    removeFromMonoStack(note);
    
    // If the released note was the currently playing note
    if (voices[0].active && voices[0].note == note) {
      int nextNote = getTopMonoNote();
      if (nextNote != -1) {
        // Play the next note in the stack without retriggering envelopes
        voices[0].note = nextNote;
        float baseFreq = 440.0 * pow(2.0, (nextNote - 69) / 12.0);
        float pitchWheelMultiplier = pow(2.0, pitchWheelValue * 2.0 / 12.0);
        
        if (glideTime > 0.0) {
          // Start glide to the next note
          targetFreq[0] = baseFreq;
          gliding[0] = true;
          // currentFreq[0] keeps its current value to glide from
        } else {
          // No glide - set frequency directly
          currentFreq[0] = baseFreq;
          targetFreq[0] = baseFreq;
          gliding[0] = false;
          osc1[0].frequency(baseFreq * osc1Range * osc1Fine * pitchWheelMultiplier);
          osc2[0].frequency(baseFreq * osc2Range * osc2Fine * pitchWheelMultiplier);
          osc3[0].frequency(baseFreq * osc3Range * osc3Fine * pitchWheelMultiplier);
        }
      } else {
        // No more notes - turn off envelopes
        ampEnv[0].noteOff();
        filtEnv[0].noteOff();
        voices[0].active = false;
      }
    }
  } 
  else {
    // Poly mode - normal note off behavior
    int voiceNum = findVoiceForNote(note);
    if (voiceNum >= 0) {
      // Turn off envelopes
      ampEnv[voiceNum].noteOff();
      filtEnv[voiceNum].noteOff();
      voices[voiceNum].active = false;
    }
  }
}

// Navigation functions for hierarchical menu
void navigateMenuForward() {
  switch(currentMenuState) {
    case PARENT_MENU:
      if (menuIndex == 0) {
        // Skip PRESETS state and go directly to preset browse
        inPresetBrowse = true;
        presetBrowseIndex = 0;
        return; // Don't change currentMenuState, stay in PARENT_MENU
      }
      else if (menuIndex == 1) currentMenuState = OSC_1;
      else if (menuIndex == 2) currentMenuState = OSC_2;
      else if (menuIndex == 3) currentMenuState = OSC_3;
      else if (menuIndex == 4) currentMenuState = NOISE;
      else if (menuIndex == 5) currentMenuState = ENVELOPES;
      else if (menuIndex == 6) currentMenuState = FILTER;
      else if (menuIndex == 7) currentMenuState = LFO;
      else if (menuIndex == 8) currentMenuState = VOICE_MODE;
      else if (menuIndex == 9) {
        // Exit menu completely
        inMenu = false;
        inPresetBrowse = false;
        return;
      }
      menuIndex = 0; // Reset index for sub-menu
      break;
    case OSC_1:
      if (menuIndex == 0) currentMenuState = OSC1_RANGE;
      else if (menuIndex == 1) currentMenuState = OSC1_WAVE;
      else if (menuIndex == 2) currentMenuState = OSC1_VOLUME;
      else if (menuIndex == 3) currentMenuState = OSC1_FINE;
      else if (menuIndex == 4) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 1; // Set to OSC_1 position in main menu
        return;
      }
      break;
    case OSC_2:
      if (menuIndex == 0) currentMenuState = OSC2_RANGE;
      else if (menuIndex == 1) currentMenuState = OSC2_WAVE;
      else if (menuIndex == 2) currentMenuState = OSC2_VOLUME;
      else if (menuIndex == 3) currentMenuState = OSC2_FINE;
      else if (menuIndex == 4) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 2; // Set to OSC_2 position in main menu
        return;
      }
      break;
    case OSC_3:
      if (menuIndex == 0) currentMenuState = OSC3_RANGE;
      else if (menuIndex == 1) currentMenuState = OSC3_WAVE;
      else if (menuIndex == 2) currentMenuState = OSC3_VOLUME;
      else if (menuIndex == 3) currentMenuState = OSC3_FINE;
      else if (menuIndex == 4) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 3; // Set to OSC_3 position in main menu
        return;
      }
      break;
    case NOISE:
      if (menuIndex == 0) currentMenuState = NOISE_VOLUME;
      else if (menuIndex == 1) currentMenuState = NOISE_TYPE;
      else if (menuIndex == 2) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 4; // Set to NOISE position in main menu
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
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 5; // Set to ENVELOPES position in main menu
        return;
      }
      break;
    case FILTER:
      if (menuIndex == 0) currentMenuState = CUTOFF;
      else if (menuIndex == 1) currentMenuState = RESONANCE;
      else if (menuIndex == 2) currentMenuState = FILTER_STRENGTH;
      else if (menuIndex == 3) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 6; // Set to FILTER position in main menu
        return;
      }
      break;
    case LFO:
      if (menuIndex == 0) currentMenuState = LFO_RATE;
      else if (menuIndex == 1) currentMenuState = LFO_DEPTH;
      else if (menuIndex == 2) currentMenuState = LFO_TOGGLE;
      else if (menuIndex == 3) currentMenuState = LFO_TARGET;
      else if (menuIndex == 4) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 7; // Set to LFO position in main menu
        return;
      }
      break;
    case VOICE_MODE:
      if (menuIndex == 0) currentMenuState = PLAY_MODE;
      else if (menuIndex == 1) currentMenuState = GLIDE_TIME;
      else if (menuIndex == 2) {
        // Back to main menu
        currentMenuState = PARENT_MENU;
        menuIndex = 8; // Set to VOICE_MODE position in main menu
        return;
      }
      break;
    default:
      // Already in a parameter, can't go deeper
      break;
  }
}

void incrementMenuIndex() {
  switch(currentMenuState) {
    case PARENT_MENU:
      menuIndex++;
      if (menuIndex > 9) menuIndex = 0; // Wrap to first item (now 10 items: 0-9)
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
    default:
      // In a parameter menu, no navigation
      break;
  }
}

void decrementMenuIndex() {
  switch(currentMenuState) {
    case PARENT_MENU:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 9; // Wrap to last item (Exit)
      break;
    case OSC_1:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // Wrap to Back button (0-4)
      break;
    case OSC_2:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // Wrap to Back button (0-4)
      break;
    case OSC_3:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // Wrap to Back button (0-4)
      break;
    case NOISE:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2; // Wrap to Back button (0-2)
      break;
    case ENVELOPES:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 6; // Wrap to Back button (0-6)
      break;
    case FILTER:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 3; // Wrap to Back button (0-3)
      break;
    case LFO:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 4; // Wrap to Back button (0-4)
      break;
    case VOICE_MODE:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2; // Wrap to Back button (0-2)
      break;
    default:
      // In a parameter menu, no navigation
      break;
  }
}

void navigateMenuBackward() {
  switch(currentMenuState) {
    case PARENT_MENU:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 7; // Wrap to last item (LFO)
      break;
    case OSC_1:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 2; // Wrap to last OSC1 item
      break;
    case OSC_2:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 3; // Wrap to last OSC2 item
      break;
    case OSC_3:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 3; // Wrap to last OSC3 item
      break;
    case NOISE:
      menuIndex = 0; // Only one item, stay at 0
      break;
    case ENVELOPES:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 5; // Wrap to last envelope item
      break;
    case FILTER:
      menuIndex--;
      if (menuIndex < 0) menuIndex = 1; // Wrap to last filter item
      break;
    default:
      // In a parameter menu, can't navigate backward with encoder
      break;
  }
}

void backMenuAction() {
  switch(currentMenuState) {
    case PARENT_MENU:
      // Already at top level
      break;
    case OSC_1:
    case OSC_2:
    case OSC_3:
    case NOISE:
    case ENVELOPES:
    case FILTER:
    case LFO:
    case VOICE_MODE:
      currentMenuState = PARENT_MENU;
      break;
    case OSC1_RANGE:
    case OSC1_WAVE:
    case OSC1_VOLUME:
    case OSC1_FINE:
      currentMenuState = OSC_1;
      break;
    case OSC2_RANGE:
    case OSC2_WAVE:
    case OSC2_VOLUME:
    case OSC2_FINE:
      currentMenuState = OSC_2;
      break;
    case OSC3_RANGE:
    case OSC3_WAVE:
    case OSC3_VOLUME:
    case OSC3_FINE:
      currentMenuState = OSC_3;
      break;
    case NOISE_VOLUME:
      currentMenuState = NOISE;
      break;
    case AMP_ATTACK:
    case AMP_SUSTAIN:
    case AMP_DECAY:
    case FILTER_ATTACK:
    case FILTER_DECAY:
    case FILTER_SUSTAIN:
      currentMenuState = ENVELOPES;
      break;
    case CUTOFF:
    case RESONANCE:
    case FILTER_STRENGTH:
      currentMenuState = FILTER;
      break;
    case LFO_RATE:
    case LFO_DEPTH:
    case LFO_TOGGLE:
    case LFO_TARGET:
      currentMenuState = LFO;
      break;
    case PLAY_MODE:
    case GLIDE_TIME:
    case NOISE_TYPE:
      currentMenuState = VOICE_MODE;
      break;
  }
}


void handleEncoder() {
  // Use the menuEncoder library object for smooth operation
  long newMenuValue = menuEncoder.read() / 2; // More sensitive for better menu navigation
  static long oldMenuValue = 0;
  
  
  if (newMenuValue != oldMenuValue) {
    if (inMenu) {
      if (inPresetBrowse) {
        // In preset browse mode
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
        // On a parameter - adjust value directly
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
          } else {
            // All continuous parameters - consistent 128-step feel across all controls
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
          } else {
            // All continuous parameters - consistent 128-step feel across all controls
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
      // Not in menu - handle cutoff control (parameter 11) like other encoders
      // Just read menuEncoder like a regular encoder for cutoff
      encoderValues[11] = menuEncoder.read() / 2; // More sensitive for cutoff control
    }
    oldMenuValue = newMenuValue;
  }
  
  // Simple single-click button handling
  static bool lastButtonState = HIGH;
  
  bool currentButtonState = digitalRead(MENU_ENCODER_SW);
  
  // Button press detection (on press, not release) 
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (!inMenu) {
      // Enter menu
      inMenu = true;
      currentMenuState = PARENT_MENU;
      menuIndex = 0;
      inPresetBrowse = false;
      printCurrentPresetValues();
      updateDisplay();
    } else if (inPresetBrowse) {
      // In preset browse mode
      if (presetBrowseIndex == NUM_PRESETS) {
        // "Back" option selected
        inPresetBrowse = false;
      } else {
        // Load selected preset and STAY in presets menu
        loadPreset(presetBrowseIndex);
      }
      updateDisplay();
    } else {
      // Navigate into sub-menu or back from parameter
      if (getParameterIndex(currentMenuState) >= 0) {
        // On a parameter - go back to parent submenu (consistent behavior)
        backMenuAction();
      } else {
        // Navigate deeper into menu
        navigateMenuForward();
      }
      updateDisplay();
    }
  }
  
  lastButtonState = currentButtonState;
}

void loadPreset(int presetIndex) {
  if (presetIndex >= 0 && presetIndex < NUM_PRESETS) {
    currentPreset = presetIndex;
    
    Serial.print("Loading preset: ");
    Serial.println(presets[presetIndex].name);
    
    // Debug: Show what we're loading vs what we get
    Serial.println("Preset data -> Loaded value -> Actual Range:");
    for (int i = 0; i < 3; i++) { // Just show first 3 (ranges) for debugging
      float presetValue = presets[presetIndex].parameters[i];
      allParameterValues[i] = presetValue;
      updateSynthParameter(i, allParameterValues[i]);
      
      float actualRange = (i == 0) ? osc1Range : (i == 1) ? osc2Range : osc3Range;
      
      Serial.print("Osc");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(presetValue, 3);
      Serial.print(" -> ");
      Serial.print(allParameterValues[i], 3);
      Serial.print(" -> ");
      Serial.println(actualRange, 3);
    }
    
    // Load remaining parameters without debug spam (now includes LFO, Play Mode and Glide parameters)
    for (int i = 3; i < 28; i++) {
      allParameterValues[i] = presets[presetIndex].parameters[i];
      updateSynthParameter(i, allParameterValues[i]);
    }
    
    Serial.println("Preset loaded.");
    
    // Update display
    updateDisplay();
  }
}

void resetEncoderBaselines() {
  // Reset all encoder positions to match current parameter values
  // This syncs the encoder counting with the actual parameter state
  Serial.println("Resetting encoder baselines to current parameter values...");
  
  for (int i = 0; i < 20; i++) {
    // Calculate what the encoder value should be based on current parameter
    long targetEncoderValue = (long)(allParameterValues[i] * 100);
    
    // Reset the encoder objects to this baseline value
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
      case 12: enc13.write(targetEncoderValue * 4); break;
      case 13: enc14.write(targetEncoderValue * 4); break;
      case 14: enc15.write(targetEncoderValue * 4); break;
      case 15: enc16.write(targetEncoderValue * 4); break;
      case 16: enc17.write(targetEncoderValue * 4); break;
      case 17: enc18.write(targetEncoderValue * 4); break;
      case 18: enc19.write(targetEncoderValue * 4); break;
      case 19: enc20.write(targetEncoderValue * 4); break;
    }
    
    // Update our tracking arrays
    encoderValues[i] = targetEncoderValue;
    lastEncoderValues[i] = targetEncoderValue;
  }
  
  Serial.println("Encoder baselines reset. Physical knob positions now match parameter values.");
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
  for (int i = 0; i < 28; i++) {
    Serial.print(allParameterValues[i], 3); // 3 decimal places
    if (i < 27) Serial.print(", ");
  }
  Serial.println("}");
  Serial.println("Copy this line into your preset array!");
  
  // Show key parameters with names
  Serial.println("\nKey Parameters:");
  Serial.print("Osc1 Range: "); Serial.print(allParameterValues[0], 3);
  Serial.print(" | Osc2 Range: "); Serial.print(allParameterValues[1], 3);
  Serial.print(" | Osc3 Range: "); Serial.println(allParameterValues[2], 3);
  Serial.print("Osc1 Wave: "); Serial.print(allParameterValues[9], 3);
  Serial.print(" | Osc2 Wave: "); Serial.print(allParameterValues[10], 3);
  Serial.print(" | Filter: "); Serial.println(allParameterValues[11], 3);
  
  // Debug: Show first few encoder raw values
  Serial.println("\nEncoder Raw Values:");
  for (int i = 0; i < 5; i++) {
    Serial.print("Enc");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(encoderValues[i]);
    Serial.print(" -> ");
    Serial.println(allParameterValues[i], 3);
  }
  Serial.println("=============================\n");
  
  // Offer to reset encoder baselines
  Serial.println("Type 'r' in Serial Monitor to reset encoder baselines to current values");
}

void updateDisplay() {
  // Throttle display updates to prevent corruption
  static unsigned long lastDisplayUpdate = 0;
  unsigned long now = millis();
  if (now - lastDisplayUpdate < 25) return; // Limit to 40Hz updates
  lastDisplayUpdate = now;
  
  lcd.clear();
  delayMicroseconds(500); // Brief pause after clear to prevent corruption
  if (inMenu) {
    lcd.setCursor(0, 0);
    
    if (inPresetBrowse) {
      // Preset browse mode
      lcd.print("Presets");
      lcd.setCursor(0, 1);
      if (presetBrowseIndex == NUM_PRESETS) {
        lcd.print("< Back");
      } else {
        lcd.print(presetBrowseIndex + 1);
        lcd.print(". ");
        lcd.print(presets[presetBrowseIndex].name);
      }
    } else {
      // Regular menu navigation
      // lcd.print("MENU ");
      
      switch(currentMenuState) {
        case PARENT_MENU:
          lcd.print("Menu");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Presets");
          else if (menuIndex == 1) lcd.print("Oscillator 1");
          else if (menuIndex == 2) lcd.print("Oscillator 2");
          else if (menuIndex == 3) lcd.print("Oscillator 3");
          else if (menuIndex == 4) lcd.print("Noise");
          else if (menuIndex == 5) lcd.print("Envelopes");
          else if (menuIndex == 6) lcd.print("Filter");
          else if (menuIndex == 7) lcd.print("LFO");
          else if (menuIndex == 8) lcd.print("Voice Mode");
          else if (menuIndex == 9) lcd.print("< Exit");
          break;
          
          
        case OSC_1:
          lcd.print("Oscillator 1");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Range");
          else if (menuIndex == 1) lcd.print("Waveform");
          else if (menuIndex == 2) lcd.print("Volume");
          else if (menuIndex == 3) lcd.print("Fine Tune");
          else if (menuIndex == 4) lcd.print("< Back");
          break;
          
        case OSC_2:
          lcd.print("Oscillator 2");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Range");
          else if (menuIndex == 1) lcd.print("Waveform");
          else if (menuIndex == 2) lcd.print("Volume");
          else if (menuIndex == 3) lcd.print("Fine Tune");
          else if (menuIndex == 4) lcd.print("< Back");
          break;
          
        case OSC_3:
          lcd.print("Oscillator 3");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Range");
          else if (menuIndex == 1) lcd.print("Waveform");
          else if (menuIndex == 2) lcd.print("Volume");
          else if (menuIndex == 3) lcd.print("Fine Tune");
          else if (menuIndex == 4) lcd.print("< Back");
          break;
          
        case NOISE:
          lcd.print("Noise");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Volume");
          else if (menuIndex == 1) lcd.print("Type");
          else if (menuIndex == 2) lcd.print("< Back");
          break;
          
        case ENVELOPES:
          lcd.print("Envelopes");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Amp Attack");
          else if (menuIndex == 1) lcd.print("Amp Sustain");
          else if (menuIndex == 2) lcd.print("Amp Decay");
          else if (menuIndex == 3) lcd.print("Filter Attack");
          else if (menuIndex == 4) lcd.print("Filter Decay");
          else if (menuIndex == 5) lcd.print("Filter Sustain");
          else if (menuIndex == 6) lcd.print("< Back");
          break;
          
        case FILTER:
          lcd.print("Filter");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Cutoff");
          else if (menuIndex == 1) lcd.print("Resonance");
          else if (menuIndex == 2) lcd.print("Strength");
          else if (menuIndex == 3) lcd.print("< Back");
          break;
          
        case LFO:
          lcd.print("LFO");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Rate");
          else if (menuIndex == 1) lcd.print("Depth");
          else if (menuIndex == 2) lcd.print("Toggle");
          else if (menuIndex == 3) lcd.print("Target");
          else if (menuIndex == 4) lcd.print("< Back");
          break;
          
        case VOICE_MODE:
          lcd.print("Voice Mode");
          lcd.setCursor(0, 1);
          if (menuIndex == 0) lcd.print("Play Mode");
          else if (menuIndex == 1) lcd.print("Glide Time");
          else if (menuIndex == 2) lcd.print("< Back");
          break;
          
        default:
          // Parameter editing
          int paramIndex = getParameterIndex(currentMenuState);
          if (paramIndex >= 0) {
            lcd.print(controlNames[paramIndex]);
            lcd.setCursor(0, 1);
            // Always show current value (no "Click to edit" screen)
            if (paramIndex == 3 || paramIndex == 4 || paramIndex == 20) { // Extended fine tuning
              float val = allParameterValues[paramIndex];
              if (val <= 0.25) {
                // Semitone range (negative)
                float semiRange = val / 0.25;
                int semitones = (int)(-12 + (semiRange * 11)); // -12 to -1
                lcd.print(semitones);
                lcd.print("st");
              } else if (val >= 0.75) {
                // Semitone range (positive)
                float semiRange = (val - 0.75) / 0.25;
                int semitones = (int)(1 + (semiRange * 11)); // +1 to +12
                lcd.print("+");
                lcd.print(semitones);
                lcd.print("st");
              } else {
                // Cents range (±25 cents)
                int cents = (int)((val - 0.5) * 100); // -25 to +25 cents
                if (cents >= 0) lcd.print("+");
                lcd.print(cents);
                lcd.print("c");
              }
            } else if (paramIndex == 22) { // LFO Rate
              float rate = 0.1 + allParameterValues[paramIndex] * 19.9;
              lcd.print(rate, 1);
              lcd.print(" Hz");
            } else if (paramIndex == 23) { // LFO Depth
              int depth = (int)(allParameterValues[paramIndex] * 100);
              lcd.print(depth);
              lcd.print("%");
            } else if (paramIndex == 24) { // LFO Toggle
              lcd.print(lfoEnabled ? "ON" : "OFF"); // Use actual variable instead of parameter
            } else if (paramIndex == 25) { // LFO Target
              if (lfoTarget == 0) lcd.print("Pitch");
              else if (lfoTarget == 1) lcd.print("Filter");
              else lcd.print("Amp");
            } else if (paramIndex == 26) { // Play Mode
              if (playMode == 0) lcd.print("Mono");
              else if (playMode == 1) lcd.print("Poly");
              else lcd.print("Legato");
            } else if (paramIndex == 27) { // Glide Time
              if (glideTime == 0.0) {
                lcd.print("OFF");
              } else {
                float timeMs = 50 + (glideTime * 950); // 50ms to 1000ms
                lcd.print((int)timeMs);
                lcd.print("ms");
              }
            } else if (paramIndex == 28) { // Noise Type
              if (noiseType == 0) {
                lcd.print("White");
              } else {
                lcd.print("Pink");
              }
            } else {
              // Show 0-127 values (MIDI standard)
              int displayValue = (int)(allParameterValues[paramIndex] * 127);
              lcd.print(displayValue);
            }
          }
          break;
      }
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("MiniTeensy");
    lcd.setCursor(0, 1);
    
    // Count active voices
    int activeVoices = 0;
    for (int v = 0; v < VOICES; v++) {
      if (voices[v].active) activeVoices++;
    }
    
    if (activeVoices > 0) {
      lcd.print("Voices: ");
      lcd.print(activeVoices);
    } else {
      lcd.print("Press for menu");
    }
  }
}

void loop() {
  // Process ALL USB Device MIDI messages immediately for minimal latency
  while (usbMIDI.read()) {
    uint8_t type = usbMIDI.getType();
    uint8_t data1 = usbMIDI.getData1();
    uint8_t data2 = usbMIDI.getData2();
    
    if (type == usbMIDI.NoteOn && data2 > 0) {
      noteOn(data1, data2);
    } else if (type == usbMIDI.NoteOff || (type == usbMIDI.NoteOn && data2 == 0)) {
      noteOff(data1);
    } else if (type == usbMIDI.ControlChange) {
      // Handle MIDI Control Change messages
      if (data1 == 1) { // Mod wheel (CC#1)
        modWheelValue = data2 / 127.0; // Convert to 0.0-1.0 range
        // No serial output for performance
      }
    } else if (type == usbMIDI.PitchBend) {
      // Handle pitch wheel (14-bit value)
      int pitchBendValue = (data2 << 7) | data1; // Combine MSB and LSB
      pitchWheelValue = (pitchBendValue - 8192) / 8192.0; // Convert to -1.0 to +1.0 range
      // No serial output for performance
    }
  }
  
  // Read controls and update synth
  readAllControls();
  
  // Handle encoder
  handleEncoder();
  
  // Update LFO filter modulation
  updateLFOModulation();
  
  // Update glide/portamento
  updateGlide();
  
  // Minimal serial input check for performance
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'r' || input == 'R') {
      resetEncoderBaselines();
    }
  }
  
  delay(5); // Reduced delay for better responsiveness
}