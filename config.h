#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Hardware Configuration Template
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

// ORIGINAL MINI-TEENSY ENCODER MAPPING (exactly as it was before)
// Array index = parameter index, encoder reading directly maps to parameter
#define ENC_1_PARAM    0   // OSC1_RANGE      // enc1 (pins 4,5) → Osc1 Range
#define ENC_2_PARAM    1   // OSC2_RANGE      // enc2 (pins 2,3) → Osc2 Range  
#define ENC_3_PARAM    2   // OSC3_RANGE      // enc3 (pins 0,1) → Osc3 Range
#define ENC_4_PARAM    3   // OSC2_FINE       // enc4 (pins 8,9) → Osc2 Fine
#define ENC_5_PARAM    4   // OSC3_FINE       // enc5 (pins 6,7) → Osc3 Fine
#define ENC_6_PARAM    5   // OSC1_WAVE       // enc6 (pins 25,27) → Osc1 Wave
#define ENC_7_PARAM    6   // OSC2_WAVE       // enc7 (pins 12,24) → Osc2 Wave
#define ENC_8_PARAM    7   // OSC3_WAVE       // enc8 (pins 10,11) → Osc3 Wave
#define ENC_9_PARAM    8   // OSC1_VOLUME     // enc9 (pins 29,30) → Osc1 Volume
#define ENC_10_PARAM   9   // OSC2_VOLUME     // enc10 (pins 28,26) → Osc2 Volume
#define ENC_11_PARAM   10  // OSC3_VOLUME     // enc11 (pins 21,20) → Osc3 Volume
#define ENC_13_PARAM   12  // RESONANCE       // enc13 (pins 34,33) → Resonance
#define ENC_14_PARAM   13  // FILTER_ATTACK   // enc14 (pins 50,41) → Filter Attack
#define ENC_15_PARAM   14  // FILTER_DECAY    // enc15 (pins 23,22) → Filter Decay/Release
#define ENC_16_PARAM   15  // FILTER_SUSTAIN  // enc16 (pins 36,35) → Filter Sustain
#define ENC_17_PARAM   16  // NOISE_VOLUME    // enc17 (pins 31,32) → Noise Volume
#define ENC_18_PARAM   17  // AMP_ATTACK      // enc18 (pins 17,16) → Amp Attack
#define ENC_19_PARAM   18  // AMP_SUSTAIN     // enc19 (pins 38,37) → Amp Sustain
#define ENC_20_PARAM   19  // AMP_DECAY       // enc20 (pins 40,39) → Amp Decay

// Menu-only parameters (20-30) - now mappable to knobs:
//   - 11: Cutoff
//   - 20: Osc1 Fine (extended fine tuning like Osc2/3)
//   - 21: Filter Strength (filter envelope amount)
//   - 22: LFO Rate (0.1 - 20 Hz)
//   - 23: LFO Depth (0-100%)
//   - 24: LFO Toggle (on/off)
//   - 25: LFO Target (Pitch/Filter/Amp)
//   - 26: Play Mode (Mono/Poly/Legato)
//   - 27: Glide Time (portamento speed)
//   - 28: Noise Type (White/Pink)
//   - 29: Macro Mode (toggles macro assignments)
//   - 30: MIDI Channel (Omni/1-16)

// ============================================================================
// Menu Encoder Parameter Configuration
// ============================================================================
// Set which parameter the menu encoder controls when NOT in menu mode
// Use -1 for menu-only mode (turning encoder auto-enters menu, no button press needed)
// Available parameters: 0-30 (see list above) 
#define MENU_ENCODER_PARAM  11  // Cutoff (traditional - button press to enter menu)

// EXAMPLES:
// #define MENU_ENCODER_PARAM  11  // Cutoff (traditional - button press to enter menu)
// #define MENU_ENCODER_PARAM  22  // LFO Rate (button press to enter menu)
// #define MENU_ENCODER_PARAM  21  // Filter Strength (button press to enter menu) 
// #define MENU_ENCODER_PARAM  -1  // Menu-only (turn encoder to auto-enter menu)


// Display Configuration (Choose ONE - comment out the other)
#define USE_LCD_DISPLAY
// #define USE_OLED_DISPLAY

// MIDI Configuration - Enable the MIDI sources you want to use
#define USE_USB_DEVICE_MIDI // USB Device MIDI for DAW/computer connection (default)
// #define USE_MIDI_HOST       // USB Host MIDI for external controllers connected to Teensy 

/*
 * EXAMPLE: DIN MIDI Pin Conflict Resolution
 * 
 * If you enable DIN MIDI, it uses pin 0 (Serial1 RX), which conflicts with ENC_3_CLK.
 * To resolve this, move enc3 to a different pin pair:
 * 
 * #define ENC_3_CLK    42     // Move enc3 to surface mount pins
 * #define ENC_3_DT     43     // (or any other available pins)
 * 
 * Then enable DIN MIDI below.
 */

// TODO: Test DIN MIDI
// Uncomment to enable DIN MIDI support (requires moving enc3 from pin 0)
// #define ENABLE_DIN_MIDI

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
 * - Uncomment #define ENABLE_DIN_MIDI above
 * - Supports both USB and DIN MIDI simultaneously
 * - Uses same MIDI channel setting from Settings menu
 * - Receives Note On/Off, Control Change, and Pitch Bend
 */

#endif // CONFIG_H