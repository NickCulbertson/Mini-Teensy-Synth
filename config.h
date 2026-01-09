#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Share Configs
// ============================================================================
// This is a shared config master file for multiple synths. Just ignore the ones you are not using.
// Use the export script, or copy this file to each project as config.h and uncomment the appropriate
// PROJECT_TYPE define below. This ensures consistent configuration across
// all synthesizers while allowing project-specific customization.

// ============================================================================
// PROJECT TYPE SELECTION - Uncomment ONE of these in each project
// ============================================================================
// #define PROJECT_EPIANO
// #define PROJECT_DCO  
// #define PROJECT_FM
#define PROJECT_MINI
// #define PROJECT_MACRO

// ============================================================================
// Preferences - Multi-Teensy Synth Collection
// ============================================================================

// • DISPLAY TYPE
// Display Configuration (Choose ONE - comment out the other)
#define USE_LCD_DISPLAY
// #define USE_OLED_DISPLAY

// • MIDI TYPE
#define USE_USB_DEVICE_MIDI // USB Device MIDI for DAW/computer connection (default)
#define USE_MIDI_HOST       // USB Host MIDI for external controllers connected to Teensy 
// #define USE_DIN_MIDI // DIN MIDI support - UNTESTED (requires moving enc3 from pin 0) 

// • AUDIO TYPE
// #define USE_TEENSY_DAC        // Use Teensy Audio Shield or other I2S DAC
#define USE_USB_AUDIO      // Use USB Audio output (default)

// ============================================================================
// Hardware Configuration - Multi-Teensy Standard Layout
// ============================================================================

// Menu Encoder Pin Assignments
#define MENU_ENCODER_CLK 14
#define MENU_ENCODER_DT 15  
#define MENU_ENCODER_SW 13

// ============================================================================
// Encoder Pin Definitions (Mini-Teensy Standard Layout)
// ============================================================================

#define ENC_1_CLK    4      // enc1 pins
#define ENC_1_DT     5
#define ENC_2_CLK    2      // enc2 pins  
#define ENC_2_DT     3
#define ENC_3_CLK    0      // enc3 pins
#define ENC_3_DT     1
#define ENC_4_CLK    8      // enc4 pins
#define ENC_4_DT     9
#define ENC_5_CLK    6      // enc5 pins
#define ENC_5_DT     7
#define ENC_6_CLK    25     // enc6 pins
#define ENC_6_DT     27
#define ENC_7_CLK    12     // enc7 pins
#define ENC_7_DT     24
#define ENC_8_CLK    10     // enc8 pins
#define ENC_8_DT     11
#define ENC_9_CLK    29     // enc9 pins
#define ENC_9_DT     30
#define ENC_10_CLK   28     // enc10 pins
#define ENC_10_DT    26
#define ENC_11_CLK   21     // enc11 pins
#define ENC_11_DT    20
//Encoder 12 is the menu encoder
#define ENC_13_CLK   34     // enc13 pins
#define ENC_13_DT    33
#define ENC_14_CLK   50     // enc14 pins
#define ENC_14_DT    41
#define ENC_15_CLK   23     // enc15 pins
#define ENC_15_DT    22
#define ENC_16_CLK   36     // enc16 pins
#define ENC_16_DT    35
#define ENC_17_CLK   31     // enc17 pins
#define ENC_17_DT    32
#define ENC_18_CLK   17     // enc18 pins
#define ENC_18_DT    16
#define ENC_19_CLK   38     // enc19 pins
#define ENC_19_DT    37
#define ENC_20_CLK   40     // enc20 pins
#define ENC_20_DT    39


// Standard MIDI CCs (shared across all projects)
#define CC_MODWHEEL      1    // Standard mod wheel
#define CC_VOLUME        7    // Standard volume control
#define CC_SUSTAIN       64   // Standard sustain pedal

// ============================================================================
// PROJECT-SPECIFIC CONFIGURATIONS
// ============================================================================

