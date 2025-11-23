# MiniTeensy Synth

A fully-featured **6-voice polyphonic virtual analog synthesizer** built with the Teensy 4.1 microcontroller, inspired by the classic **Minimoog**. This project delivers the best in microcontroller synthesis with comprehensive MIDI support, intuitive menu navigation, and multiple play modes.

## Features

### 🎹 **Polyphonic Synthesis**
- **6-voice polyphony** with voice allocation
- **3 oscillators** per voice (18 oscillators total) with frequency ranges (32', 16', 8', 4', 2', LO)
- **6 waveforms** per oscillator (Triangle, Reverse Saw, Sawtooth, Square, Pulse variations)
- **Shared White & Pink noise generators** (mixed into all voices)

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
- **Menu encoder** with button for parameter access
- **Hierarchical menu system** with intuitive navigation

### 🎼 **MIDI Integration**
- **USB Device MIDI** (plug-and-play with DAWs) - *Current setup*
- **Polyphonic note on/off** with velocity sensitivity
- **Pitch wheel** support (±2 semitone range)
- **Mod wheel** adds to LFO depth for enhanced performance control
- **Expandable to hardware MIDI** with code modifications

### 🎨 **Preset System**
- **Dynamic preset system** - automatically adapts to preset count
- **Instant preset loading** with parameter synchronization
- **Professional sounds** including: 80s Brass, Saw Keys, Square Keys, 8-Bit Square, Butter Supersaw, West Coast Lead, Teensy Lead, Mono Bass, Funk Bass, Smashed Bass, 8-Bit Harp, Love Pad, Thoughtful Pad, Saw Pad, and more

## Hardware Requirements

### **Core Components**
- **Teensy 4.1** microcontroller
- **20x Rotary Encoders** (with detents) - *Optional: See minimal build below*
- **1x Menu Encoder** with push button
- **16x2 I2C LCD** display (HD44780 compatible)

### **Minimal Build Option**
For testing or budget builds, you can start with just:
- **Teensy 4.1** (standalone - no audio shield needed)
- **1x Menu Encoder** with push button (pins 14, 13, 15)
- **16x2 I2C LCD** (pins 18, 19)

**Audio output:** Uses Teensy's built-in **USB Audio** - no additional hardware required!
All synthesis parameters are accessible through the menu system. Add hardware encoders later for direct real-time control.

### **Additional Components**
- **Enclosure** - Recommend aluminum or wood case
- **Knob caps** - 21 total (20 parameter + 1 menu)
- **MicroUSB cable** - for connecting and programming
- **Hookup wire** - I used jumper wires
- **Breadboard or perfboard** - For connections

## Hardware Assembly

### **Teensy 4.1 Pin Connections**

#### **Parameter Encoders (20 total)**
| Encoder | Function | CLK Pin | DT Pin | Parameter |
|---------|----------|---------|--------|-----------|
| enc1 | Osc1 Range | 4 | 5 | Oscillator 1 frequency range |
| enc2 | Osc2 Range | 2 | 3 | Oscillator 2 frequency range |
| enc3 | Osc3 Range | 0 | 1 | Oscillator 3 frequency range |
| enc4 | Osc2 Fine | 8 | 9 | Oscillator 2 fine tuning |
| enc5 | Osc3 Fine | 6 | 7 | Oscillator 3 fine tuning |
| enc6 | Osc1 Wave | 25 | 27 | Oscillator 1 waveform |
| enc7 | Osc2 Wave | 12 | 24 | Oscillator 2 waveform |
| enc8 | Osc3 Wave | 10 | 11 | Oscillator 3 waveform |
| enc9 | Volume 1 | 29 | 30 | Oscillator 1 volume |
| enc10 | Volume 2 | 28 | 26 | Oscillator 2 volume |
| enc11 | Volume 3 | 21 | 20 | Oscillator 3 volume |
| enc13 | Filter Cutoff | 34 | 33 | Filter cutoff frequency |
| enc14 | Filter Attack* | 50 | 41 | Filter envelope attack |
| enc15 | Filter Decay* | 23 | 22 | Filter envelope decay |
| enc16 | Filter Sustain | 36 | 35 | Filter envelope sustain |
| enc17 | Filter Resonance | 31 | 32 | Filter resonance |
| enc18 | Amp Attack | 17 | 16 | Amplitude envelope attack |
| enc19 | Amp Sustain | 38 | 37 | Amplitude envelope sustain |
| enc20 | Amp Decay | 40 | 39 | Amplitude envelope decay |

*_When macro mode is enabled, enc14/enc15 control LFO Rate/Depth instead_

#### **Menu Encoder**
| Function | Pin | Description |
|----------|-----|-------------|
| Menu CLK | 14 | Menu encoder clock |
| Menu DT | 13 | Menu encoder data |
| Menu SW | 15 | Menu encoder push button |

#### **LCD Display (I2C)**
| Function | Pin | Description |
|----------|-----|-------------|
| SDA | 18 | I2C data line |
| SCL | 19 | I2C clock line |
| VCC | 3.3V | Power supply |
| GND | GND | Ground |

**LCD I2C Address:** 0x27 (default)

#### **Audio Output (USB Audio)**
- **No additional hardware needed** - audio comes through USB
- Connect Teensy to computer via **MicroUSB**  
- Teensy appears as **USB Audio device** in your OS
- **High-quality digital audio** directly to computer
- **Perfect for:** DAW recording, software monitoring, headphone amps

#### **MIDI Connectivity Options**

**Option 1: USB Device MIDI (Current Setup - Recommended)**
- Same **MicroUSB** connection provides both audio and MIDI
- Teensy appears as **USB MIDI device** in DAW
- **Plug-and-play** - no additional hardware needed
- Supports bidirectional MIDI communication
- **Best for:** Studio use with DAW/computer

**Option 2: Traditional MIDI DIN (Hardware MIDI)**
If you need standalone operation or hardware MIDI keyboards:
- **MIDI IN:** Use Teensy pins for MIDI input circuit
- **MIDI Host USB:** Connect USB MIDI keyboards to Teensy's host pins
- **Requires additional circuitry:** MIDI optoisolator and DIN connectors
- **Best for:** Standalone operation, hardware MIDI keyboards

**Current firmware is configured for USB Device MIDI.** For hardware MIDI, you'll need to modify the code to use Serial MIDI instead of USB MIDI.

## Software Installation

### **Step 1: Install Arduino IDE**
1. **Download Arduino IDE 2.0+** from [arduino.cc](https://www.arduino.cc/en/software)
2. **Install** following platform-specific instructions
3. **Launch Arduino IDE** to verify installation

### **Step 2: Install Teensy Support**

**Option A: Modern Arduino IDE (2.1.0+)**
Recent versions may have built-in Teensy support:
1. **Tools → Board Manager** 
2. Search for **"Teensy"** by Paul Stoffregen
3. **Install** if available
4. **Verify** Teensy 4.1 appears in board selection

**Option B: Teensyduino (If needed)**
If Teensy boards don't appear in Arduino IDE:
1. **Download Teensyduino** from [PJRC website](https://www.pjrc.com/teensy/td_download.html)
2. **Run installer** - it will detect Arduino IDE automatically  
3. **Select components** - ensure "Teensy Audio Library" is checked
4. **Complete installation** and restart Arduino IDE

### **Step 3: Install Required Libraries**
Open Arduino IDE → **Tools → Manage Libraries** and install:

| Library | Author | Version | Purpose |
|---------|---------|---------|---------|
| `LiquidCrystal I2C` | Frank de Brabander | Latest | LCD display control |
| `Encoder` | Paul Stoffregen | Latest | Rotary encoder reading |

**Note:** These libraries come with Teensy support (Teensyduino or Board Manager):
- `Audio.h` - Teensy Audio Library (synthesis engine)
- `Wire.h` - I2C communication 
- `SPI.h` - SPI communication  
- `SD.h` - SD card support
- `SerialFlash.h` - Flash memory support

**Important:** If using Board Manager instead of Teensyduino, you may need to manually install the Audio library from [GitHub](https://github.com/PaulStoffregen/Audio).

### **Step 4: Download Project**
```bash
# Clone the repository
git clone https://github.com/your-username/Mini-Teensy-Synth.git

# Or download ZIP from GitHub and extract
```

### **Step 5: Configure Arduino IDE**
1. **Connect Teensy 4.1** via USB
2. **Select Board:**
   - Tools → Board → Teensyduino → **Teensy 4.1**
3. **Select USB Type:**
   - Tools → USB Type → **Audio + MIDI** 
   - This enables both USB audio output and MIDI functionality
4. **Select Port:**
   - Tools → Port → Select your Teensy port

### **Step 6: Upload Firmware**
1. **Open sketch:**
   - File → Open → `Mini-Teensy-Synth.ino`
2. **Verify compilation:**
   - Click ✓ (Verify) button
   - Should compile without errors
3. **Upload to Teensy:**
   - Click → (Upload) button  
   - Teensy Loader will automatically program the device
4. **Wait for completion:**
   - "Done uploading" message confirms success

### **Troubleshooting**
- **"Board not found"** → Check USB connection and drivers
- **"Library missing"** → Reinstall Teensyduino or missing libraries
- **"Compilation errors"** → Verify all libraries are correctly installed
- **"Upload failed"** → Press Teensy's program button manually

### **First Boot**
After successful upload:
1. **LCD displays:** "MiniTeensy Synth" and "6-Voice Poly"
2. **Audio device appears** in your OS as "Teensy Audio"
3. **Connect MIDI** keyboard or DAW for testing
4. **Turn encoders** to verify hardware functionality

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
- **20 Hardware Encoders**: Direct parameter access with optimized 128-step resolution *(Optional - can use menu-only)*
- **Menu System**: Access to all synthesis parameters with consistent navigation
- **MIDI Input**: Play from DAW or MIDI keyboard with low-latency processing
- **Preset Loading**: Instant recall of saved sounds
- **LFO Independence**: Internal LFO works standalone, mod wheel adds additional depth

**Note:** The synthesizer is fully functional with just the menu encoder and LCD. Hardware parameter encoders provide enhanced real-time control but are not required for basic operation.

### **Macro Knob System**
- **Remappable filter knobs**: Filter Attack/Decay/Release encoders can switch between controlling filter envelope parameters and LFO Rate/Depth/Target
- **Settings Menu Control**: Navigate to Settings → Macro Knobs and turn encoder to toggle between "Filter Env" and "LFO Controls"
- **Real-time switching**: Change parameter mapping instantly while performing
- **Enhanced workflow**: Access frequently-used LFO controls without menu diving

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
