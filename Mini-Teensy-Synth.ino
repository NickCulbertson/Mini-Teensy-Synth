/*
 * MiniTeensy Synth v1.1
 * A 6-voice polyphonic virtual analog synthesizer built with the Teensy 4.1 microcontroller, 
 * inspired by the classic Minimoog. Features comprehensive synthesis with USB audio/MIDI 
 * and intuitive menu control.
 */

#define NUM_PARAMETERS 31
#define NUM_PRESETS 20
#define VOICES 6

#include "config.h"
#include "MenuNavigation.h"

const char* PROJECT_NAME = "MiniTeensy Synth";
const char* PROJECT_SUBTITLE = "6-Voice Poly";

#include <USBHost_t36.h>
#include <Audio.h>
#include <Wire.h>
#include <Encoder.h>


#ifdef USE_MIDI_HOST
USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi1(myusb);
#endif

#ifdef USE_DIN_MIDI
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#endif

#ifdef USE_LCD_DISPLAY
  #include <LiquidCrystal_I2C.h>
#endif

#ifdef USE_OLED_DISPLAY
  #include <U8g2lib.h>
  #define OLED_WIDTH 128
  #define OLED_HEIGHT 32
  #define OLED_RESET -1
#endif


// All 19 Mini-Teensy Encoders (using configurable pin definitions from config.h)
Encoder enc1(ENC_1_CLK, ENC_1_DT);
Encoder enc2(ENC_2_CLK, ENC_2_DT);
Encoder enc3(ENC_3_CLK, ENC_3_DT);
Encoder enc4(ENC_4_CLK, ENC_4_DT);
Encoder enc5(ENC_5_CLK, ENC_5_DT);
Encoder enc6(ENC_6_CLK, ENC_6_DT);
Encoder enc7(ENC_7_CLK, ENC_7_DT);
Encoder enc8(ENC_8_CLK, ENC_8_DT);
Encoder enc9(ENC_9_CLK, ENC_9_DT);
Encoder enc10(ENC_10_CLK, ENC_10_DT);
Encoder enc11(ENC_11_CLK, ENC_11_DT);
Encoder enc13(ENC_13_CLK, ENC_13_DT);
Encoder enc14(ENC_14_CLK, ENC_14_DT);
Encoder enc15(ENC_15_CLK, ENC_15_DT);
Encoder enc16(ENC_16_CLK, ENC_16_DT);
Encoder enc17(ENC_17_CLK, ENC_17_DT);
Encoder enc18(ENC_18_CLK, ENC_18_DT);
Encoder enc19(ENC_19_CLK, ENC_19_DT);
Encoder enc20(ENC_20_CLK, ENC_20_DT);
Encoder menuEncoder(MENU_ENCODER_DT, MENU_ENCODER_CLK);

// Configurable encoder to parameter mapping (defined in config.h)
// Array indices: 0=enc1, 1=enc2, ..., 10=enc11, 11=menuEncoder, 12=enc13, 13=enc14, ..., 19=enc20
const int encoderMapping[20] = {
  ENC_1_PARAM, ENC_2_PARAM, ENC_3_PARAM, ENC_4_PARAM, ENC_5_PARAM,              // 0-4: enc1-enc5
  ENC_6_PARAM, ENC_7_PARAM, ENC_8_PARAM, ENC_9_PARAM, ENC_10_PARAM,             // 5-9: enc6-enc10
  ENC_11_PARAM, MENU_ENCODER_PARAM, ENC_13_PARAM, ENC_14_PARAM, ENC_15_PARAM,   // 10-14: enc11,menuEncoder,enc13-enc15
  ENC_16_PARAM, ENC_17_PARAM, ENC_18_PARAM, ENC_19_PARAM, ENC_20_PARAM          // 15-19: enc16-enc20
};