#ifdef PROJECT_EPIANO
// ============================================================================
// EPiano-Teensy-Synth Configuration
// ============================================================================

// EPiano MIDI CC Parameter Mapping
#define CC_1_PARAM       73
#define CC_2_PARAM       75
#define CC_3_PARAM       79
#define CC_4_PARAM       72
#define CC_5_PARAM       80
#define CC_6_PARAM       81
#define CC_7_PARAM       82
#define CC_8_PARAM       83
#define CC_9_PARAM       74
#define CC_10_PARAM      71
#define CC_11_PARAM      76
#define CC_12_PARAM      93
#define CC_13_PARAM      77
#define CC_14_PARAM      93
#define CC_15_PARAM      18
#define CC_16_PARAM      19
#define CC_17_PARAM      16
#define CC_18_PARAM      17
#define CC_19_PARAM      85
#define CC_20_PARAM      86

// EPiano Encoder Mapping
#define ENC_1_PARAM    0   // Decay
#define ENC_2_PARAM    1   // Release
#define ENC_3_PARAM    2   // Hardness
#define ENC_4_PARAM    3   // Treble
#define ENC_5_PARAM    4   // Pan/Tremolo
#define ENC_6_PARAM    5   // LFO Rate
#define ENC_7_PARAM    6   // Velocity
#define ENC_8_PARAM    7   // Stereo
#define ENC_9_PARAM    8   // Polyphony
#define ENC_10_PARAM   9   // Master Tune
#define ENC_11_PARAM   10  // Detune
#define ENC_12_PARAM   -1  // Disabled
#define ENC_13_PARAM   11  // Overdrive
#define ENC_14_PARAM   12  // Volume
#define ENC_15_PARAM   -1  // Disabled
#define ENC_16_PARAM   -1  // Disabled
#define ENC_17_PARAM   -1  // Disabled
#define ENC_18_PARAM   -1  // Disabled
#define ENC_19_PARAM   -1  // Disabled
#define ENC_20_PARAM   -1  // Disabled
#define ENC_21_PARAM   -1  // Disabled
#define ENC_22_PARAM   -1  // Disabled
#define ENC_23_PARAM   -1  // Disabled

//EPiano Parameters (14 total):
// 0: Decay (0.0-1.0)
// 1: Release (0.0-1.0)
// 2: Hardness (0.0-1.0)
// 3: Treble (0.0-1.0)
// 4: Pan/Tremolo (0.0-1.0)
// 5: LFO Rate (0.0-1.0)
// 6: Velocity (0.0-1.0)
// 7: Stereo (0.0-1.0)
// 8: Polyphony (1.0-16.0)
// 9: Master Tune (0.0-1.0)
// 10: Detune (0.0-1.0)
// 11: Overdrive (0.0-1.0) - Internal gain/drive saturation
// 12: Volume (0.0-1.0) - Master output level
// 13: MIDI Channel (0.0-1.0)

// Menu Encoder Configuration
#define MENU_ENCODER_PARAM  -1  // Menu-only mode

#endif // PROJECT_EPIANO

#ifdef PROJECT_DCO
// ============================================================================
// DCO-Teensy-Synth Configuration  
// ============================================================================

// DCO MIDI CC Parameter Mapping
#define CC_1_PARAM       19  // PWM Width
#define CC_2_PARAM       16  // Chorus
#define CC_3_PARAM       -1  // LFO Rate
#define CC_4_PARAM       -1  // LFO Pitch
#define CC_5_PARAM       -1  // LFO PWM
#define CC_6_PARAM       17  // Filter Strength
#define CC_7_PARAM       72  // Amp Release
#define CC_8_PARAM       83  // Filter Release
#define CC_9_PARAM       76  // PWM Volume
#define CC_10_PARAM      77  // Saw Volume
#define CC_11_PARAM      93  // Sub Volume
#define CC_12_PARAM      74  // Cutoff
#define CC_13_PARAM      71  // Resonance
#define CC_14_PARAM      80  // Filter Attack
#define CC_15_PARAM      81  // Filter Decay
#define CC_16_PARAM      82  // Filter Sustain
#define CC_17_PARAM      18  // Noise volume
#define CC_18_PARAM      73  // Amp Attack
#define CC_19_PARAM      79  // Amp Sustain
#define CC_20_PARAM      75  // Amp Decay
#define CC_21_PARAM      -1
#define CC_22_PARAM      -1
#define CC_23_PARAM      -1

