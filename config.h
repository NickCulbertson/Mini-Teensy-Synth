#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Preferences - Mini-Teensy Synth
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
#define CC_21_PARAM      -1 // AMP_ATTACK
#define CC_22_PARAM      -1 // AMP_SUSTAIN
#define CC_23_PARAM      -1 // AMP_DECAY

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

// Menu Encoder Configuration
#define MENU_ENCODER_PARAM  11  // CUTOFF

// ============================================================================
// DIN MIDI Configuration (UNTESTED)
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
 * 3. IMPORTANT: Move enc3 (Osc3 Range) CLK wire from Pin 0 to surface mount pin (42-47)
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