long encoderValues[20] = {0};
long lastEncoderValues[20] = {0};
// Default parameter values - matches "Init" preset
float allParameterValues[NUM_PARAMETERS] = {
  0.417, 0.417, 0.417, 0.500, 0.500, 0.417, 0.417, 0.417, 0.789, 0.789,
  0.789, 1.000, 0.000, 0.000, 0.160, 1.000, 0.000, 0.000, 1.000, 0.016,
  0.500, 1.000, 0.250, 0.000, 0.000, 0.330, 0.330, 0.000, 0.000, 0.000,
  0.000  // MIDI Channel (0 = omni, 1-16 = channels)
};

// Audio synthesis
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

#ifdef USE_USB_AUDIO
AudioOutputUSB           usb1;            // USB audio output (stereo)
#endif

#ifdef USE_TEENSY_DAC
AudioOutputI2S           i2s1;            // I2S DAC output (Teensy Audio Shield)
AudioControlSGTL5000     sgtl5000_1;
#endif

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

// Final output connections
#ifdef USE_USB_AUDIO
AudioConnection patchCordOut1(finalMix, 0, usb1, 0); // Left channel
AudioConnection patchCordOut2(finalMix, 0, usb1, 1); // Right channel
#endif

#ifdef USE_TEENSY_DAC
AudioConnection patchCordOut3(finalMix, 0, i2s1, 0); // Left channel
AudioConnection patchCordOut4(finalMix, 0, i2s1, 1); // Right channel
#endif

// AudioControlSGTL5000     sgt15000_1;

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
int midiChannel = 0; // MIDI channel (1-16, 0 = omni)
unsigned long lastMidiTime = 0; // For MIDI throttling
int playMode = 1; // 0=Mono, 1=Poly, 2=Legato

// Display update tracking for MIDI CC changes
int lastChangedParam = -1;
float lastChangedValue = 0.0;
String lastChangedName = "";
bool parameterChanged = false;
float glideTime = 0.0; // Glide/portamento time (0 = off, 0.1-1.0 = 100ms to 10s)
int noiseType = 0; // 0 = White, 1 = Pink
float targetFreq[VOICES]; // Target frequencies for glide
float currentFreq[VOICES]; // Current frequencies during glide
bool gliding[VOICES]; // Whether each voice is gliding

// Control system
#ifdef USE_LCD_DISPLAY
  LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

#ifdef USE_OLED_DISPLAY
  U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#endif

int menuIndex = 0;
bool inMenu = false;

// Control names
const char* controlNames[] = {
  "Osc1 Range", "Osc2 Range", "Osc3 Range", "Osc2 Fine", "Osc3 Fine",
  "Osc1 Wave", "Osc2 Wave", "Osc3 Wave", "Volume 1", "Volume 2", 
  "Volume 3", "Cutoff", "Resonance", "Filt Attack", "Filt Decay",
  "Filt Sustain", "Noise Vol", "Amp Attack", "Amp Sustain", "Amp Decay",
  "Osc1 Fine", "Filt Strength", "LFO Rate", "LFO Depth", "LFO Toggle", "LFO Target", "Play Mode", "Glide Time", "Noise Type", "Macro Mode", "MIDI Channel"
};

bool macroMode = false;

extern const MiniTeensyPreset presets[];
int currentPreset = 0;

MenuState currentMenuState = PARENT_MENU;
bool inPresetBrowse = false; // When browsing individual presets
int presetBrowseIndex = 0; // Which preset we're browsing

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

#ifdef ENABLE_DIN_MIDI
// DIN MIDI callback handlers
void OnNoteOn(byte channel, byte note, byte velocity) {
  if (midiChannel != 0 && channel != midiChannel) return;
  noteOn(note, velocity);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  if (midiChannel != 0 && channel != midiChannel) return;
  noteOff(note);
}

void OnControlChange(byte channel, byte number, byte value) {
  if (midiChannel != 0 && channel != midiChannel) return;
  if (number == 1) modWheelValue = value / 127.0;
}

void OnPitchBend(byte channel, int bend) {
  if (midiChannel != 0 && channel != midiChannel) return;
  pitchWheelValue = (bend - 8192) / 8192.0;
}
#endif