// DCO Encoder Mapping
#define ENC_1_PARAM    1    // PWM Width
#define ENC_2_PARAM    22   // Chorus
#define ENC_3_PARAM    5    // LFO Rate
#define ENC_4_PARAM    8    // LFO Pitch
#define ENC_5_PARAM    7    // LFO PWM
#define ENC_6_PARAM    13   // Filter Strength
#define ENC_7_PARAM    21   // Amp Release
#define ENC_8_PARAM    17   // Filter Release
#define ENC_9_PARAM    0    // PWM Volume
#define ENC_10_PARAM   2    // Saw Volume
#define ENC_11_PARAM   3    // Sub Volume
#define ENC_13_PARAM   12   // Resonance
#define ENC_14_PARAM   14   // Filter Attack
#define ENC_15_PARAM   15   // Filter Decay
#define ENC_16_PARAM   16   // Filter Sustain
#define ENC_17_PARAM   4    // Noise volume
#define ENC_18_PARAM   18   // Amp Attack
#define ENC_19_PARAM   20   // Amp Sustain
#define ENC_20_PARAM   19   // Amp Decay

//DCO Parameters (31 total):
// 0: PWM Volume (0.0-1.0)
// 1: PWM Width (0.0-1.0) 
// 2: Saw Volume (0.0-1.0)
// 3: Sub Volume (0.0-1.0)
// 4: Noise Volume (0.0-1.0)
// 5: LFO Rate (0.0-1.0)
// 6: LFO Delay (0.0-1.0)
// 7: LFO>PWM (0.0-1.0)
// 8: LFO>Pitch (0.0-1.0)
// 9: LFO>Filter (0.0-1.0)
// 10: HPF Cutoff (0.0-1.0)
// 11: LPF Cutoff (0.0-1.0)
// 12: Resonance (0.0-1.0)
// 13: Filter Strength (0.0-1.0)
// 14: Filt Attack (0.0-1.0)
// 15: Filt Decay (0.0-1.0)
// 16: Filt Sustain (0.0-1.0)
// 17: Filt Release (0.0-1.0)
// 18: Amp Attack (0.0-1.0)
// 19: Amp Decay (0.0-1.0)
// 20: Amp Sustain (0.0-1.0)
// 21: Amp Release (0.0-1.0)
// 22: Chorus Mode (0.0-1.0)
// 23-24: Reserved
// 25: Play Mode (0.0-1.0)
// 26: Glide Time (0.0-1.0)
// 27-29: Reserved
// 30: MIDI Channel (0.0-1.0)

#define ENC_21_PARAM   20  // Amp Sustain
#define ENC_22_PARAM   21  // Amp Release
#define ENC_23_PARAM   22  // Chorus Mode

// Menu Encoder Configuration
#define MENU_ENCODER_PARAM  11  // Filter Cutoff

#endif // PROJECT_DCO

#ifdef PROJECT_FM
// ============================================================================
// FM-Teensy-Synth Configuration
// ============================================================================

