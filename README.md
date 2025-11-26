# MiniTeensy Synth

A **6-voice polyphonic virtual analog synthesizer** built with the Teensy 4.1 microcontroller, inspired by the classic **Minimoog**. Features comprehensive synthesis with USB audio/MIDI and intuitive menu control.

[*“DIY Minimoog Inspired Synth | A Teensy Powered Tribute”*](https://youtu.be/ETfcjzIK8Po)

[![Watch the video](https://img.youtube.com/vi/ETfcjzIK8Po/hqdefault.jpg)](https://youtu.be/ETfcjzIK8Po)

**Featured on [Synth Anatomy](https://synthanatomy.com/2025/11/moby-pixel-miniteensy-a-teensy-based-diy-polyphonic-minimoog.html), [Synthtopia](https://www.synthtopia.com/content/2025/11/25/diy-miniteensy-is-an-open-source-polysynth-based-on-the-minimoog-synth-voice/), and [Matrix Synth](https://www.matrixsynth.com/2025/11/diy-minimoog-inspired-synth-teensy.html)! 🚀**

## Key Features
- **6-voice polyphony** with 3 oscillators per voice (18 total)
- **6 waveforms** per oscillator
- **Noise** shared white/pink noise
- **24dB Moog-style ladder filter** with ADSR envelopes  
- **Independent LFO** with pitch/filter/amp targets
- **Macro knob system** - remap filter knobs to LFO controls
- **USB Audio + MIDI** - single cable to computer
- **MIDI channel selection** - receive on specific channel or omni mode
- **20 hardware encoders** + LCD for real-time control
- **20 Presets** - 80s Brass, Saw Keys, Bass sounds, Pads, and more
- **Multiple play modes** - Mono, Poly, Legato with glide

## Hardware Requirements

**Full Build:**
- **Teensy 4.1** microcontroller  
- **19x Rotary Encoders** + **1x Menu Encoder** with push button
- **16x2 I2C LCD** display
- Enclosure, knob caps, hookup wire

**Minimal Build:**
- **Teensy 4.1** + **1x Menu Encoder** + **LCD**
- USB cable for audio/MIDI output
- All parameters accessible via menu system

**Parts:**
- Teensy 4.1 (https://www.sparkfun.com/teensy-4-1-without-ethernet.html)
- Menu Encoder (https://www.amazon.com/Taiss-KY-040-Encoder-15×16-5-Arduino/dp/B07F26CT6B/ref=sr_1_3_pp)
- Other Encoders (https://www.aliexpress.us/item/3256801237549169.html)
- LCD 2X16 (https://www.amazon.com/Hosyond-Display-Module-Arduino-Raspberry/dp/B0BWTFN9WF/ref=sr_1_2)
- Knobs (https://www.amazon.com/Taiss-Silver-Rotary-Potentiometer-Diameter/dp/B07F25NMJ7/ref=sr_1_5)

## Wiring

### **Minimal Build (LCD + 1 Encoder)**
Perfect for testing or budget builds:
```
Menu Encoder: CLK→14, DT→13, SW→15
LCD (I2C):   SDA→18, SCL→19, VCC→3.3V, GND→GND
Power:       USB cable to computer
```
**That's it!** All synthesis parameters accessible through menu.

### **Full Build (20 Encoders)**
**Parameter Encoder Pins (configurable in code):**
```
Osc Ranges:  enc1(4,5), enc2(2,3), enc3(0,1)
Osc Fine:    enc4(8,9), enc5(6,7)
Osc Waves:   enc6(25,27), enc7(12,24), enc8(10,11)  
Volumes:     enc9(29,30), enc10(28,26), enc11(21,20)
Filter:      enc13(34,33), enc14(50,41), enc15(23,22), enc16(36,35)
Noise Vol:   enc17(31,32)
Amp Env:     enc18(17,16), enc19(38,37), enc20(40,39)
```

**Encoder Wiring:**
- **CLK/DT pins** to Teensy as shown above
- **All encoder GND pins** daisy-chained to Teensy GND
- **Menu encoder VCC pin** to Teensy 3.3V (if ebcoder has a breakout board)

*Macro Mode: enc14/enc15/enc16 become LFO Rate/Depth/Target instead of Filter Attack/Decay/Sustain*

### **Audio + MIDI Options**

**Option 1: Computer DAW (Current Setup)**
- Single USB cable provides audio output + MIDI input
- Plug-and-play with some DAWs

**Option 2: Standalone with MIDI Keyboard**  
For standalone use without computer:
- Connect USB MIDI keyboard to **Teensy USB Host pins**
- Requires USB Host cable
- Audio output through computer USB (or modify code for I2S/line out)
- **Note:** Requires code modification for USB Host MIDI instead of USB Device

**Option 3: DIN MIDI Support (Hardware Modification)**
Add traditional 5-pin DIN MIDI input for hardware compatibility:

**Hardware Required:**
- 6N138 optocoupler IC  
- 220Ω resistor
- 5-pin DIN MIDI connector
- Standard MIDI interface circuit (see MIDI spec)

**Wiring:**
- Connect MIDI interface circuit output to **Teensy Serial1 (Pin 0)**
- MIDI input circuit connects to DIN connector pins 4,5
- **Important:** Pin 0 is currently used by enc3 (Osc3 Range). You'll need to move enc3's CLK wire from Pin 0 to one of the surface mount pins on the bottom of the Teensy 4.1 board (pins 42-47 are available)

**Code Changes Required:**
```cpp
// In Mini-Teensy-Synth.ino, uncomment this line:
#define ENABLE_DIN_MIDI

// That's it! The DIN MIDI support code is already included.
```

**Features:**
- Supports both USB MIDI and DIN MIDI simultaneously
- Selectable MIDI channel (1-16 or Omni) via Settings menu
- Standard MIDI implementation (Note On/Off, CC, Pitch Bend)

## Software Installation

1. **Install Arduino IDE** 2.0+ from [arduino.cc](https://www.arduino.cc/en/software)
2. **Add Teensy support:**
   - **Modern IDE:** Tools → Board Manager → Search "Teensy" → Install
   - **Fallback:** Download Teensyduino from [PJRC](https://www.pjrc.com/teensy/td_download.html)
3. **Install libraries:** Tools → Manage Libraries → Install "LiquidCrystal I2C" and "Encoder" 
   - **For DIN MIDI:** Also install "MIDI Library" by Francois Best 
4. **Download this project** and open `Mini-Teensy-Synth.ino`
5. **Configure:** Board→Teensy 4.1, USB Type→Audio+MIDI
6. **Upload!** Your computer will show "Teensy Audio" device

## Usage

**Menu Navigation:** Click encoder to navigate, turn to adjust values. All parameters accessible via hierarchical menu system.

**Macro Knobs:** Settings → Macro Knobs toggles filter envelope controls between Filter (Attack/Decay/Sustain) and LFO (Rate/Depth/Target) modes.

**MIDI Channel:** Settings → MIDI Channel sets which MIDI channel to receive (1-16, or Omni for all channels).

**Menu Structure:**
```
Presets | OSC 1-3 | Noise | Envelopes | Filter | LFO | Voice Mode | Settings
```

## Contributing
Fork, test on hardware, submit PR. Open source project!

## Thanks
- **PJRC** - Teensy platform and Audio Library
- **Minimoog** - The OG
- **Open source community**