#ifdef USE_MIDI_HOST
// USB Host MIDI callback handlers
void OnUSBHostNoteOn(byte channel, byte note, byte velocity) {
  if (midiChannel != 0 && channel != midiChannel) return;
  noteOn(note, velocity);
}
void OnUSBHostNoteOff(byte channel, byte note, byte velocity) {
  if (midiChannel != 0 && channel != midiChannel) return;
  noteOff(note);
}
void OnUSBHostControlChange(byte channel, byte number, byte value) {
  if (midiChannel != 0 && channel != midiChannel) return;
  if (number == 1) modWheelValue = value / 127.0;
}
void OnUSBHostPitchBend(byte channel, int bend) {
  if (midiChannel != 0 && channel != midiChannel) return;
  // USB Host MIDI uses signed range -8192 to +8191, center = 0
  pitchWheelValue = bend / 8192.0;
}
#endif


void processMidiMessage(byte type, byte channel, byte data1, byte data2) {
  // Filter by MIDI channel (0 = omni, 1-16 = specific channel)
  if (midiChannel != 0 && channel != midiChannel) return;
  
  switch (type) {
    case 0x90: // Note On (or Note Off with velocity 0)
      if (data2 > 0) {
        noteOn(data1, data2);
      } else {
        noteOff(data1);
      }
      break;
      
    case 0x80: // Note Off
      noteOff(data1);
      break;
      
    case 0xB0: // Control Change
      handleControlChange(data1, data2);
      break;
      
    case 0xC0: // Program Change
      handleProgramChange(data1);
      break;
      
    case 0xE0: // Pitch Bend
      {
        int pitchBendValue = (data2 << 7) | data1; // Combine MSB and LSB
        pitchWheelValue = (pitchBendValue - 8192) / 8192.0;
      }
      break;
  }
}