// FM MIDI CC Parameter Mapping
#define CC_1_PARAM       73 // Algorithm
#define CC_2_PARAM       83 // Feedback
#define CC_3_PARAM       74 // LFO Speed
#define CC_4_PARAM       71 // Master Volume
#define CC_5_PARAM       75 // OP1 Level
#define CC_6_PARAM       79 // OP2 Level
#define CC_7_PARAM       72 // OP3 Level
#define CC_8_PARAM       80 // OP4 Level
#define CC_9_PARAM       81 // OP5 Level
#define CC_10_PARAM      82 // OP6 Level
#define CC_11_PARAM      78
#define CC_12_PARAM      76
#define CC_13_PARAM      77
#define CC_14_PARAM      93
#define CC_15_PARAM      18
#define CC_16_PARAM      19
#define CC_17_PARAM      16
#define CC_18_PARAM      17
#define CC_19_PARAM      85
#define CC_20_PARAM      86

// FM Encoder Mapping
#define ENC_1_PARAM    0   // Algorithm
#define ENC_2_PARAM    1   // Feedback
#define ENC_3_PARAM    2   // LFO Speed
#define ENC_4_PARAM    3   // Master Volume
#define ENC_5_PARAM    4   // OP1 Level
#define ENC_6_PARAM    5   // OP2 Level
#define ENC_7_PARAM    6   // OP3 Level
#define ENC_8_PARAM    7   // OP4 Level
#define ENC_9_PARAM    8   // OP5 Level
#define ENC_10_PARAM   9   // OP6 Level
#define ENC_11_PARAM   -1
#define ENC_13_PARAM   -1
#define ENC_14_PARAM   -1
#define ENC_15_PARAM   -1
#define ENC_16_PARAM   -1
#define ENC_17_PARAM   -1
#define ENC_18_PARAM   -1
#define ENC_19_PARAM   -1
#define ENC_20_PARAM   -1

//FM Parameters (10 total):
// 0: Algorithm (0-31)
// 1: Feedback (0-7)
// 2: LFO Speed (0-99)
// 3: Master Volume (0-99)
// 4: OP1 Level (0-99)
// 5: OP2 Level (0-99)
// 6: OP3 Level (0-99)
// 7: OP4 Level (0-99)
// 8: OP5 Level (0-99)
// 9: OP6 Level (0-99)

// Menu Encoder Configuration
#define MENU_ENCODER_PARAM  -1  // Algorithm

#endif // PROJECT_FM

#ifdef PROJECT_MINI
// ============================================================================
// Mini-Teensy-Synth Configuration
// ============================================================================


#define CC_1_PARAM       -1 // OSC1_RANGE
#define CC_2_PARAM       -1 // OSC2_RANGE
#define CC_3_PARAM       -1 // OSC3_RANGE
#define CC_4_PARAM       -1 // OSC2_FINE
#define CC_5_PARAM       -1 // OSC3_FINE
#define CC_6_PARAM       -1 // OSC1_WAVE
#define CC_7_PARAM       -1 // OSC2_WAVE
#define CC_8_PARAM       -1 // OSC3_WAVE
#define CC_9_PARAM       -1 // OSC1_VOLUME
#define CC_10_PARAM      -1 // OSC2_VOLUME
#define CC_11_PARAM      -1 // OSC3_VOLUME
#define CC_12_PARAM      74 // CUTOFF
#define CC_13_PARAM      71 // RESONANCE
#define CC_14_PARAM      80 // FILTER_ATTACK
#define CC_15_PARAM      81 // FILTER_DECAY/RELEASE
#define CC_16_PARAM      82 // FILTER_SUSTAIN
#define CC_17_PARAM      -1 // NOISE_VOLUME
#define CC_18_PARAM      73 // AMP_ATTACK
#define CC_19_PARAM      79 // AMP_SUSTAIN
#define CC_20_PARAM      75 // AMP_DECAY
#define CC_21_PARAM      -1 // LFO_Rate
#define CC_22_PARAM      -1 // LFO_Depth
#define CC_23_PARAM      -1 // LFO_Target

