# Mini Teensy Synth

A fully-featured **6-voice polyphonic analog synthesizer** built with the Teensy 4.1 microcontroller, inspired by the classic **Minimoog**. This project delivers professional-grade synthesis with comprehensive MIDI support, intuitive menu navigation, and multiple play modes.

## Features

### 🎹 **Polyphonic Synthesis**
- **6-voice polyphony** with voice allocation
- **3 oscillators** per voice with frequency ranges (32', 16', 8', 4', 2', LO)
- **6 waveforms** per oscillator (Triangle, Reverse Saw, Sawtooth, Square, Pulse variations)
- **White & Pink noise generator**

### 🎛️ **Sound Design**
- **24dB Moog-style ladder filter** with cutoff, resonance, and envelope amount
- **ADSR envelopes** for both amplitude and filter
- **Independent LFO system** with rate, depth, toggle, and target selection (Pitch/Filter/Amp)
- **Macro knob functionality** - Filter envelope knobs can be remapped to control LFO parameters
- **Extended fine tuning** (±12 semitones with ±25 cent precision)
- **Glide/Portamento** with adjustable time (50ms - 1000ms)

### 🎵 **Play Modes**
- **Polyphonic** - Multiple notes simultaneously 
- **Monophonic** - Single note with retriggered envelopes
- **Legato** - Single note with sustained envelopes

### 🎚️ **Hardware Control**
- **20 rotary encoders** for real-time parameter control
- **I2C LCD display** (16x2) for visual feedback
- **Menu encoder** with button for deep parameter access
- **Hierarchical menu system** with intuitive navigation

### 🎼 **MIDI Integration**
- **USB Device MIDI** (plug-and-play with DAWs)
- **Polyphonic note on/off** with velocity sensitivity
- **Pitch wheel** support (±2 semitone range)
- **Mod wheel** adds to LFO depth for enhanced performance control
- **All Notes Off** panic function

### 🎨 **Preset System**
- **Dynamic preset system** - automatically adapts to preset count
- **Instant preset loading** with parameter synchronization
- **Professional sounds** including: 80s Brass, Saw Keys, Square Keys, 8-Bit Square, Butter Supersaw, West Coast Lead, Teensy Lead, Mono Bass, Funk Bass, Smashed Bass, 8-Bit Harp, Love Pad, Thoughtful Pad, Saw Pad, and more

## Hardware Requirements

### **Core Components**
- **Teensy 4.1** microcontroller
- **Teensy Audio Shield** (Rev D recommended)
- **20x Rotary Encoders** (with detents)
- **1x Menu Encoder** with push button
- **16x2 I2C LCD** display (HD44780 compatible)

### **Connections**
- **Power**: 5V supply capable of 500mA+
- **Audio**: 1/8" stereo output jack
- **USB**: Type-C for MIDI and programming
- **I2C**: SDA (18), SCL (19) for LCD
- **Encoders**: See pin mapping in code comments

## Software Installation

### **Prerequisites**
1. **Arduino IDE** 2.0+ or **PlatformIO**
2. **Teensy Loader** and **Teensyduino** add-on
3. **Required Libraries:**
   - `Audio.h` (Teensy Audio Library)
   - `Encoder.h` 
   - `LiquidCrystal_I2C.h`
   - `Wire.h`, `SPI.h`, `SD.h`, `SerialFlash.h`

### **Installation Steps**
1. **Clone this repository:**
   ```bash
   git clone https://github.com/your-username/Mini-Teensy-Synth.git
   ```

2. **Install Teensyduino:**
   - Download from [PJRC website](https://www.pjrc.com/teensy/td_download.html)
   - Install with Arduino IDE integration

3. **Install Libraries:**
   - Open Arduino IDE → Library Manager
   - Search and install: `LiquidCrystal I2C`, `Encoder`
   - Teensy Audio Library comes with Teensyduino

4. **Upload:**
   - Open `Mini-Teensy-Synth.ino`
   - Select **Board: Teensy 4.1**
   - Select **USB Type: MIDI**
   - Click Upload

## Usage

### **Menu Navigation**
- **Single Click**: Navigate forward or go back from parameters
- **Encoder Turn**: Browse options and adjust values
- **Back Buttons**: Clear navigation path in every submenu

### **Menu Structure**
```
Main Menu
├── Presets (dynamic preset system)
├── OSC 1 → Range, Waveform, Volume, Fine Tune
├── OSC 2 → Range, Waveform, Volume, Fine Tune  
├── OSC 3 → Range, Waveform, Volume, Fine Tune
├── Noise → Volume, Type (White/Pink)
├── Envelopes → Amp/Filter Attack, Sustain, Decay
├── Filter → Cutoff, Resonance, Strength
├── LFO → Rate, Depth, Toggle, Target
├── Voice Mode → Play Mode, Glide Time
├── Settings → Macro Knobs (Filter Env ↔ LFO Controls)
└── Exit
```

### **Real-time Controls**
- **20 Hardware Encoders**: Direct parameter access with optimized 128-step resolution
- **Menu System**: Access to all synthesis parameters with consistent navigation
- **MIDI Input**: Play from DAW or MIDI keyboard with low-latency processing
- **Preset Loading**: Instant recall of saved sounds
- **LFO Independence**: Internal LFO works standalone, mod wheel adds additional depth

### **Macro Knob System**
- **Remappable filter knobs**: Filter Attack/Decay/Release encoders can switch between controlling filter envelope parameters and LFO Rate/Depth/Target
- **Settings Menu Control**: Navigate to Settings → Macro Knobs and turn encoder to toggle between "Filter Env" and "LFO Controls"
- **Real-time switching**: Change parameter mapping instantly while performing
- **Enhanced workflow**: Access frequently-used LFO controls without menu diving

## Technical Specifications

- **Polyphony**: 6 voices
- **Sample Rate**: 44.1 kHz
- **Bit Depth**: 16-bit
- **Latency**: <3ms
- **CPU Usage**: ~40% (Teensy 4.1 @ 600MHz)
- **Memory**: ~30% PSRAM usage
- **MIDI Channels**: 1-16 (configurable)

## Project Structure

```
Mini-Teensy-Synth/
├── Mini-Teensy-Synth.ino    # Main synthesizer code
├── README.md                # This file
└── LICENSE                  # Open source license
```

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test thoroughly on hardware
4. Submit a pull request with clear description

## License

This project is open source. See `LICENSE` file for details.

## Acknowledgments

- **PJRC** for the incredible Teensy platform and Audio Library
- **Minimoog** for the original analog synthesizer inspiration
- **Open source community** for libraries and examples

---

🎵 **Built with passion for analog synthesis and open hardware** 🎵