void handleControlChange(int cc, int value) {
  // Convert MIDI value (0-127) to parameter value (0.0-1.0)
  float paramValue = value / 127.0;
  
  // Handle standard MIDI CCs first
  if (cc == CC_MODWHEEL) {
    modWheelValue = paramValue;
    
    // Track mod wheel change for display
    lastChangedParam = -1;  // Special flag for non-parameter controls
    lastChangedName = "Mod Wheel";
    lastChangedValue = value;  // Use raw 0-127 value for display
    parameterChanged = true;
    return;  // Don't process as parameter - mod wheel has its own function
  }
  
  // Handle Mini synthesizer parameter CCs
  int paramIndex = -1;
  
  // if (cc == CC_OSC1_RANGE) paramIndex = 0;
  // else if (cc == CC_OSC2_RANGE) paramIndex = 1;
  // else if (cc == CC_OSC3_RANGE) paramIndex = 2;
  // else if (cc == CC_OSC2_FINE) paramIndex = 3;
  // else if (cc == CC_OSC3_FINE) paramIndex = 4;
  // else if (cc == CC_OSC1_WAVE) paramIndex = 5;
  // else if (cc == CC_OSC2_WAVE) paramIndex = 6;
  // else if (cc == CC_OSC3_WAVE) paramIndex = 7;
  // else if (cc == CC_OSC1_VOL) paramIndex = 8;
  // else if (cc == CC_OSC2_VOL) paramIndex = 9;
  // else if (cc == CC_OSC3_VOL) paramIndex = 10;
  // else if (cc == CC_CUTOFF) paramIndex = 11;
  // else if (cc == CC_RESONANCE) paramIndex = 12;
  // else if (cc == CC_FILTER_ATTACK) paramIndex = 13;
  // else if (cc == CC_FILTER_DECAY) paramIndex = 14;
  // else if (cc == CC_FILTER_SUSTAIN) paramIndex = 15;
  // else if (cc == CC_NOISE_VOL) paramIndex = 16;
  // else if (cc == CC_AMP_ATTACK) paramIndex = 17;
  // else if (cc == CC_AMP_SUSTAIN) paramIndex = 18;
  // else if (cc == CC_AMP_DECAY) paramIndex = 19;
  // else if (cc == CC_OSC1_FINE) paramIndex = 20;
  // else if (cc == CC_FILTER_STRENGTH) paramIndex = 21;
  // else if (cc == CC_LFO_RATE) paramIndex = 22;
  // else if (cc == CC_LFO_DEPTH) paramIndex = 23;

  if (cc == CC_1_PARAM) paramIndex = ENC_1_PARAM;   
  else if (cc == CC_2_PARAM) paramIndex = ENC_2_PARAM; 
  else if (cc == CC_3_PARAM) paramIndex = ENC_3_PARAM; 
  else if (cc == CC_4_PARAM) paramIndex = ENC_4_PARAM; 
  else if (cc == CC_5_PARAM) paramIndex = ENC_5_PARAM; 
  else if (cc == CC_6_PARAM) paramIndex = ENC_6_PARAM; 
  else if (cc == CC_7_PARAM) paramIndex = ENC_7_PARAM; 
  else if (cc == CC_8_PARAM) paramIndex = ENC_8_PARAM;
  else if (cc == CC_9_PARAM) paramIndex = ENC_9_PARAM;
  else if (cc == CC_10_PARAM) paramIndex = ENC_10_PARAM;
  else if (cc == CC_11_PARAM) paramIndex = ENC_11_PARAM;
  else if (cc == CC_12_PARAM) paramIndex = MENU_ENCODER_PARAM;
  else if (cc == CC_13_PARAM) paramIndex = ENC_13_PARAM;
  else if (cc == CC_14_PARAM) paramIndex = ENC_14_PARAM;
  else if (cc == CC_15_PARAM) paramIndex = ENC_15_PARAM;
  else if (cc == CC_16_PARAM) paramIndex = ENC_16_PARAM;
  else if (cc == CC_17_PARAM) paramIndex = ENC_17_PARAM;
  else if (cc == CC_18_PARAM) paramIndex = ENC_18_PARAM;
  else if (cc == CC_19_PARAM) paramIndex = ENC_19_PARAM;
  else if (cc == CC_20_PARAM) paramIndex = ENC_20_PARAM;
  else if (cc == CC_21_PARAM) paramIndex = ENC_21_PARAM;
  else if (cc == CC_22_PARAM) paramIndex = ENC_22_PARAM;
  else if (cc == CC_23_PARAM) paramIndex = ENC_23_PARAM; 
  
  // Update parameter if mapped
  if (paramIndex >= 0) {
    allParameterValues[paramIndex] = paramValue;
    updateParameterFromMenu(paramIndex, paramValue);
    
    // Track parameter change for display
    lastChangedParam = paramIndex;
    lastChangedValue = paramValue;
    lastChangedName = controlNames[paramIndex];
    parameterChanged = true;
  }
}

void handleProgramChange(int program) {
  if (program >= 0 && program < NUM_PRESETS) {
    loadPreset(program);
    Serial.print("Program change to preset: ");
    Serial.println(program);
    
    // Update display to show preset name
    String line1 = "Preset " + String(program + 1);
    String line2 = String(getPresetName(program));
    displayText(line1, line2);
  }
}

void updateParameterDisplay(int paramIndex, float val) {
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
  else if (paramIndex == 3 || paramIndex == 4 || paramIndex == 20) { // Fine tuning controls
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
  }
  else if (paramIndex == 22) { // LFO Rate
    float hz = 0.1 + (val * 19.9); // 0.1 to 20 Hz
    line2 = String(hz, 1) + " Hz";
  }
  else {
    int displayValue = (int)(val * 127); // 0-127 scale
    line2 = String(displayValue);
  }
  
  displayText(line1, line2);
}

void setup() {
  Serial.begin(115200);
  AudioMemory(48);
  
#ifdef USE_TEENSY_DAC
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);
  Serial.println("Teensy Audio Shield initialized");
#endif

#ifdef USE_MIDI_HOST
  myusb.begin();
  Serial.println("USB Host MIDI initialized");
#endif