// Mini Encoder Mapping
#define ENC_1_PARAM    0   // OSC1_RANGE
#define ENC_2_PARAM    1   // OSC2_RANGE
#define ENC_3_PARAM    2   // OSC3_RANGE
#define ENC_4_PARAM    3   // OSC2_FINE
#define ENC_5_PARAM    4   // OSC3_FINE
#define ENC_6_PARAM    5   // OSC1_WAVE
#define ENC_7_PARAM    6   // OSC2_WAVE
#define ENC_8_PARAM    7   // OSC3_WAVE
#define ENC_9_PARAM    8   // OSC1_VOLUME
#define ENC_10_PARAM   9   // OSC2_VOLUME
#define ENC_11_PARAM   10  // OSC3_VOLUME
#define ENC_13_PARAM   12  // RESONANCE
#define ENC_14_PARAM   13  // FILTER_ATTACK
#define ENC_15_PARAM   14  // FILTER_DECAY/RELEASE
#define ENC_16_PARAM   15  // FILTER_SUSTAIN
#define ENC_17_PARAM   16  // NOISE_VOLUME
#define ENC_18_PARAM   17  // AMP_ATTACK
#define ENC_19_PARAM   18  // AMP_SUSTAIN
#define ENC_20_PARAM   19  // AMP_DECAY
#define ENC_21_PARAM   22  // LFO_Rate
#define ENC_22_PARAM   23  // LFO_Depth
#define ENC_23_PARAM   25  // LFO_Target

//Mini-Teensy Parameters (31 total):
// 0: Osc1 Range (32' to LO)
// 1: Osc2 Range (32' to LO)
// 2: Osc3 Range (32' to LO)
// 3: Osc2 Fine (±12 semitones)
// 4: Osc3 Fine (±12 semitones)
// 5: Osc1 Wave (Triangle to Pulse)
// 6: Osc2 Wave (Triangle to Pulse)
// 7: Osc3 Wave (Triangle to Pulse)
// 8: Osc1 Volume (0.0-1.0)
// 9: Osc2 Volume (0.0-1.0)
// 10: Osc3 Volume (0.0-1.0)
// 11: Filter Cutoff (20Hz-20kHz)
// 12: Filter Resonance (0.0-3.0)
// 13: Filter Attack (1-3000ms)
// 14: Filter Decay (10-5000ms)
// 15: Filter Sustain (0.0-1.0)
// 16: Noise Volume (0.0-1.0)
// 17: Amp Attack (1-3000ms)
// 18: Amp Sustain (0.0-1.0)
// 19: Amp Decay (10-5000ms)
// 20: Osc1 Fine (±12 semitones)
// 21: Filter Strength (0.0-1.0)
// 22: LFO Rate (0.1-20Hz)
// 23: LFO Depth (0.0-1.0)
// 24: LFO Enable (0/1)
// 25: LFO Target (Pitch/Filter/Amp)
// 26: Play Mode (Mono/Poly/Legato)
// 27: Glide Time (0-1000ms)
// 28: Noise Type (White/Pink)
// 29: Macro Mode (0/1)
// 30: MIDI Channel (0-16)

// Menu Encoder Configuration
#define MENU_ENCODER_PARAM  11  // CUTOFF

#endif // PROJECT_MINI

#ifdef PROJECT_MACRO
// ============================================================================
// MacroOscillator-Teensy-Synth Configuration
// ============================================================================

// Macro MIDI CC Parameter Mapping
#define CC_1_PARAM       73
#define CC_2_PARAM       75
#define CC_3_PARAM       79
#define CC_4_PARAM       72
#define CC_5_PARAM       80
#define CC_6_PARAM       81
#define CC_7_PARAM       82
#define CC_8_PARAM       83
#define CC_9_PARAM       74
#define CC_10_PARAM      71
#define CC_11_PARAM      76
#define CC_12_PARAM      76
#define CC_13_PARAM      77
#define CC_14_PARAM      93
#define CC_15_PARAM      18
#define CC_16_PARAM      19
#define CC_17_PARAM      16
#define CC_18_PARAM      17
#define CC_19_PARAM      -1
#define CC_20_PARAM      -1

