# Changelog

All notable changes to the MiniTeensy Synth project will be documented in this file.

## [v1.1] - 2025-12-23

### Changed
- **BREAKING CHANGE:** Menu encoder pin assignments to avoid encoder timing issues
  - Menu Encoder CLK: Pin 13 → Pin 14
  - Menu Encoder DT: Pin 14 → Pin 15  
  - Menu Encoder SW: Pin 15 → Pin 13
- Updated README.md pinout diagrams and wiring tables
- Added I2C display option
- Moved MenuNavigation to a separate file
- Added config file

### Fixed
- Menu encoder moved from pin 13 per the Encoder library documentation (LED interference)

### Notes
- **For existing builds:** Swap Menu Encoder CLK wire from pin 13 to pin 14, and SW wire from pin 15 to pin 13
- This change follows Paul Stoffregen's Encoder library recommendation to avoid pins with LEDs attached
- Pin 13 (onboard LED) is now used for the encoder button which is not affected by LED timing issues

## [v1.0] - 2025-11-25

### Added
- Initial release of MiniTeensy Synth
- 6-voice polyphonic synthesis with 3 oscillators per voice
- 24dB Moog-style ladder filter with ADSR envelopes
- LFO with multiple targets (pitch, filter, amplitude)
- 20 preset sounds (80s Brass, Saw Keys, Bass, Pads, etc.)
- USB Audio + MIDI support
- Hierarchical menu system with LCD/OLED display support
- Multiple play modes (Mono, Poly, Legato with glide)
- Macro knob system for LFO control remapping
- MIDI channel selection and DIN MIDI support