#ifdef USE_DIN_MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    processMidiMessage(0x90, channel, note, velocity);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    processMidiMessage(0x80, channel, note, velocity);
  });
  MIDI.setHandleControlChange([](byte channel, byte cc, byte value) {
    processMidiMessage(0xB0, channel, cc, value);
  });
  MIDI.setHandleProgramChange([](byte channel, byte program) {
    processMidiMessage(0xC0, channel, program, 0);
  });
  MIDI.setHandlePitchBend([](byte channel, int bend) {
    byte data1 = bend & 0x7F;       // LSB
    byte data2 = (bend >> 7) & 0x7F; // MSB
    processMidiMessage(0xE0, channel, data1, data2);
  });
  Serial.println("DIN MIDI initialized");
#endif

  
  // Initialize LFO
  lfo.frequency(lfoRate);
  lfo.amplitude(1.0);
  
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
  
  delay(100);
  
#ifdef USE_LCD_DISPLAY
  lcd.init();
  lcd.backlight();
  displayText(PROJECT_NAME, PROJECT_SUBTITLE);
#endif

#ifdef USE_OLED_DISPLAY
  Wire.begin();
  delay(50);
  display.begin();
  Serial.println("OLED initialized");
  
  display.clearBuffer();
  display.setFont(u8g2_font_8x13_tf);
  display.drawStr(3, 20, PROJECT_NAME);
  display.drawStr(3, 40, PROJECT_SUBTITLE);
  display.sendBuffer();
#endif
  
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
  
  noise1.amplitude(0.5); // Reduced noise amplitude
  noisePink.amplitude(0.5); // Pink noise amplitude
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
    
// sgt15000_1.enable();
//     sgt15000_1.volume(1);

  Serial.println("Mini-Teensy Synth initialized");
  Serial.println("Parameters: 31 synthesizer parameters");
  
#ifdef USE_USB_AUDIO
  Serial.println("Audio output: USB Audio");
#endif
#ifdef USE_TEENSY_DAC
  Serial.println("Audio output: Teensy Audio Shield (I2S)");
#endif

  delay(2000);
  updateDisplay();
  Serial.print(PROJECT_NAME);
  Serial.println(" Ready!");
}


// Read all 20 Mini-Teensy encoders (excluding menu encoder - handled in MenuNavigation.cpp)
void readDirectEncoders() {
  encoderValues[0] = enc1.read() / 4;   // enc1
  encoderValues[1] = enc2.read() / 4;   // enc2
  encoderValues[2] = enc3.read() / 4;   // enc3
  encoderValues[3] = enc4.read() / 4;   // enc4
  encoderValues[4] = enc5.read() / 4;   // enc5
  encoderValues[5] = enc6.read() / 4;   // enc6
  encoderValues[6] = enc7.read() / 4;   // enc7
  encoderValues[7] = enc8.read() / 4;   // enc8
  encoderValues[8] = enc9.read() / 4;   // enc9
  encoderValues[9] = enc10.read() / 4;  // enc10
  encoderValues[10] = enc11.read() / 4; // enc11
  // encoderValues[11] handled separately for cutoff (uses menuEncoder hardware) in MenuNavigation.cpp
  encoderValues[12] = enc13.read() / 4; // enc13 (note: no enc12)
  encoderValues[13] = enc14.read() / 4; // enc14
  encoderValues[14] = enc15.read() / 4; // enc15
  encoderValues[15] = enc16.read() / 4; // enc16
  encoderValues[16] = enc17.read() / 4; // enc17
  encoderValues[17] = enc18.read() / 4; // enc18
  encoderValues[18] = enc19.read() / 4; // enc19
  encoderValues[19] = enc20.read() / 4; // enc20
}