// Macro Encoder Mapping
#define ENC_1_PARAM    0    // BRAIDS_SHAPE
#define ENC_2_PARAM    1    // BRAIDS_TIMBRE
#define ENC_3_PARAM    2    // BRAIDS_COLOR
#define ENC_4_PARAM    -1   //
#define ENC_5_PARAM    -1   //
#define ENC_6_PARAM    10   // BRAIDS_FILTER_STR
#define ENC_7_PARAM    -1   //
#define ENC_8_PARAM    -1   //
#define ENC_9_PARAM    -1   //
#define ENC_10_PARAM   -1   //
#define ENC_11_PARAM   11   // BRAIDS_FILT_ATTACK
#define ENC_13_PARAM   9    // BRAIDS_RES
#define ENC_14_PARAM   12   // BRAIDS_FILT_DECAY
#define ENC_15_PARAM   13   // BRAIDS_FILT_SUSTAIN
#define ENC_16_PARAM   14   // BRAIDS_FILT_RELEASE
#define ENC_17_PARAM   4    // BRAIDS_AMP_ATTACK
#define ENC_18_PARAM   5    // BRAIDS_AMP_DECAY
#define ENC_19_PARAM   7    // BRAIDS_AMP_RELEASE
#define ENC_20_PARAM   6    // BRAIDS_AMP_SUSTAIN
#define ENC_21_PARAM   -1   //
#define ENC_22_PARAM   -1   //
#define ENC_23_PARAM   -1   //

//MacroOscillator (Braids) Parameters (22 total):
// 0: Shape (0-42) - Braids synthesis algorithm
// 1: Timbre (0-127) - Timbral control
// 2: Color (0-127) - Color/tone control
// 3: Coarse (±48 semitones) - Transpose
// 4: Amp Attack (0-127)
// 5: Amp Decay (0-127)
// 6: Amp Sustain (0-127)
// 7: Amp Release (0-127)
// 8: Filter Cutoff (0-127)
// 9: Filter Resonance (0-127)
// 10: Filter Strength (0-127)
// 11: Filter Attack (0-127)
// 12: Filter Decay (0-127)
// 13: Filter Sustain (0-127)
// 14: Filter Release (0-127)
// 15: Volume (0-127)
// 16: LFO Rate (0.1-20 Hz)
// 17: LFO>Timbre (0-100%)
// 18: LFO>Color (0-100%)
// 19: LFO>Pitch (0-100%)
// 20: LFO>Filter (0-100%)
// 21: LFO>Volume (0-100%)


// Menu Encoder Configuration
#define MENU_ENCODER_PARAM  8  // Menu-only

#endif // PROJECT_MACRO

// ============================================================================
// DIN MIDI Configuration (shared across all projects)
// ============================================================================

/*
 * DIN MIDI Setup Instructions:
 * 
 * HARDWARE REQUIRED:
 * - 6N138 optocoupler IC
 * - 220Ω resistor  
 * - 5-pin DIN MIDI connector
 * - Standard MIDI interface circuit (see MIDI specification)
 * 
 * WIRING:
 * 1. Build MIDI input circuit: DIN connector → 6N138 optocoupler → 220Ω resistor
 * 2. Connect MIDI circuit output to Teensy Serial1 RX (Pin 0)
 * 3. IMPORTANT: Move enc3 (Color/Range/etc) CLK wire from Pin 0 to surface mount pin (42-47)
 * 
 * USAGE:
 * - Install "MIDI Library" by Francois Best via Arduino Library Manager
 * - Uncomment #define USE_DIN_MIDI above
 * - Can work with both USB Device MIDI (default) and USB Host MIDI
 * - Supports USB and DIN MIDI simultaneously
 * - Uses same MIDI channel setting from Settings menu
 * - Receives Note On/Off, Control Change, and Pitch Bend
 */

#endif // CONFIG_H