void readAllControls() {
  // Read all encoders
  readDirectEncoders();
  
  // Check for encoder changes and update parameters using configurable mapping
  for (int i = 0; i < 20; i++) {
    if (encoderValues[i] != lastEncoderValues[i]) {
      // If any physical knob is turned, exit menu mode
      if (inMenu) {
        inMenu = false;
      }
      
      int change = encoderValues[i] - lastEncoderValues[i];
      int paramIndex = encoderMapping[i]; // Use configurable mapping
      
      // Only update if encoder is mapped to a valid parameter (not -1)
      if (paramIndex >= 0 && paramIndex < NUM_PARAMETERS) {
        // Handle macro mode for mapped parameters
        int targetParam = paramIndex;
        if (macroMode && paramIndex == 13) targetParam = 22;  // Filter Attack -> LFO Rate  
        if (macroMode && paramIndex == 14) targetParam = 23;  // Filter Decay -> LFO Depth
        if (macroMode && paramIndex == 15) targetParam = 25;  // Filter Sustain -> LFO Target
        
        updateEncoderParameter(targetParam, change);
      }
      
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
      for (int v = 0; v < VOICES; v++) osc1[v].begin(osc1Wave);
      break;
    case 6: // Osc2 Wave
      osc2Wave = getMiniTeensyWaveform(val, 2);
      for (int v = 0; v < VOICES; v++) osc2[v].begin(osc2Wave);
      break;
    case 7: // Osc3 Wave
      osc3Wave = getMiniTeensyWaveform(val, 3);
      for (int v = 0; v < VOICES; v++) osc3[v].begin(osc3Wave);
      break;
    case 8: // Volume 1
      vol1 = val * 0.8; // Increased gain from 0.4 to 0.8
      for (int v = 0; v < VOICES; v++) oscMix[v].gain(0, vol1);
      break;
    case 9: // Volume 2
      vol2 = val * 0.8; // Increased gain from 0.4 to 0.8
      for (int v = 0; v < VOICES; v++) oscMix[v].gain(1, vol2);
      break;
    case 10: // Volume 3
      vol3 = val * 0.8; // Increased gain from 0.4 to 0.8
      for (int v = 0; v < VOICES; v++) oscMix[v].gain(2, vol3);
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
      for (int v = 0; v < VOICES; v++) oscMix[v].gain(3, noiseVol);
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
    case 29: // Macro Mode (menu-only)
      macroMode = (val > 0.5); // Toggle at 50%
      break;
    case 30: // MIDI Channel
      midiChannel = (int)(val * 16.0); // 0-16 (0 = omni, 1-16 = channels)
      break;
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

void updateAllVoiceParameters() {
  // Update all parameters for all voices in one loop (for initialization)
  for (int v = 0; v < VOICES; v++) {
    osc1[v].begin(osc1Wave);
    osc2[v].begin(osc2Wave);
    osc3[v].begin(osc3Wave);
    oscMix[v].gain(0, vol1);
    oscMix[v].gain(1, vol2);
    oscMix[v].gain(2, vol3);
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

void loop() {
  // Handle USB Device MIDI
#ifdef USE_USB_DEVICE_MIDI
  while (usbMIDI.read()) {
    processMidiMessage(usbMIDI.getType(), usbMIDI.getChannel(), 
                      usbMIDI.getData1(), usbMIDI.getData2());
  }
#endif

  // Handle USB Host MIDI
#ifdef USE_MIDI_HOST
  myusb.Task();
  while (midi1.read()) {
    processMidiMessage(midi1.getType(), midi1.getChannel(), 
                      midi1.getData1(), midi1.getData2());
  }
#endif

  // Handle DIN MIDI
#ifdef USE_DIN_MIDI
  MIDI.read();
#endif
  
  readAllControls();
  handleEncoder();
  updateLFOModulation();
  updateGlide();
  
  // Update display if parameter changed during this loop iteration
  if (parameterChanged) {
    // If we were in menu mode, exit menu to show MIDI parameter
    if (inMenu) {
      inMenu = false;
    }
    
    if (lastChangedParam >= 0) {
      // Use the specialized display function for Mini-Teensy
      updateParameterDisplay(lastChangedParam, lastChangedValue);
    } else {
      // For mod wheel and other non-parameter controls
      String line2 = String((int)lastChangedValue);
      displayText(lastChangedName, line2);
    }
    parameterChanged = false;
  }
  
  delay(5); // Reduced delay for better responsiveness
}