
#include <Arduino.h>
#include <M5Unified.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>
#include <math.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

// MusicLED 180x180 home-screen icon (restored from v1.6).
static const uint8_t MUSICLED_ICON_PNG[] = {0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0xb4,0x00,0x00,0x00,0xb4,0x08,0x02,0x00,0x00,0x00,0xb2,0xaf,0x91,0x65,0x00,0x00,0x04,0x69,0x49,0x44,0x41,0x54,0x78,0xda,0xed,0xdd,0x21,0x6f,0x14,0x41,0x18,0x80,0xe1,0xeb,0xa6,0xae,0xa6,0x09,0xa6,0xb2,0x09,0x0e,0x0b,0x39,0x42,0x25,0x12,0x83,0xad,0xeb,0x1f,0x40,0x35,0x4d,0x70,0x88,0xba,0x26,0xa4,0x8a,0x3f,0x50,0x57,0x5b,0x53,0x59,0x59,0x02,0xa1,0x16,0x57,0x8f,0x41,0xe2,0x11,0x24,0xcd,0xd1,0xce,0xed,0xed,0xde,0xce,0xce,0xed,0xcc,0x3c,0xaf,0xe2,0x2e,0x0d,0xbd,0xdc,0x3e,0xf9,0x66,0x76,0x0f,0xf6,0xb6,0x76,0x76,0xf7,0x67,0x52,0xa8,0xc6,0x5b,0x20,0x38,0x04,0x87,0xe0,0x10,0x1c,0x82,0x43,0x70,0x08,0x0e,0xc1,0xa1,0xd2,0xdb,0xde,0xe0,0xef,0x3e,0x38,0x3c,0x71,0x00,0xba,0x74,0x7b,0xf9,0x79,0x23,0xbf,0x77,0x2b,0xe5,0xe5,0x73,0x1a,0xf2,0xb2,0x92,0x02,0x07,0x13,0x99,0x2a,0x19,0x17,0x47,0x3b,0x8b,0xbb,0xeb,0x0b,0x07,0xb8,0x4b,0x2f,0xdf,0x1d,0x6d,0x84,0xc8,0x58,0x38,0x96,0xb1,0x00,0x62,0x24,0x28,0x63,0x10,0x89,0x8f,0x23,0xc8,0x82,0x89,0x34,0x4a,0xe2,0x12,0x89,0x8c,0xe3,0x91,0x0c,0x26,0xd2,0x2b,0x89,0xe8,0xa3,0x21,0x23,0xf7,0x1e,0xbd,0xcf,0x11,0xb7,0xff,0x71,0x26,0x07,0x16,0x45,0x8e,0x90,0x86,0x0c,0x23,0x64,0xac,0xc9,0xb1,0xf8,0x0a,0xb0,0x98,0xe0,0x08,0x19,0x32,0x3f,0x1a,0x32,0xca,0x1e,0x21,0x43,0xe6,0x47,0x43,0x06,0x1f,0x91,0x71,0x90,0x51,0x83,0x8f,0x26,0xe2,0x2b,0xd0,0xc4,0xb7,0xa8,0x29,0x70,0x3c,0x30,0x24,0x23,0x23,0x1f,0x6b,0x0c,0x8f,0x66,0x6d,0x19,0xca,0xae,0xbe,0xc7,0xae,0x59,0xfb,0x6f,0x37,0x36,0x8a,0xdf,0x7c,0x34,0xc3,0x7f,0x9f,0x4a,0xdd,0x7c,0xf4,0xc0,0x61,0x41,0xa9,0x6d,0x71,0x69,0x92,0x31,0x54,0x76,0xc3,0xa3,0xe9,0xcb,0x8d,0x8c,0x7a,0xce,0x5c,0xfc,0xd7,0x04,0x0d,0xc3,0x61,0xb7,0x51,0xe7,0xce,0xa3,0xdf,0xe4,0xb0,0xa6,0x54,0xb5,0xf3,0xb0,0xac,0x68,0x00,0x0e,0x6b,0x4a,0xb5,0x2b,0x4b,0x8f,0xc9,0x61,0x4d,0xa9,0x6d,0x65,0xd9,0xae,0xe4,0x1d,0xf9,0xf4,0xf6,0xfb,0xe2,0xc3,0xd3,0x9b,0x39,0x25,0xf6,0x1c,0x01,0x19,0xc1,0x67,0x54,0x23,0x8e,0x65,0x0e,0xf8,0x18,0x8a,0x23,0xf7,0x0b,0xa3,0xed,0x02,0xaa,0xf5,0xd1,0xf1,0x52,0x69,0xc9,0x93,0xa3,0xcb,0xb1,0x37,0x3f,0x6a,0xdf,0x73,0x08,0x0e,0xc1,0x21,0x38,0x04,0x87,0xe0,0x10,0x1c,0x2a,0xa9,0x6d,0x6f,0xc1,0xca,0x7e,0xbd,0xf9,0xb8,0xf8,0x70,0xef,0xeb,0x99,0xc9,0xa1,0x80,0x8c,0xe0,0x33,0x70,0x90,0x51,0x9d,0x0f,0x38,0x7a,0xcb,0xa8,0xc7,0x07,0x1c,0xeb,0x1f,0xfb,0xe2,0x7d,0xc0,0xa1,0x22,0xce,0x56,0x5e,0x1c,0xff,0xf7,0x09,0xea,0xcf,0x73,0xff,0x9a,0xcb,0xe4,0x08,0xc9,0x08,0x3e,0xa3,0x1a,0x71,0x2c,0x73,0xc0,0x47,0xed,0x38,0xda,0x05,0xf0,0x51,0x2f,0x8e,0x2e,0xc7,0x9e,0x0f,0x67,0x2b,0x82,0x43,0x70,0x68,0xfa,0xf9,0x54,0x76,0xdc,0x7e,0x7f,0xd9,0x59,0x7c,0xf8,0xec,0xc3,0x1f,0x93,0x43,0x01,0x19,0xc1,0x67,0xe0,0x20,0x23,0x4b,0x1f,0x70,0x24,0x95,0x91,0x97,0x0f,0x38,0x52,0xcb,0xc8,0xc8,0x07,0x1c,0x82,0x43,0xd3,0x3f,0x95,0x7d,0x7e,0xf5,0xdf,0xa5,0xee,0xfb,0xf7,0x3e,0x76,0x37,0x39,0x42,0x32,0x82,0xcf,0xa8,0xc6,0xc9,0xb1,0xcc,0xc1,0xf3,0xab,0xef,0xe6,0x47,0xb0,0x57,0xdf,0x8e,0x17,0x1f,0xfe,0x78,0x7d,0x5e,0xe6,0xe4,0x68,0x9f,0x10,0xe6,0xc7,0x4a,0x19,0xc1,0x67,0x4a,0xc0,0xd1,0xe5,0xd8,0xf3,0xd1,0xc5,0x41,0x62,0x1f,0xce,0x56,0xb2,0x91,0x91,0xde,0x07,0x1c,0x39,0xc9,0x48,0xec,0x03,0x0e,0xc1,0x21,0x38,0x04,0x87,0xe0,0x10,0x1c,0x82,0x43,0x70,0x08,0x0e,0xc1,0x21,0x38,0x24,0x38,0x04,0x87,0xe0,0x10,0x1c,0x82,0x43,0x70,0x08,0x0e,0xc1,0x21,0x38,0x04,0x87,0x04,0x87,0xe0,0x10,0x1c,0x82,0x43,0x70,0x08,0x0e,0x3d,0xad,0xcb,0x7d,0x16,0x92,0xdd,0x8b,0x01,0x8e,0xcc,0x7c,0xa4,0xbc,0x4b,0x47,0x0a,0x1c,0x5d,0xee,0xcd,0xe2,0xfe,0x2d,0x5d,0x04,0x24,0xbe,0x7f,0x4b,0xa2,0xc9,0xd1,0x7e,0xec,0xc9,0xe8,0xe2,0x20,0xfd,0x9d,0x7d,0xd2,0xdd,0xf6,0xe9,0xfe,0xfd,0x3c,0x78,0x87,0x16,0x32,0xa6,0xb0,0x82,0x6c,0x7e,0xcf,0xf1,0xd4,0x01,0x19,0x53,0x2e,0xf5,0xad,0x26,0x69,0x70,0x2a,0x2b,0x38,0x04,0x87,0x7a,0xd5,0xe5,0x1b,0x77,0xb2,0xf8,0x56,0x1e,0x38,0x36,0xe0,0x23,0x97,0xef,0x6b,0x82,0x23,0xb5,0x8f,0x8c,0xbe,0xc9,0x0b,0x8e,0xa4,0x3e,0xf2,0xfa,0x8e,0x37,0x5f,0x00,0xb8,0xf9,0xfd,0x87,0xc9,0x21,0x38,0x04,0x87,0xe0,0xd8,0x7c,0x3f,0xcf,0xe7,0x51,0x7e,0x46,0x65,0x4e,0x8e,0xf6,0x63,0x4f,0x46,0xed,0xcb,0xca,0x32,0x01,0x64,0xc0,0x11,0x76,0x40,0xc6,0xd8,0xe5,0x74,0x9d,0x83,0x06,0x93,0x43,0x70,0x4c,0xbb,0xbd,0xaf,0x67,0x51,0x7e,0x06,0x8e,0x1a,0x7d,0x14,0x2f,0x03,0x8e,0x35,0x7d,0xd4,0x20,0x03,0x8e,0x75,0x7c,0x54,0x22,0x63,0xe6,0x53,0xd9,0x58,0xfb,0x0f,0x93,0x43,0x70,0x48,0x70,0x08,0x0e,0xc1,0x21,0x38,0x04,0x87,0xe0,0x18,0xb3,0xd3,0x9b,0x79,0x94,0x9f,0x81,0x23,0xdc,0xed,0xe5,0xe7,0x7f,0x7f,0x78,0xf9,0xee,0xa8,0x3c,0x1f,0xd5,0xca,0x78,0x38,0x9a,0x0f,0xc7,0xb7,0xd2,0x65,0x65,0x99,0x00,0x33,0xc3,0x9e,0x23,0xec,0x80,0x8c,0x2e,0xd5,0xf2,0xd9,0x0a,0x0d,0xe3,0x4e,0x8e,0x4c,0xb7,0x1d,0x5a,0xfb,0x38,0xae,0xc6,0xd1,0xbe,0x67,0x51,0xbe,0xad,0x3c,0xb2,0xae,0x73,0x28,0x12,0x0e,0x2b,0x4b,0x3d,0x6b,0x4a,0x57,0x1c,0x56,0x96,0x0a,0xd7,0x14,0xcb,0x8a,0x62,0xe0,0xc8,0xfd,0x52,0xa9,0x66,0x9d,0x2f,0x8c,0x0e,0x9a,0x1c,0x7c,0x14,0xbf,0xdb,0xe8,0x8d,0xc3,0xce,0xa3,0x9e,0xdd,0xc6,0xa0,0x3d,0x87,0xe1,0x51,0xfc,0xd8,0xe8,0x8d,0x63,0x11,0x1d,0x1f,0x39,0xca,0xe8,0x35,0xfe,0x7b,0x4f,0x0e,0x8b,0x4b,0x0d,0x0b,0xca,0xfa,0xcb,0x8a,0x33,0x97,0x82,0xcf,0x50,0x22,0xec,0x39,0x2c,0x2e,0x05,0x6f,0x35,0x86,0xe2,0xb0,0xf9,0x28,0x78,0xab,0x11,0x61,0x72,0xf0,0x51,0xb6,0x8c,0xa1,0xcb,0x0a,0x1f,0x05,0xcb,0x98,0xcd,0x66,0x5b,0x3b,0xbb,0xfb,0x03,0x5f,0xca,0xc1,0xe1,0xc9,0xe2,0xc3,0xbb,0xeb,0x0b,0x87,0x67,0x22,0x9b,0x8c,0x81,0xa7,0x96,0x11,0x3e,0x78,0x7b,0xf4,0x0a,0x8c,0x90,0x32,0x64,0xc4,0x99,0x1c,0x46,0x48,0x79,0x2c,0xa2,0x4d,0x0e,0x23,0xa4,0x48,0x19,0x91,0x27,0x47,0x70,0x7e,0x98,0x22,0x69,0x4c,0x44,0x97,0x31,0x0a,0x8e,0x16,0x22,0x94,0x8c,0x64,0x62,0x36,0xce,0xc7,0x1a,0x63,0xe1,0x68,0x27,0x02,0x4a,0x14,0x10,0xe3,0xb1,0x48,0x81,0xa3,0x0b,0x11,0xc5,0xda,0xe4,0x65,0x89,0x83,0x92,0xbc,0x4c,0x6c,0x06,0x07,0x2b,0x13,0xd7,0x30,0x21,0x1c,0x9a,0x78,0xfe,0x6b,0x82,0xe0,0x10,0x1c,0x82,0x43,0x70,0x08,0x0e,0xc1,0x21,0x38,0x54,0x4a,0x7f,0x01,0x38,0x93,0xa9,0x85,0xd4,0xfb,0xb9,0x0b,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

// =====================================================
// HARDWARE
// =====================================================

// Runtime-configurable LED strip. MAX_LEDS reserves storage for effects.
static const uint16_t MAX_LEDS = 300;
static const uint16_t DEFAULT_NUM_LEDS = 62;
static const uint8_t DEFAULT_LED_PIN = 1;
uint16_t NUM_LEDS = DEFAULT_NUM_LEDS;
uint8_t ledPin = DEFAULT_LED_PIN;
#define BRIGHTNESS 150

#define SAMPLE_RATE 16000
#define SAMPLES 512

Adafruit_NeoPixel strip(
    MAX_LEDS,
    DEFAULT_LED_PIN,
    NEO_GRB + NEO_KHZ800
);

M5Canvas canvas(&M5.Display);

int16_t audio[SAMPLES];
double vReal[SAMPLES];
double vImag[SAMPLES];

ArduinoFFT<double> FFT(
    vReal,
    vImag,
    SAMPLES,
    SAMPLE_RATE
);

// =====================================================
// MODES
// =====================================================

enum LedMode {
    RAINBOW_MUSIC,
    VU_METER,
    COMET_BEAT,
    BPM_AUTO,
    ENERGY_STROBE,
    FIRE_SPARKS
};

volatile LedMode mode = RAINBOW_MUSIC;
// Palette selection shared by the web and LED tasks. Zero retains original colors.
volatile uint8_t effectPalette[6] = {0,0,0,0,0,0};
volatile int8_t temporaryGlobalPalette = -1;
volatile uint32_t paletteRevision = 0;
volatile bool paletteReverse[6] = {false,false,false,false,false,false};
uint8_t activePalette(uint8_t effect) {
    return temporaryGlobalPalette > 0 ? (uint8_t)temporaryGlobalPalette : effectPalette[effect];
}

volatile int requestedMode = -1;
volatile bool requestedColor = false;
volatile bool requestedWaveColor = false;
volatile bool requestedWaveVariant = false;
volatile bool requestedVUVariant = false;
volatile uint8_t waveVariant = 0; // 0 standard, 1 top, 2 bottom, 3 ping pong
volatile uint8_t waveColor = 5; // 0 classic, 1 blue, 2 red, 3 purple, 4 gold, 5 rainbow
volatile bool vuCentered = false;
volatile bool requestedCometColor = false;
volatile bool requestedBPMColor = false;
volatile uint8_t dynamicsPercent = 50;  // 50% = original response
volatile bool manualBPM = false;
volatile float manualBPMValue = 120.0f;
volatile uint8_t bpmColorVariant = 0;  // legacy default coloring
volatile uint8_t bpmDisplayVariant = 0; // 0 blink, 1 hold until next beat
uint32_t tapTimes[6] = {};
uint8_t tapCount = 0;
uint32_t lastTapTime = 0;
const uint32_t MUSIC_SILENCE_MS = 1000;
// Track the quiet microphone floor separately from musical activity.
float musicNoiseFloor = 0.0f;
bool musicGateInitialized = false;
// Energy pad cycles 2 -> 3 -> 4 -> 1 -> 2, remembered across mode changes.
volatile uint8_t energyFlashVariant = 2;
volatile bool requestedEnergyCycle = false;
uint32_t lastMusicActivity = 0;
bool audioIsActive = false;
float dynamicsFactor() { return constrain(1.0f + ((int)dynamicsPercent - 50) / 100.0f, 0.5f, 1.5f); }

volatile uint8_t requestedBrightness = BRIGHTNESS;
uint8_t currentBrightness = BRIGHTNESS;

const char* modeNames[] = {
    "WAVE",
    "VU METER",
    "COMET BEAT",
    "BPM AUTO",
    "ENERGY STROBE",
    "FIRE & SPARKS"
};

// =====================================================
// GENERAL
// =====================================================

int screenW = 0;
int screenH = 0;

uint32_t lastScreen = 0;
uint32_t lastAudioUpdate = 0;

float rms = 0.0f;
float signalRms = 0.0f;
float adjustedRms = 0.0f;
float vu = 0.0f;

float rainbowPhase = 0.0f;

// =====================================================
// AGC
// =====================================================

const float RMS_NOISE_FLOOR = 500.0f;
const float AGC_ACTIVITY_THRESHOLD = 250.0f;
const float AGC_TARGET_RMS = 3000.0f;

const float AGC_MIN_GAIN = 0.25f;
const float AGC_MAX_GAIN = 8.0f;

const float AGC_FAST_TIME = 2.0f;
const float AGC_SLOW_TIME = 8.0f;

const float VU_ATTACK_TIME = 0.015f;
const float VU_RELEASE_TIME = 0.035f;

const float VU_MIN_DB = -21.0f;
const float VU_MAX_DB = 21.0f;

const float VU_DYNAMIC_GAIN = 1.8f;
const float VU_DISPLAY_OFFSET = 0.58f;

float agcGain = 1.0f;
float agcAverage = 0.0f;
bool agcInitialized = false;

// =====================================================
// VU COLORS
// =====================================================

enum VUColorMode {
    VU_AUTO,
    VU_BLUE,
    VU_RED,
    VU_PURPLE,
    VU_GOLD,
    VU_RAINBOW,
    VU_CLASSIC
};

VUColorMode vuColorMode = VU_CLASSIC;

const int VU_COLOR_MODE_COUNT = 7;

void nextVUColor() {
    switch (vuColorMode) {
        case VU_CLASSIC: vuColorMode = VU_BLUE; break;
        case VU_BLUE: vuColorMode = VU_RED; break;
        case VU_RED: vuColorMode = VU_PURPLE; break;
        case VU_PURPLE: vuColorMode = VU_GOLD; break;
        case VU_GOLD: vuColorMode = VU_RAINBOW; break;
        default: vuColorMode = VU_CLASSIC; break;
    }
}

int autoColorIndex = 0;

const char* vuColorNames[] = {
    "AUTO",
    "BLUE",
    "RED",
    "PURPLE",
    "GOLD",
    "RAINBOW",
    "CLASSIC VU"
};

const char* autoColorNames[] = {
    "BLUE",
    "RED",
    "PURPLE",
    "GOLD",
    "RAINBOW",
    "CLASSIC VU"
};

const uint32_t AUTO_SILENCE_MS = 1000;
const float AUTO_SILENCE_THRESHOLD = 300.0f;

uint32_t silenceStart = 0;
bool silenceTriggered = false;

// COMET: color selection is independent of VU colors.
enum CometColor { COMET_CYAN, COMET_BLUE, COMET_PURPLE, COMET_RED, COMET_GOLD, COMET_RAINBOW };
volatile CometColor cometColor = COMET_CYAN;
void nextCometColor() { cometColor = (CometColor)(((int)cometColor + 1) % 6); }

// =====================================================
// VU PEAK
// =====================================================

float vuPeak = 0.0f;
uint32_t lastPeakUpdate = 0;
uint32_t vuPeakHoldUntil = 0;

const float PEAK_FALL_SPEED =
    12.0f / NUM_LEDS;

// =====================================================
// FFT DISPLAY
// =====================================================

const float noiseGate[3] = {
    35.0f, 15.0f, 45.0f
};

const float maxLevel[3] = {
    2500.0f, 260.0f, 230.0f
};

const int starts[3] = {0, 24, 48};
const int counts[3] = {24, 24, 20};

float levels[3] = {0, 0, 0};

// =====================================================
// ENERGY STROBE - SAME DETECTION SETTINGS
// =====================================================

const uint32_t ENERGY_FLASH_MS = 20;
const uint32_t ENERGY_SECOND_FLASH_DELAY_MS = 90;
const uint32_t ENERGY_COOLDOWN_MS = 260;

const float ENERGY_TRIGGER_RATIO = 1.20f;
const float ENERGY_MIN_RISE = 0.030f;
const float ENERGY_MIN_RMS = 650.0f;
const float ENERGY_MIN_LEVEL = 0.035f;
const float ENERGY_AVERAGE_TIME = 0.55f;

float energyStrobeValue = 0.0f;
float energyStrobeAverage = 0.0f;
float energyStrobePrevious = 0.0f;
float energyStrobeRise = 0.0f;

bool energyInitialized = false;

uint32_t lastEnergyTrigger = 0;
uint32_t energyTriggerCount = 0;

// =====================================================
// BPM AUTO - NEW DETECTOR
// =====================================================

// Supported music tempo
const float BPM_MIN = 90.0f;
const float BPM_MAX = 170.0f;

const float BPM_MIN_INTERVAL_MS =
    60000.0f / BPM_MAX;

const float BPM_MAX_INTERVAL_MS =
    60000.0f / BPM_MIN;

// Detection
const float BPM_BASS_MIN_HZ = 50.0f;
const float BPM_BASS_MAX_HZ = 250.0f;

const float BPM_NOISE_GATE = 0.018f;
const float BPM_MIN_RMS = 420.0f;

const float BPM_PEAK_RATIO = 1.09f;
const float BPM_PEAK_PROMINENCE = 0.010f;

const uint32_t BPM_REFRACTORY_MS = 250;

// Baseline adapts slowly
const float BPM_BASELINE_TIME = 0.65f;

// One white flash per beat
const uint32_t BPM_FLASH_MS = 25;

// Keep clock running after missed detections
const uint32_t BPM_HOLD_MS = 30000;

// Detector values
float bpmBass = 0.0f;
float bpmBaseline = 0.0f;

float bpmPrevious2 = 0.0f;
float bpmPrevious1 = 0.0f;

uint32_t bpmPrevious1Time = 0;

bool bpmDetectorInitialized = false;

uint32_t lastBPMOnset = 0;
uint32_t bpmDetectedBeats = 0;

// Candidate intervals
const int BPM_HISTORY_SIZE = 8;

float bpmIntervals[BPM_HISTORY_SIZE];
int bpmHistoryCount = 0;
int bpmHistoryIndex = 0;

// Clock
float estimatedBPM = 0.0f;
float beatIntervalMs = 0.0f;

bool bpmLocked = false;
uint8_t bpmConfidence = 0;

uint32_t lastBPMEvidence = 0;
uint32_t nextBPMFlash = 0;

uint32_t bpmFlashCount = 0;

// Comet Beat: multiple independent comets, fixed travel speed at 120 BPM.
// Tail shortens with BPM: 60=>28, 90=>24, 120=>20, 150=>16, 180=>12.
static const int COMET_MAX_TAIL = 32;
static const int COMET_MAX_ACTIVE = 8;
uint32_t cometStarts[COMET_MAX_ACTIVE] = {0};
uint8_t cometTails[COMET_MAX_ACTIVE] = {0};
uint8_t cometWriteIndex = 0;
uint32_t cometLastBeat = 0;

void resetComets() {
    for (int n = 0; n < COMET_MAX_ACTIVE; n++) { cometStarts[n] = 0; cometTails[n] = 0; }
    cometWriteIndex = 0;
    cometLastBeat = bpmFlashCount;
}


// MusicLED 2.0: random-play release; particle diffusion, gravity heights and fireworks variation tuned.
volatile uint8_t particleVariant=0; // 0 explosion, 1 diffusion, 2 breathe, 3 streamer, 4 balls, 5 fireworks
volatile uint8_t particleColor=5; // classic, blue, red, purple, gold, rainbow
volatile bool requestedParticleVariant=false;
volatile bool requestedParticleColor=false;
struct Particle {float pos, vel, life; uint32_t start; uint8_t type; uint16_t hue; bool active; float origin, fadeRate;};
static Particle particles[72];
static uint32_t particleBeat=0, particleLastFrame=0;
volatile uint32_t particleOnsetCount=0; // Raw detected bass peaks for responsive center effects
static float particleGlow=0;
static uint8_t particleNext=0;
static uint32_t particleLastLaunch=0;
static uint8_t particleBurst=0;
struct FireworkRocket {float pos, vel, power; uint16_t hue; bool active;};
static FireworkRocket fireworkRockets[3] = {};
static uint32_t fireworkBassSerial=0;
static uint32_t fireworkLastLaunch=0;
uint8_t paletteStopCount(uint8_t id);
uint32_t paletteStopColor(uint8_t id,uint32_t beat);
uint32_t paletteColorAt(uint8_t id, uint16_t position);
uint32_t saturatedPaletteColor(uint32_t color,uint8_t id);
uint32_t fireworkFragmentColor(uint16_t hue,uint8_t brightness) {
  uint8_t id=activePalette(COMET_BEAT);
  uint32_t c=id ? paletteStopColor(id,(uint32_t)hue*paletteStopCount(id)/65536UL)
                : strip.ColorHSV(hue,255,255);
  // Preserve the exact palette stop hue; remove its shared white component
  // for chromatic colors so explosion fragments never look pastel/washed out.
  uint8_t r=(c>>16)&255,g=(c>>8)&255,b=c&255;
  uint8_t hi=max((int)r,max((int)g,(int)b));
  uint8_t lo=min((int)r,min((int)g,(int)b));
  if(hi>lo && lo>0){
    r=(uint16_t)(r-lo)*hi/(hi-lo);
    g=(uint16_t)(g-lo)*hi/(hi-lo);
    b=(uint16_t)(b-lo)*hi/(hi-lo);
    c=strip.Color(r,g,b);
  }
  return strip.Color(((c>>16)&255)*brightness/255,((c>>8)&255)*brightness/255,(c&255)*brightness/255);
}
static float fireworkPower=0.0f; // Captured from music strength at rocket launch

void resetParticles(){
  for(auto &p:particles)p.active=false;
  particleBeat=(particleVariant==0||particleVariant==1||particleVariant==5)?particleOnsetCount:bpmFlashCount;
  particleLastFrame=millis();particleGlow=0;
  particleLastLaunch=0;particleBurst=0;fireworkPower=0.0f;
  fireworkBassSerial=0;fireworkLastLaunch=0;
  for(auto &rocket:fireworkRockets)rocket.active=false;
}
uint32_t particleRGB(uint16_t hue, uint8_t strength){
  uint8_t r=0,g=0,b=0;
  switch(particleColor){
    case 0:r=strength;g=(uint8_t)(strength*.85f);b=(uint8_t)(strength*.45f);break;
    case 1:b=strength;g=(uint8_t)(strength*.35f);break;
    case 2:r=strength;break;
    case 3:r=(uint8_t)(strength*.7f);b=strength;break;
    case 4:r=strength;g=(uint8_t)(strength*.65f);break;
    default:return strip.gamma32(strip.ColorHSV(hue,255,strength));
  }
  return strip.Color(r,g,b);
}
void particleAdd(int i,uint32_t color){
 if(i<0||i>=NUM_LEDS)return;
 uint32_t old=strip.getPixelColor(i);
 uint8_t r=min(255,(int)((old>>16)&255)+(int)((color>>16)&255));
 uint8_t g=min(255,(int)((old>>8)&255)+(int)((color>>8)&255));
 uint8_t b=min(255,(int)(old&255)+(int)(color&255));
 strip.setPixelColor(i,strip.Color(r,g,b));
}
// Keep the brightest palette fragment at a pixel rather than adding RGB
// channels from overlapping fragments (which creates unwanted white).
void particlePaintExclusive(int i,uint32_t color){
 if(i<0||i>=NUM_LEDS)return;
 uint32_t old=strip.getPixelColor(i);
 uint8_t oldMax=max((int)((old>>16)&255),max((int)((old>>8)&255),(int)(old&255)));
 uint8_t newMax=max((int)((color>>16)&255),max((int)((color>>8)&255),(int)(color&255)));
 if(newMax>=oldMax)strip.setPixelColor(i,color);
}
void particleSpawn(float pos,float vel,float life,uint8_t type,uint16_t hue){
 Particle &p=particles[particleNext++%72];
 p={pos,vel,life,millis(),type,hue,true,pos,0.72f};
}
void updateParticles(){
 uint32_t now=millis();float dt=min(0.06f,(now-particleLastFrame)*.001f);particleLastFrame=now;
 strip.clear();
 uint32_t beatSource=(particleVariant==0||particleVariant==1||particleVariant==5) ? particleOnsetCount : bpmFlashCount;
 uint32_t beats=beatSource-particleBeat;particleBeat=beatSource;
 if(beats>3)beats=3;
 bool beat=beats>0 && audioIsActive;
 if(beat){
   particleGlow=1.0f;
   if(particleVariant==0||particleVariant==1){
     for(uint32_t k=0;k<beats;k++)particleSpawn((NUM_LEDS-1)*.5f,0,1.0f,particleVariant,(uint16_t)random(65536));
   }else if(particleVariant==2){ /* Breathe uses a smoothed envelope below. */ }
   else if(particleVariant==3){
     for(int k=0;k<3;k++)particleSpawn(random(NUM_LEDS),((random(2)?1:-1)*(8+random(24))),1.0f,3,(uint16_t)random(65536));
   }else if(particleVariant==4){
     // Bottom is always LED 0; initial upward speed follows beat strength.
     // Select a fresh apex between 30% and 100% of the available strip.
     // v=sqrt(2*g*h) ensures that every height is physically reachable.
     const float heightFraction=random(30,101)*.01f;
     const float targetHeight=(NUM_LEDS-1)*heightFraction;
     const float v=sqrtf(2.0f*55.0f*targetHeight);
     particleSpawn(0,v,1.0f,4,(uint16_t)random(65536));
   }else if(particleVariant==5){
     // Launch on alternate detected bass onsets, no faster than 300 ms.
     // Independent rocket slots allow up to three rockets simultaneously.
     for(uint32_t k=0;k<beats;k++){
       ++fireworkBassSerial;
       if((fireworkBassSerial&1U)!=0 || now-fireworkLastLaunch<300)continue;
       for(auto &rocket:fireworkRockets){
         if(rocket.active)continue;
         rocket.active=true;rocket.pos=0;
         rocket.power=constrain(vu,0.0f,1.0f);
         rocket.hue=(uint16_t)random(65536);
         const float height=(NUM_LEDS-1)*(random(25,61)*.01f);
         rocket.vel=sqrtf(2.0f*55.0f*height);
         fireworkLastLaunch=now;
         break;
       }
     }
   }
 }
 particleGlow*=powf(.18f,dt);
 if(particleVariant==2){
   // Audio amplitude supplies a slow base glow; beats add a visible breath.
   float base=audioIsActive?.16f:0.02f;
   uint8_t v=(uint8_t)(255.0f*min(1.0f,base+particleGlow*.84f));
   for(int i=0;i<NUM_LEDS;i++)strip.setPixelColor(i,particleRGB((uint16_t)(i*65535UL/max(1,(int)NUM_LEDS)),v));
 }
 if(particleVariant==3 && audioIsActive && now-particleLastLaunch>200){
   particleLastLaunch=now;
   particleSpawn(random(NUM_LEDS),((random(2)?1:-1)*(5+random(14))),1.0f,3,(uint16_t)random(65536));
 }
 if(particleVariant==5){
   for(auto &rocket:fireworkRockets){
     if(!rocket.active)continue;
     rocket.pos+=rocket.vel*dt;rocket.vel-=55.0f*dt;
     particlePaintExclusive((int)roundf(rocket.pos),strip.Color(255,255,255));
     if(rocket.vel<=0 || rocket.pos>=NUM_LEDS-2){
       rocket.active=false;
       const float origin=constrain(rocket.pos,0.0f,(float)(NUM_LEDS-1));
       const float power=rocket.power;
       const int fragments=12+(int)(power*8.0f);
       const float spread=0.65f+random(75)*.01f;
       for(int k=0;k<fragments;k+=2){
         const float velocity=12.0f+random(12,28)*spread;
         const float life=0.90f+power*0.22f+random(10)*.01f;
         const uint16_t hue=(uint16_t)(rocket.hue+random(65536));
         particleSpawn(origin,-velocity,life,5,hue);
         Particle &left=particles[(particleNext+71)%72];
         left.fadeRate=2.0f*(0.85f-0.20f*power);
         if(k+1<fragments){
           particleSpawn(origin,velocity,life,5,hue);
           Particle &right=particles[(particleNext+71)%72];
           right.fadeRate=left.fadeRate;
         }
       }
     }
   }
 }
 for(auto &p:particles){
   if(!p.active)continue;
   float age=(now-p.start)*.001f;
   if(p.type==0||p.type==1){
     float speed=p.type==0?70.0f:58.0f;
     float radius=age*speed;
     float fade=max(0.0f,1.0f-age/(p.type==0?0.85f:1.45f));
     if(fade<=0){p.active=false;continue;}
     if(p.type==0){
       for(int side=-1;side<=1;side+=2)for(int w=-3;w<=3;w++){
         int idx=(int)roundf(p.pos+side*radius+w);
         float edge=1.0f-abs(w)*.21f;
         particleAdd(idx,particleRGB(p.hue,(uint8_t)(235*fade*edge)));
       }
     }else{
       // Diffusion is a growing, soft cloud rather than a sharp traveling ring.
       for(int i=0;i<NUM_LEDS;i++){
         float d=fabsf(i-p.pos);
         float width=5.0f+radius;
         if(d<width){ const float envelope=0.30f+0.70f*sqrtf(max(0.0f,1.0f-d/width)); const float intensity=min(255.0f,245.0f*fade*envelope); particleAdd(i,particleRGB(p.hue,(uint8_t)intensity)); }
       }
     }
   }else if(p.type==3){
     p.pos+=p.vel*dt;
     if(p.pos<0||p.pos>=NUM_LEDS){p.active=false;continue;}
     p.life-=dt*.7f;
     if(p.life<=0){p.active=false;continue;}
     for(int k=0;k<4;k++)particleAdd((int)(p.pos-k*(p.vel>0?1:-1)),particleRGB(p.hue,(uint8_t)(255*p.life*(1.0f-.22f*k))));
   }else if(p.type==4||p.type==5){
     // Gravity, in LED units per second squared. Balls bounce with energy loss.
     p.pos+=p.vel*dt;
     if(p.type==4)p.vel-=55.0f*dt; // Firework fragments travel evenly outward.
     if(p.type==5 && (p.pos<0 || p.pos>=NUM_LEDS)){p.active=false;continue;}
     if(p.type==4){
       if(p.pos>=NUM_LEDS-1){p.pos=NUM_LEDS-1;p.vel=-fabsf(p.vel)*.7f;}
       if(p.pos<=0){
         p.pos=0;
         if(fabsf(p.vel)>9)p.vel=-p.vel*.63f;
         else {p.active=false;continue;}
       }
     }
     if(p.type==5){
       // Fragments travel at most 20-30 LEDs from the burst, then disappear.
       const float distance=fabsf(p.pos-p.origin);
       if(distance>25.0f){p.active=false;continue;}
       p.life-=dt*p.fadeRate;
       if(p.life<=0){p.active=false;continue;}
     }
     float strength=p.type==5 ? constrain(p.life,0.0f,1.0f)*constrain((25.0f-fabsf(p.pos-p.origin))/12.0f,0.0f,1.0f) : 1.0f;
     if(p.type==5 && strength<0.025f){p.active=false;continue;}
     uint8_t v=(uint8_t)(255.0f*strength);
     if(p.type==5){
       particlePaintExclusive((int)roundf(p.pos),fireworkFragmentColor(p.hue,v));
       int tail=(int)roundf(p.pos-p.vel*.03f);
       particlePaintExclusive(tail,fireworkFragmentColor(p.hue,v/2));
       particlePaintExclusive((int)roundf(p.pos-p.vel*.06f),fireworkFragmentColor(p.hue,v/4));
     }else particleAdd((int)roundf(p.pos),particleRGB(p.hue,v));
   }
 }
}
void updateComet() {
    const uint32_t now = millis();

    // BPM counter is incremented by the audio/clock loop.
    // Each increment creates a separate comet; no restart of older comets.
    if (audioIsActive && cometLastBeat != bpmFlashCount) {
        uint32_t beats = bpmFlashCount - cometLastBeat;
        cometLastBeat = bpmFlashCount;
        if (beats > COMET_MAX_ACTIVE) beats = COMET_MAX_ACTIVE;
        float tempo = manualBPM ? manualBPMValue : (estimatedBPM > 0 ? estimatedBPM : 120.0f);
        // Pronounced tail difference: 100 BPM ~27 LEDs, 140 BPM ~13 LEDs.
        uint8_t tail = (uint8_t)constrain((int)roundf(62.0f - 0.35f * tempo), 9, 32);
        for (uint32_t n = 0; n < beats; n++) {
            cometTails[cometWriteIndex] = tail;
            cometStarts[cometWriteIndex] = now ? now : 1;
            cometWriteIndex = (cometWriteIndex + 1) % COMET_MAX_ACTIVE;
        }
    }

    // At 120 BPM, one beat is 500 ms; the head advances 34 LEDs
    // during that time, so the previous comet is about halfway
    // when the next launches. Use 500 ms until tempo is established.
    const float ledsPerMs = (NUM_LEDS * 0.5f) / 500.0f; // fixed at 120 BPM speed
    uint8_t red[MAX_LEDS] = {0};
    uint8_t green[MAX_LEDS] = {0};
    uint8_t blue[MAX_LEDS] = {0};

    for (int n = 0; n < COMET_MAX_ACTIVE; n++) {
        if (!cometStarts[n]) continue;
        int tailLength = cometTails[n] ? cometTails[n] : 18;
        float head = (now - cometStarts[n]) * ledsPerMs;
        if (head >= NUM_LEDS + COMET_MAX_TAIL) {
            cometStarts[n] = 0;
            continue;
        }
        for (int i = 0; i < NUM_LEDS; i++) {
            float distance = head - i;
            if (distance < 0 || distance >= tailLength) continue;

            // Bright head, smooth fading tail, no sudden LED jumps.
            float intensity = 1.0f - distance / tailLength;
            intensity *= intensity;
            uint8_t v = (uint8_t)(255.0f * intensity);
            uint8_t r=0, g=0, b=0;
            switch (cometColor) {
                case COMET_BLUE: b=v; break;
                case COMET_PURPLE: r=(uint8_t)(v*.65f); b=v; break;
                case COMET_RED: r=v; break;
                case COMET_GOLD: r=v; g=(uint8_t)(v*.60f); break;
                case COMET_RAINBOW: {
                    uint32_t c = strip.gamma32(strip.ColorHSV((uint16_t)(i*65535L/NUM_LEDS)));
                    r=(uint8_t)(((c>>16)&255)*intensity);
                    g=(uint8_t)(((c>>8)&255)*intensity);
                    b=(uint8_t)((c&255)*intensity);
                    break;
                }
                default: g=(uint8_t)(v*.85f); b=v; break;
            }
            // Saturating addition keeps overlapping comet tails visible.
            red[i] = (uint8_t)min(255, (int)red[i] + (int)r);
            green[i] = (uint8_t)min(255, (int)green[i] + (int)g);
            blue[i] = (uint8_t)min(255, (int)blue[i] + (int)b);
        }
    }

    strip.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(red[i], green[i], blue[i]));
    }
}

// =====================================================
// LED TASK AND COMMANDS
// =====================================================

enum FlashCommand {
    FLASH_NONE,
    FLASH_SINGLE,
    FLASH_DOUBLE
};

portMUX_TYPE ledMux = portMUX_INITIALIZER_UNLOCKED;

volatile FlashCommand flashCommand = FLASH_NONE;
volatile uint32_t flashCommandTime = 0;

volatile uint32_t flashStart[4] = {0, 0, 0, 0};
volatile uint32_t flashEnd[4] = {0, 0, 0, 0};

volatile uint8_t flashCount = 0;

volatile bool ledTaskEnabled = true;

TaskHandle_t ledTaskHandle = nullptr;

// =====================================================
// HELPERS
// =====================================================

float filterAlpha(float dt, float seconds) {
    if (seconds <= 0.0f) {
        return 1.0f;
    }

    return 1.0f - expf(-dt / seconds);
}

bool timeReached(
    uint32_t now,
    uint32_t target
) {
    return (int32_t)(now - target) >= 0;
}

bool timeInWindow(
    uint32_t now,
    uint32_t start,
    uint32_t end
) {
    return
        (int32_t)(now - start) >= 0 &&
        (int32_t)(end - now) > 0;
}

float rmsToLogLevel(float value) {
    if (value <= 0.0f) {
        return 0.0f;
    }

    float db =
        20.0f * log10f(
            value / AGC_TARGET_RMS
        );

    return constrain(
        (db - VU_MIN_DB) /
        (VU_MAX_DB - VU_MIN_DB),
        0.0f,
        1.0f
    );
}

float expandVU(float level) {
    if (level <= 0.001f) {
        return 0.0f;
    }

    return constrain(
        VU_DISPLAY_OFFSET +
        (level - 0.5f) *
        VU_DYNAMIC_GAIN,
        0.0f,
        1.0f
    );
}

// =====================================================
// AGC
// =====================================================

void updateAGC(float measuredRms, float dt) {
    signalRms = max(
        0.0f,
        measuredRms - RMS_NOISE_FLOOR
    );

    bool active =
        signalRms > AGC_ACTIVITY_THRESHOLD;

    if (active) {
        if (!agcInitialized) {
            agcAverage = signalRms;
            agcInitialized = true;
        }

        float averageTime =
            signalRms > agcAverage
            ? AGC_FAST_TIME
            : AGC_SLOW_TIME;

        agcAverage +=
            (signalRms - agcAverage) *
            filterAlpha(dt, averageTime);

        float desiredGain = constrain(
            AGC_TARGET_RMS /
            max(agcAverage, 1.0f),
            AGC_MIN_GAIN,
            AGC_MAX_GAIN
        );

        float gainTime =
            desiredGain < agcGain
            ? AGC_FAST_TIME
            : AGC_SLOW_TIME;

        agcGain +=
            (desiredGain - agcGain) *
            filterAlpha(dt, gainTime);
    }

    adjustedRms = signalRms * agcGain;

    float target = active
        ? rmsToLogLevel(adjustedRms)
        : 0.0f;

    float vuTime =
        target > vu
        ? VU_ATTACK_TIME
        : VU_RELEASE_TIME;

    vu += (target - vu) *
        filterAlpha(dt, vuTime);

    vu = constrain(
        vu, 0.0f, 1.0f
    );
}

// =====================================================
// VU COLOR
// =====================================================

void updateAutoColor() {
    if (mode != VU_METER ||
        vuColorMode != VU_AUTO) {

        silenceStart = 0;
        silenceTriggered = false;
        return;
    }

    uint32_t now = millis();

    bool silent =
        signalRms < AUTO_SILENCE_THRESHOLD;

    if (!silent) {
        silenceStart = 0;
        silenceTriggered = false;
        return;
    }

    if (silenceStart == 0) {
        silenceStart = now;
    }

    if (!silenceTriggered &&
        now - silenceStart >=
        AUTO_SILENCE_MS) {

        autoColorIndex =
            (autoColorIndex + 1) % 6;

        silenceTriggered = true;
    }
}

int activeColorIndex() {
    if (vuColorMode == VU_AUTO) {
        return autoColorIndex;
    }

    return (int)vuColorMode - 1;
}

uint32_t vuColorAt(int ledIndex) {
    switch (activeColorIndex()) {
        case 0:
            return strip.Color(0, 100, 255);

        case 1:
            return strip.Color(255, 0, 0);

        case 2:
            return strip.Color(160, 0, 255);

        case 3:
            return strip.Color(255, 175, 0);

        case 4: {
            uint16_t hue = (uint16_t)(
                (uint32_t)ledIndex *
                65535 /
                max(1, (int)NUM_LEDS - 1)
            );

            return strip.gamma32(
                strip.ColorHSV(hue)
            );
        }

        case 5:
            if (
                ledIndex <
                NUM_LEDS * 0.50f
            ) {
                return strip.Color(
                    0, 255, 0
                );
            }
            else if (
                ledIndex <
                NUM_LEDS * 0.70f
            ) {
                return strip.Color(
                    255, 200, 0
                );
            }
            return strip.Color(
                255, 0, 0
            );
    }

    return strip.Color(
        255, 255, 255
    );
}

uint16_t vuScreenColorAt(int ledIndex) {
    uint32_t rgb =
        vuColorAt(ledIndex);

    return canvas.color565(
        (rgb >> 16) & 255,
        (rgb >> 8) & 255,
        rgb & 255
    );
}

// =====================================================
// VU PEAK
// =====================================================

void updatePeak() {
    uint32_t now = millis();

    float dt =
        lastPeakUpdate == 0
        ? 0.0f
        : (now - lastPeakUpdate) /
          1000.0f;

    lastPeakUpdate = now;

    if (vu > vuPeak) {
        vuPeak = vu;
        vuPeakHoldUntil = now + 350; // Hold peak visibly before its original falling speed.
    }
    else if ((int32_t)(now - vuPeakHoldUntil) >= 0) {
        vuPeak -=
            PEAK_FALL_SPEED * dt;

        vuPeak = max(
            vuPeak, vu
        );
    }

    vuPeak = constrain(
        vuPeak, 0.0f, 1.0f
    );
}

// =====================================================
// LED FLASH SCHEDULER
// =====================================================

void clearFlashSchedule() {
    portENTER_CRITICAL(&ledMux);

    flashCount = 0;
    flashCommand = FLASH_NONE;

    portEXIT_CRITICAL(&ledMux);
}

void requestFlash(
    FlashCommand command
) {
    portENTER_CRITICAL(&ledMux);

    flashCommand = command;
    flashCommandTime = millis();

    portEXIT_CRITICAL(&ledMux);
}

// =====================================================
// BPM INTERVAL HELPERS
// =====================================================

float normalizeBeatInterval(float interval) {
    // An interval may represent multiple
    // beats if the detector missed some.
    for (int divisor = 1; divisor <= 4; divisor++) {
        float candidate =
            interval / divisor;

        if (
            candidate >= BPM_MIN_INTERVAL_MS &&
            candidate <= BPM_MAX_INTERVAL_MS
        ) {
            if (beatIntervalMs > 0.0f) {
                float error = fabsf(
                    candidate -
                    beatIntervalMs
                );

                if (
                    error <
                    beatIntervalMs * 0.20f
                ) {
                    return candidate;
                }
            }
        }
    }

    // Without a reference, prefer
    // plausible dance tempo.
    float best = 0.0f;
    float bestError = 1e9f;

    const float preferredInterval =
        60000.0f / 125.0f;

    for (int divisor = 1; divisor <= 4; divisor++) {
        float candidate =
            interval / divisor;

        if (
            candidate <
                BPM_MIN_INTERVAL_MS ||
            candidate >
                BPM_MAX_INTERVAL_MS
        ) {
            continue;
        }

        float error = fabsf(
            candidate -
            preferredInterval
        );

        if (error < bestError) {
            bestError = error;
            best = candidate;
        }
    }

    return best;
}

void addBPMInterval(float interval) {
    bpmIntervals[bpmHistoryIndex] =
        interval;

    bpmHistoryIndex =
        (bpmHistoryIndex + 1) %
        BPM_HISTORY_SIZE;

    if (
        bpmHistoryCount <
        BPM_HISTORY_SIZE
    ) {
        bpmHistoryCount++;
    }
}

float medianBPMInterval() {
    if (bpmHistoryCount == 0) {
        return 0.0f;
    }

    float sorted[BPM_HISTORY_SIZE];

    for (
        int i = 0;
        i < bpmHistoryCount;
        i++
    ) {
        sorted[i] = bpmIntervals[i];
    }

    for (
        int i = 1;
        i < bpmHistoryCount;
        i++
    ) {
        float value = sorted[i];
        int j = i - 1;

        while (
            j >= 0 &&
            sorted[j] > value
        ) {
            sorted[j + 1] =
                sorted[j];

            j--;
        }

        sorted[j + 1] = value;
    }

    return sorted[
        bpmHistoryCount / 2
    ];
}

// =====================================================
// BPM CLOCK AND ONSETS
// =====================================================

void acceptBPMOnset(uint32_t onsetTime) {
    if (!audioIsActive) return;
    bpmDetectedBeats++;
    if (mode == COMET_BEAT) particleOnsetCount++; // Every detected peak triggers explosion/diffusion.

    if (lastBPMOnset != 0) {
        float rawInterval =
            onsetTime - lastBPMOnset;

        float normalized =
            normalizeBeatInterval(
                rawInterval
            );

        if (normalized > 0.0f) {
            addBPMInterval(normalized);

            float median =
                medianBPMInterval();

            if (median > 0.0f) {
                if (beatIntervalMs <= 0.0f) {
                    beatIntervalMs = median;
                }
                else {
                    // Slow tempo adjustment
                    beatIntervalMs =
                        beatIntervalMs *
                        0.88f +
                        median * 0.12f;
                }

                estimatedBPM =
                    60000.0f /
                    beatIntervalMs;

                if (bpmConfidence < 10) {
                    bpmConfidence++;
                }

                if (
                    bpmHistoryCount >= 4 &&
                    bpmConfidence >= 4
                ) {
                    if (!bpmLocked) {
                        bpmLocked = true;

                        nextBPMFlash =
                            onsetTime +
                            (uint32_t)
                            beatIntervalMs;
                    }
                }
            }
        }
    }

    lastBPMOnset = onsetTime;
    lastBPMEvidence = onsetTime;

    if (!bpmLocked) {
        if (audioIsActive) {
            if (mode == COMET_BEAT || (mode == RAINBOW_MUSIC && waveVariant != 0)) bpmFlashCount++;
            requestFlash(FLASH_SINGLE);
        }
        return;
    }

    // Gently correct phase, never restart
    // the clock at every detected onset.
    if (beatIntervalMs > 0.0f) {
        uint32_t previousPredicted =
            nextBPMFlash -
            (uint32_t)beatIntervalMs;

        int32_t phaseError =
            (int32_t)(
                onsetTime -
                previousPredicted
            );

        float halfInterval =
            beatIntervalMs * 0.5f;

        if (
            fabsf((float)phaseError) <
            halfInterval
        ) {
            int32_t correction =
                (int32_t)(
                    phaseError * 0.15f
                );

            nextBPMFlash += correction;
        }
    }
}

// =====================================================
// BPM DETECTION: LOCAL BASS PEAKS
// =====================================================

void updateBPMDetector(
    float bass,
    float dt
) {
    uint32_t now = millis();

    bpmBass = bass;

    if (!bpmDetectorInitialized) {
        bpmBaseline = bass;
        bpmPrevious2 = bass;
        bpmPrevious1 = bass;
        bpmPrevious1Time = now;

        bpmDetectorInitialized = true;
        return;
    }

    // A local maximum is confirmed when
    // the next sample starts falling.
    bool localPeak =
        bpmPrevious1 > bpmPrevious2 &&
        bpmPrevious1 >= bass;

    float prominence =
        bpmPrevious1 -
        bpmBaseline;

    bool strongEnough =
        bpmPrevious1 >
        BPM_NOISE_GATE &&
        bpmPrevious1 >
        bpmBaseline *
        BPM_PEAK_RATIO &&
        prominence >
        BPM_PEAK_PROMINENCE &&
        rms > BPM_MIN_RMS;

    bool cooldownFinished =
        bpmPrevious1Time -
        lastBPMOnset >=
        BPM_REFRACTORY_MS;

    if (
        audioIsActive &&
        localPeak &&
        strongEnough &&
        cooldownFinished
    ) {
        acceptBPMOnset(
            bpmPrevious1Time
        );
    }

    bpmBaseline +=
        (bass - bpmBaseline) *
        filterAlpha(
            dt,
            BPM_BASELINE_TIME
        );

    bpmPrevious2 = bpmPrevious1;
    bpmPrevious1 = bass;
    bpmPrevious1Time = now;
}

// =====================================================
// BPM CLOCK
// =====================================================

void updateBPMClock() {
    if (mode != BPM_AUTO && mode != COMET_BEAT && !(mode == RAINBOW_MUSIC && waveVariant != 0)) return;
    uint32_t now = millis();
    // Silence gates beat output even when using manual tempo.
    if (!audioIsActive) {
        nextBPMFlash = 0; // do not accumulate missed beats
        return;
    }
    float interval = manualBPM ? 60000.0f / manualBPMValue : beatIntervalMs;
    if ((!manualBPM && !bpmLocked) || interval <= 0.0f) return;
    if (nextBPMFlash == 0) nextBPMFlash = now + (uint32_t)interval;   

    if (
        nextBPMFlash != 0 &&
        timeReached(
            now,
            nextBPMFlash
        )
    ) {
        requestFlash(FLASH_SINGLE);

        bpmFlashCount++;

        nextBPMFlash +=
            (uint32_t)interval;

        // Skip overdue ticks, never
        // generate catch-up flashes.
        if (
            timeReached(
                now,
                nextBPMFlash
            )
        ) {
            nextBPMFlash =
                now +
                (uint32_t)
                interval;
        }
    }
}

// =====================================================
// ENERGY STROBE - ORIGINAL BEHAVIOUR
// =====================================================

void updateEnergyStrobe(
    float energy,
    float dt
) {
    uint32_t now = millis();

    energy *= dynamicsFactor();
    energyStrobeValue = energy;

    if (!energyInitialized) {
        energyStrobeAverage = energy;
        energyStrobePrevious = energy;
        energyInitialized = true;
        return;
    }

    energyStrobeRise = max(
        0.0f,
        energy -
        energyStrobePrevious
    );

    bool valid =
        rms >= ENERGY_MIN_RMS &&
        energy >= ENERGY_MIN_LEVEL &&
        energy >
            energyStrobeAverage *
            ENERGY_TRIGGER_RATIO &&
        energyStrobeRise >=
            ENERGY_MIN_RISE;

    bool cooledDown =
        now - lastEnergyTrigger >=
        ENERGY_COOLDOWN_MS;

    if (valid && cooledDown) {
        lastEnergyTrigger = now;
        energyTriggerCount++;

        requestFlash(FLASH_DOUBLE);
    }

    energyStrobeAverage +=
        (energy -
        energyStrobeAverage) *
        filterAlpha(
            dt,
            ENERGY_AVERAGE_TIME
        );

    energyStrobePrevious = energy;
}

// =====================================================
// AUDIO
// =====================================================

void analyzeAudio() {
    if (!M5.Mic.record(
        audio,
        SAMPLES,
        SAMPLE_RATE
    )) {
        return;
    }

    double mean = 0.0;
    double sum = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        mean += audio[i];
    }

    mean /= SAMPLES;

    for (int i = 0; i < SAMPLES; i++) {
        double value =
            audio[i] - mean;

        sum += value * value;

        vReal[i] = value;
        vImag[i] = 0.0;
    }

    rms = sqrt(
        sum / SAMPLES
    );

    uint32_t now = millis();

    float dt =
        lastAudioUpdate == 0
        ? 0.032f
        : (now - lastAudioUpdate) /
          1000.0f;

    lastAudioUpdate = now;

    dt = constrain(
        dt,
        0.001f,
        0.25f
    );

    if (
        (mode == RAINBOW_MUSIC && waveVariant == 0) ||
        mode == VU_METER
    ) {
        updateAGC(rms, dt);
        updateAutoColor();
        return;
    }

    FFT.windowing(
        FFTWindow::Hann,
        FFTDirection::Forward
    );

    FFT.compute(
        FFTDirection::Forward
    );

    FFT.complexToMagnitude();

    double bandPower[3] = {0, 0, 0};
    int bandBins[3] = {0, 0, 0};

    double beatPower[3] = {0, 0, 0};
    int beatBins[3] = {0, 0, 0};

    double bassPower = 0.0;
    int bassBins = 0;

    for (
        int i = 2;
        i < SAMPLES / 2;
        i++
    ) {
        double freq =
            (double)i *
            SAMPLE_RATE / SAMPLES;

        double power =
            vReal[i] * vReal[i];

        int band = -1;

        if (
            freq >= 60 &&
            freq < 250
        ) {
            band = 0;
        }
        else if (
            freq >= 250 &&
            freq < 2000
        ) {
            band = 1;
        }
        else if (
            freq >= 2000 &&
            freq < 8000
        ) {
            band = 2;
        }

        if (band >= 0) {
            bandPower[band] += power;
            bandBins[band]++;
        }

        if (mode == ENERGY_STROBE) {
            int beatBand = -1;

            if (
                freq >= 50 &&
                freq < 250
            ) {
                beatBand = 0;
            }
            else if (
                freq >= 250 &&
                freq < 800
            ) {
                beatBand = 1;
            }
            else if (
                freq >= 800 &&
                freq < 2000
            ) {
                beatBand = 2;
            }

            if (beatBand >= 0) {
                beatPower[beatBand] +=
                    power;

                beatBins[beatBand]++;
            }
        }

        if (
            (mode == BPM_AUTO || mode == COMET_BEAT || (mode == RAINBOW_MUSIC && waveVariant != 0)) &&
            freq >= BPM_BASS_MIN_HZ &&
            freq < BPM_BASS_MAX_HZ
        ) {
            bassPower += power;
            bassBins++;
        }
    }

    // FFT display bands
    for (int b = 0; b < 3; b++) {
        if (bandBins[b] == 0) {
            continue;
        }

        float magnitude =
            sqrt(
                bandPower[b] /
                bandBins[b]
            ) *
            2.0f / SAMPLES;

        float signal = max(
            0.0f,
            magnitude -
            noiseGate[b]
        );

        float target = constrain(
            signal / maxLevel[b],
            0.0f,
            1.0f
        );

        if (target > levels[b]) {
            levels[b] = target;
        }
        else {
            levels[b] =
                levels[b] * 0.75f +
                target * 0.25f;
        }
    }

    // ENERGY STROBE:
    // same three-band weighting as before
    if (mode == ENERGY_STROBE) {
        float beatBands[3] = {0, 0, 0};

        const float beatScales[3] = {
            2500.0f,
            500.0f,
            300.0f
        };

        for (int b = 0; b < 3; b++) {
            if (beatBins[b] == 0) {
                continue;
            }

            float magnitude =
                sqrt(
                    beatPower[b] /
                    beatBins[b]
                ) *
                2.0f / SAMPLES;

            beatBands[b] = constrain(
                magnitude /
                beatScales[b],
                0.0f,
                1.0f
            );
        }

        float combined =
            beatBands[0] * 0.45f +
            beatBands[1] * 0.40f +
            beatBands[2] * 0.15f;

        updateEnergyStrobe(
            combined,
            dt
        );
    }

    // BPM AUTO:
    // dedicated bass detector
    if (
        (mode == BPM_AUTO || mode == COMET_BEAT || (mode == RAINBOW_MUSIC && waveVariant != 0)) &&
        bassBins > 0
    ) {
        float bassMagnitude =
            sqrt(
                bassPower / bassBins
            ) *
            2.0f / SAMPLES;

        float bassNormalized =
            constrain(
                bassMagnitude / 1800.0f,
                0.0f,
                1.0f
            );

        if (!manualBPM) {
            updateBPMDetector(
                constrain(bassNormalized * dynamicsFactor(), 0.0f, 1.0f), dt
            );
        }
    }
}

// =====================================================
// LED EFFECTS
// =====================================================

void updateRainbow() {
    float speed =
        1.0f + vu * 12.0f;

    rainbowPhase += speed;

    uint8_t brightness =
        12 + (uint8_t)(
            vu * 210
        );

    // Palette Wave uses the exact original animated hue phase and audio
    // brightness; only the color source changes.
    uint8_t wavePalette=activePalette(RAINBOW_MUSIC);
    if(wavePalette){
        for(int i=0;i<NUM_LEDS;i++){
            uint16_t phase=(uint16_t)(rainbowPhase*256.0f+(uint32_t)i*65536UL/NUM_LEDS);
            uint16_t position=(uint32_t)phase*(NUM_LEDS-1)/65535UL;
            if(paletteReverse[RAINBOW_MUSIC])position=NUM_LEDS-1-position;
            uint32_t color=saturatedPaletteColor(paletteColorAt(wavePalette,position),wavePalette);
            strip.setPixelColor(i,strip.Color(((color>>16)&255)*brightness/255,
                ((color>>8)&255)*brightness/255,(color&255)*brightness/255));
        }
        return;
    }
    for (
        int i = 0;
        i < NUM_LEDS;
        i++
    ) {
        uint16_t hue = (uint16_t)(
            rainbowPhase * 256 +
            i * 65536.0f /
            NUM_LEDS
        );

        uint32_t color =
            strip.gamma32(
                strip.ColorHSV(hue)
            );

        uint8_t r =
            ((color >> 16) & 255) *
            brightness / 255;

        uint8_t g =
            ((color >> 8) & 255) *
            brightness / 255;

        uint8_t b =
            (color & 255) *
            brightness / 255;

        if (waveColor != 5) {
            uint32_t base = waveColor == 0 ? (i < NUM_LEDS*0.5f ? strip.Color(0,255,0) : i < NUM_LEDS*0.7f ? strip.Color(255,200,0) : strip.Color(255,0,0)) :
                waveColor == 1 ? strip.Color(0,100,255) :
                waveColor == 2 ? strip.Color(255,0,0) :
                waveColor == 3 ? strip.Color(160,0,255) : strip.Color(255,175,0);
            r = ((base >> 16) & 255) * brightness / 255;
            g = ((base >> 8) & 255) * brightness / 255;
            b = (base & 255) * brightness / 255;
        }
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
}

// Wave runners: beat-launched, 2 bright LEDs + 5 fading tail LEDs.
static const uint8_t WAVE_RUNNERS = 8;
uint32_t waveStarts[WAVE_RUNNERS] = {};
int8_t waveDirections[WAVE_RUNNERS] = {};
uint8_t waveWrite = 0;
uint32_t waveLastBeat = 0;
uint32_t waveBeatSerial = 0;void resetWaveRunners() {
    for (int i=0;i<WAVE_RUNNERS;i++) waveStarts[i]=0;
    waveWrite=0; waveLastBeat=bpmFlashCount; waveBeatSerial=0;
}
void updateWaveRunners() {
    uint32_t now=millis();
    if (waveLastBeat != bpmFlashCount) {
        uint32_t beats=bpmFlashCount-waveLastBeat;
        waveLastBeat=bpmFlashCount;
        if (audioIsActive) for(uint32_t j=0;j<min(beats,(uint32_t)WAVE_RUNNERS);j++) {
            int dir = waveVariant==1 ? 1 : waveVariant==2 ? -1 : ((waveBeatSerial++ & 1) ? -1 : 1);
            waveStarts[waveWrite]=now ? now : 1;
            waveDirections[waveWrite]=dir;
            waveWrite=(waveWrite+1)%WAVE_RUNNERS;
        }
    }
    strip.clear();
    float tempo=manualBPM ? manualBPMValue : (estimatedBPM>0 ? estimatedBPM : 120.0f);
    float speed=NUM_LEDS * constrain(tempo,75.0f,180.0f) / 120000.0f; // traverse in ~2 beats
    for(int k=0;k<WAVE_RUNNERS;k++) {
        if(!waveStarts[k])continue;
        float travel=(now-waveStarts[k])*speed;
        if(travel>=NUM_LEDS+7){waveStarts[k]=0;continue;}
        int dir=waveDirections[k];
        int head=(int)travel;
        for(int tail=0;tail<7;tail++) {
            int position=(dir>0 ? head-tail : (int)NUM_LEDS-1-head+tail);
            if(position<0 || position>=NUM_LEDS)continue;
            uint8_t level=tail<2 ? 255 : (uint8_t)(210-(tail-2)*45);
            uint8_t wavePalette=activePalette(RAINBOW_MUSIC);
            uint16_t palettePosition=waveDirections[k]>0 ? (uint16_t)position : (uint16_t)(NUM_LEDS-1-position);
            if(paletteReverse[RAINBOW_MUSIC])palettePosition=NUM_LEDS-1-palettePosition;
            uint32_t base=wavePalette ? saturatedPaletteColor(paletteColorAt(wavePalette,palettePosition),wavePalette) :
                waveColor==5 ? strip.gamma32(strip.ColorHSV((uint16_t)((uint32_t)position*65535/NUM_LEDS))) :
                waveColor==0 ? (position < NUM_LEDS*0.5f ? strip.Color(0,255,0) : position < NUM_LEDS*0.7f ? strip.Color(255,200,0) : strip.Color(255,0,0)) :
                waveColor==1 ? strip.Color(0,100,255) :
                waveColor==2 ? strip.Color(255,0,0) :
                waveColor==3 ? strip.Color(160,0,255) : strip.Color(255,175,0);
            uint32_t old=strip.getPixelColor(position);
            uint8_t r=max((int)((old>>16)&255),(int)(((base>>16)&255)*level/255));
            uint8_t g=max((int)((old>>8)&255),(int)(((base>>8)&255)*level/255));
            uint8_t b=max((int)(old&255),(int)((base&255)*level/255));
            strip.setPixelColor(position,strip.Color(r,g,b));
        }
    }
}

void updateVU() {
    float displayVU =
        expandVU(constrain(vu * dynamicsFactor(), 0.0f, 1.0f));

    float displayPeak =
        expandVU(vuPeak);

    int active = constrain(
        (int)(
            displayVU *
            NUM_LEDS
        ),
        0,
        NUM_LEDS
    );

    int peakLED = constrain(
        (int)(
            displayPeak *
            (NUM_LEDS - 1)
        ),
        0,
        NUM_LEDS - 1
    );

    strip.clear();
    if (vuCentered) {
        int half = NUM_LEDS / 2;
        int lit = constrain((int)(displayVU * half), 0, half);
        for(int i=0;i<lit;i++) {
            int left=half-1-i, right=half+i;
            if(left>=0) strip.setPixelColor(left,vuColorAt(i*2));
            if(right<NUM_LEDS) strip.setPixelColor(right,vuColorAt(i*2));
        }
        if(vuPeak>0.01f) {
            int peak=constrain((int)(displayPeak*half),0,half-1);
            strip.setPixelColor(half-1-peak,vuColorAt(peak*2));
            if(half+peak<NUM_LEDS) strip.setPixelColor(half+peak,vuColorAt(peak*2));
        }
        return;
    }

    for (
        int i = 0;
        i < active;
        i++
    ) {
        strip.setPixelColor(
            i,
            vuColorAt(i)
        );
    }

    if (vuPeak > 0.01f) {
        strip.setPixelColor(
            peakLED,
            vuColorAt(peakLED)
        );
    }
}

void updateFFT() {
    strip.clear();

    for (int b = 0; b < 3; b++) {
        int active =
            (int)(
                levels[b] *
                counts[b]
            );

        uint32_t color =
            b == 0
            ? strip.Color(0, 0, 255)
            : b == 1
            ? strip.Color(0, 255, 0)
            : strip.Color(255, 140, 0);

        for (
            int i = 0;
            i < counts[b];
            i++
        ) {
            strip.setPixelColor(
                starts[b] + i,
                i < active ? color : 0
            );
        }
    }
}

// =====================================================
// LED TASK
// =====================================================

// Fire2012-inspired one-dimensional flame with audio-triggered sparks.
// Runs only on the LED task: no network polling or extra audio sampling.
uint8_t fireHeat[MAX_LEDS] = {};

void updateFireSparks() {
    // Cooling and upward diffusion. LED 0 is the base of the flame.
    for (int i = 0; i < NUM_LEDS; ++i) {
        int cooling = random(0, 14);
        fireHeat[i] = (fireHeat[i] > cooling) ? fireHeat[i] - cooling : 0;
    }
    for (int i = NUM_LEDS - 1; i >= 2; --i) {
        fireHeat[i] = (uint8_t)(((int)fireHeat[i-1] + 2*(int)fireHeat[i-2]) / 3);
    }
    if (random(100) < 70) {
        int y = random(0, min((int)NUM_LEDS, 8));
        fireHeat[y] = (uint8_t)min(255L, (long)fireHeat[y] + random(150, 256));
    }

    // Sparse music-reactive sparks, decaying independently of the fire.
    static uint8_t spark[MAX_LEDS] = {};
    float activity = constrain((signalRms - 250.0f) / 4500.0f * dynamicsFactor(), 0.0f, 1.0f);
    for (int i = 0; i < NUM_LEDS; ++i) {
        // 30ms frame, ~0.4-0.8 second fade-out.
        uint8_t fade = (uint8_t)random(10, 20);
        spark[i] = spark[i] > fade ? spark[i] - fade : 0;
    }
    // Minimum sparkle even with quiet music; louder music adds sparks.
    int sparkChance = (int)(18.0f + activity * 160.0f * dynamicsFactor());
    if (random(1000) < sparkChance) {
        spark[random(NUM_LEDS)] = 255;
    }
    for (int i = 0; i < NUM_LEDS; ++i) {
        uint8_t h = fireHeat[i];
        uint8_t r = (uint8_t)min(255, (int)h * 3);
        uint8_t g = (h < 90) ? 0 : (uint8_t)min(210, ((int)h - 90) * 2);
        uint8_t b = (h > 210) ? (uint8_t)((h - 210) / 3) : 0;
        if (spark[i]) {
            uint8_t a = spark[i];
            r = max(r, a);
            g = max(g, (uint8_t)((int)a * 92 / 100));
            b = max(b, (uint8_t)((int)a * 68 / 100));
        }
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
}


// v2.1 step 2B: apply the selected palette to the finished LED frame.
// Keep effect algorithms and audio response intact, including their brightness.
// Palette order matches the web UI (1-4 global; 5-32 saved per effect).
// Palette definitions: edit these four tables to change LED colors.
// IDs 1-7: eight stops; 8-16: four; 17-24: one; 25-32: two.
// Web palette previews are separately defined in the HTML/JS below.
static const uint32_t palettes8[][8] = {
    {0xff2222, 0xffaa00, 0xffff00, 0x00dd55, 0x0099ee, 0x6655ff, 0xcc00ff, 0xff33aa}, // 1
    {0xff00aa, 0xffaa00, 0x9900ff, 0x00ddff, 0xff2255, 0xffff00, 0x00ff99, 0xff00aa}, // 2
    {0x001a88, 0x004dff, 0x008cff, 0x00d6ff, 0x00ffdd, 0x002acc, 0x006bff, 0x00ffef}, // 3
    {0x00451b, 0x00822b, 0x00bc31, 0x59db00, 0x9ee600, 0x00a54d, 0x007c32, 0x6ed400}, // 4
    {0x27104e, 0x702b86, 0xb33986, 0xf25f5c, 0xff914d, 0xffcf70, 0xf76d39, 0x38164b}, // 5
    {0x350000, 0x9c1600, 0xff4000, 0xff8c00, 0xffe080, 0xff5400, 0xb32100, 0x350000}, // 6
    {0x08213e, 0x007c80, 0x00bb83, 0x8bffba, 0x8a53d6, 0x1bb1a7, 0x3b358e, 0x08213e}, // 7
};

static const uint32_t palettes4[][4] = {
    {0xf22525, 0xffb600, 0x0ab64a, 0x2567ed}, // 1
    {0xff00aa, 0x00e5ff, 0x9d4edd, 0xffee00}, // 2
    {0x00d1b2, 0xffb703, 0xfb5607, 0x3a86ff}, // 3
    {0x002b7f, 0x008cff, 0x8ceeff, 0xffffff}, // 4
    {0xf9c74f, 0xf9844a, 0xf94144, 0x577590}, // 5
    {0xff65b3, 0xb388eb, 0x66d9e8, 0xffe66d}, // 6
    {0x8c2f00, 0xd65a00, 0xffa62b, 0xffdf80}, // 7
    {0x061a40, 0x214e8a, 0x6844b8, 0xc44dce}, // 8
    {0x9c4637, 0xd29b6f, 0xe8d8b8, 0x647e73}, // 9
};

// Saturated base hues: effect brightness scales from black to this exact RGB value.
static const uint32_t palettes1[][1] = {
    {0xff0000}, // Red
    {0x0000ff}, // Blue
    {0x00ff00}, // Green
    {0x8000ff}, // Purple
    {0xffa000}, // Amber
    {0x00ffff}, // Cyan
    {0xff0080}, // Pink
    {0xffffff}, // White
};

static const uint32_t palettes2[][2] = {
    {0xff0000, 0x003dff}, // 1
    {0x003dff, 0xffa000}, // 2
    {0xff0090, 0x00cfff}, // 3
    {0x7900ff, 0x00e647}, // 4
    {0xff7500, 0x00cccc}, // 5
    {0xff0000, 0xffffff}, // 6
    {0xffbc00, 0x7000ff}, // 7
    {0x004aff, 0xffffff}, // 8
};

// One continuous gradient from bottom to top; no repeated patterns.
uint8_t paletteStopCount(uint8_t id) {return id<=7?8:id<=16?4:id<=24?1:2;}
uint32_t paletteStopColor(uint8_t id,uint32_t beat) {
    if(id<1||id>32) return strip.Color(255,255,255);
    uint8_t count=paletteStopCount(id);
    uint8_t index=(uint8_t)(beat%count);
    if(id<=7)return palettes8[id-1][index];
    if(id<=16)return palettes4[id-8][index];
    if(id<=24)return palettes1[id-17][0];
    return palettes2[id-25][index];
}
uint32_t paletteColorAt(uint8_t id, uint16_t position) {
    if (id < 1 || id > 32) return 0;
    const uint32_t* colors;
    uint8_t count;
    if (id <= 7)       { colors = palettes8[id-1]; count = 8; }
    else if (id <= 16) { colors = palettes4[id-8]; count = 4; }
    else if (id <= 24) { colors = palettes1[id-17]; count = 1; }
    else               { colors = palettes2[id-25]; count = 2; }
    if (count == 1) return colors[0];
    const uint16_t maxPosition = NUM_LEDS > 1 ? NUM_LEDS-1 : 1;
    const uint32_t scaled = (uint32_t)position * (count-1) * 256UL / maxPosition;
    const uint8_t index = min((int)(scaled >> 8), (int)count-1);
    const uint8_t next = min((int)index+1, (int)count-1);
    const uint8_t fraction = (uint8_t)(scaled & 255);
    const uint32_t from = colors[index], to = colors[next];
    const uint8_t r = (((from>>16)&255)*(256-fraction) + ((to>>16)&255)*fraction) >> 8;
    const uint8_t g = (((from>>8)&255)*(256-fraction) + ((to>>8)&255)*fraction) >> 8;
    const uint8_t b = ((from&255)*(256-fraction) + (to&255)*fraction) >> 8;
    return strip.Color(r,g,b);
}
// Boost saturation by removing the shared white component from RGB.
// Neutral colors stay neutral only in palettes explicitly containing white/grey.
// Interpolate directly between palette stops in RGB (no white or black midpoint).
// Remove only the shared white component; never boost the peak to 255,
// because that washes out the intermediate colors in all multi-color palettes.
uint32_t saturatedPaletteColor(uint32_t color, uint8_t id) {
    // Palettes deliberately containing white/grey retain those defined stops.
    if (id == 11 || id == 24 || id == 30 || id == 32) return color;
    uint8_t r=(color>>16)&255, g=(color>>8)&255, b=color&255;
    const uint8_t low=min(r,min(g,b));
    const uint8_t remove=(uint16_t)low*95/100;
    return strip.Color(r-remove,g-remove,b-remove);
}
void applyPaletteFrame(LedMode effect) {
    if (effect == ENERGY_STROBE || effect == RAINBOW_MUSIC) return; // Wave renders palettes in its own animated renderer.
    if (effect == COMET_BEAT && particleVariant == 5) return; // Firework fragments use individual palette-stop colors.
    uint8_t id = activePalette((uint8_t)effect);
    if (!id || id > 32) return; // DEFAULT retains original effect colors.
    for (uint16_t i=0; i<NUM_LEDS; i++) {
        uint32_t old = strip.getPixelColor(i);
        uint8_t r=(old>>16)&255, g=(old>>8)&255, b=old&255;
        uint8_t intensity = max(r,max(g,b));
        if (!intensity) continue;
        // Square the effect envelope: dark levels remain dark, peaks reach full color.
        const uint16_t level = ((uint32_t)intensity*intensity + 127) / 255;
        // Center-origin effects use a mirrored palette: center is stop 0,
// both ends are the last stop. Reverse swaps center and ends.
        const bool centerOrigin =
            (effect == VU_METER && vuCentered) ||
            (effect == COMET_BEAT && (particleVariant == 0 || particleVariant == 1));
        uint16_t palettePosition = i;
        if (centerOrigin && NUM_LEDS > 1) {
            // Integer mapping handles odd and even LED counts symmetrically.
            const uint16_t doubledDistance = (uint16_t)abs((int)(2*i) - ((int)NUM_LEDS-1));
            palettePosition = (uint32_t)doubledDistance * (NUM_LEDS-1) /
                              (NUM_LEDS-1 + (NUM_LEDS%2==0 ? 1 : 0));
            if (palettePosition >= NUM_LEDS) palettePosition = NUM_LEDS-1;
        }
        if (paletteReverse[(uint8_t)effect]) palettePosition = NUM_LEDS-1-palettePosition;
        uint32_t c = saturatedPaletteColor(paletteColorAt(id,palettePosition),id);
        strip.setPixelColor(i, strip.Color(
            ((c>>16)&255)*level/255,
            ((c>>8)&255)*level/255,
            (c&255)*level/255
        ));
    }
}

// Draw VU peak last, at full selected LED brightness, using the palette's
// final stop (or first stop when reversed), without palette attenuation.
void drawVUPeakDot(){
    if(vuPeak<=0.01f)return;
    uint8_t id=activePalette(VU_METER);
    uint32_t color;
    if(id){
        uint16_t end=paletteReverse[VU_METER]?0:NUM_LEDS-1;
        color=saturatedPaletteColor(paletteColorAt(id,end),id);
    }else color=vuColorAt(NUM_LEDS-1);
    uint8_t r=(color>>16)&255,g=(color>>8)&255,b=color&255;
    uint8_t maximum=max((int)r,max((int)g,(int)b));
    if(maximum==0)return;
    color=strip.Color((uint16_t)r*255/maximum,(uint16_t)g*255/maximum,(uint16_t)b*255/maximum);
    float displayPeak=expandVU(vuPeak);
    if(vuCentered){
        int half=NUM_LEDS/2;
        int peak=constrain((int)(displayPeak*half),0,half-1);
        strip.setPixelColor(half-1-peak,color);
        if(half+peak<NUM_LEDS)strip.setPixelColor(half+peak,color);
    }else{
        int peak=constrain((int)(displayPeak*(NUM_LEDS-1)),0,NUM_LEDS-1);
        strip.setPixelColor(peak,color);
    }
}
void ledTask(void* parameter) {
    LedMode previousMode = RAINBOW_MUSIC;

    bool outputOn = false;
    uint32_t lastPaletteRevision = paletteRevision;

    uint32_t lastNormalLED = 0;

    while (true) {
        if (currentBrightness != requestedBrightness) {
            currentBrightness = requestedBrightness;
            strip.setBrightness(currentBrightness);
        }
        LedMode currentMode = mode;
        bool paletteChanged=(lastPaletteRevision!=paletteRevision);
        if (paletteChanged) lastPaletteRevision=paletteRevision;

        if (currentMode != previousMode) {
            strip.clear();
            strip.show();

            outputOn = false;
            if (currentMode == COMET_BEAT) resetParticles();
            if (currentMode == RAINBOW_MUSIC) resetWaveRunners();
            previousMode = currentMode;
        }

        if (
            currentMode == BPM_AUTO ||
            currentMode == ENERGY_STROBE
        ) {
            FlashCommand command;
            uint32_t commandTime;

            portENTER_CRITICAL(&ledMux);

            command = flashCommand;
            commandTime =
                flashCommandTime;

            flashCommand = FLASH_NONE;

            portEXIT_CRITICAL(&ledMux);

            if (command != FLASH_NONE) {
                portENTER_CRITICAL(&ledMux);

                flashStart[0] =
                    commandTime;

                flashEnd[0] =
                    commandTime +
                    (command == FLASH_SINGLE
                        ? BPM_FLASH_MS
                        : ENERGY_FLASH_MS);

                flashCount = 1;

                if (command == FLASH_DOUBLE) {
                    uint8_t count = (currentMode == ENERGY_STROBE)
                        ? constrain((int)energyFlashVariant, 1, 4) : 2;
                    for (uint8_t k = 1; k < count; ++k) {
                        flashStart[k] = commandTime + k * ENERGY_SECOND_FLASH_DELAY_MS;
                        flashEnd[k] = flashStart[k] + ENERGY_FLASH_MS;
                    }
                    flashCount = count;
                }

                portEXIT_CRITICAL(&ledMux);
            }

            uint32_t now = millis();

            bool shouldBeOn = false;

            portENTER_CRITICAL(&ledMux);

            for (
                int i = 0;
                i < flashCount;
                i++
            ) {
                if (timeInWindow(
                    now,
                    flashStart[i],
                    flashEnd[i]
                )) {
                    shouldBeOn = true;
                    break;
                }
            }

            portEXIT_CRITICAL(&ledMux);

            if (currentMode == BPM_AUTO && bpmDisplayVariant == 1 && bpmFlashCount > 0) {
                shouldBeOn = true;
            }
            if (currentMode == BPM_AUTO && bpmColorVariant != 0 && bpmFlashCount > 0 && activePalette(BPM_AUTO)==0) {
                shouldBeOn = true;  // hold last red/blue or rainbow color in silence
            }
            static uint32_t lastShownBeat = 0;
            static uint8_t lastShownVariant = 255;
            bool colorChanged = (currentMode == BPM_AUTO && (bpmDisplayVariant==1 || bpmColorVariant != 0) &&
                (lastShownBeat != bpmFlashCount || lastShownVariant != bpmColorVariant));
            static uint8_t lastDisplayVariant=255;
            bool displayChanged=lastDisplayVariant!=bpmDisplayVariant;
            lastDisplayVariant=bpmDisplayVariant;
            if (shouldBeOn != outputOn || colorChanged || paletteChanged || displayChanged) {
                outputOn = shouldBeOn;
                lastShownBeat = bpmFlashCount;
                lastShownVariant = bpmColorVariant;

                strip.clear();

                if (outputOn) {
                    for (
                        int i = 0;
                        i < NUM_LEDS;
                        i++
                    ) {
                        strip.setPixelColor(
                            i,
                            strip.Color(
                                (currentMode == BPM_AUTO && bpmColorVariant == 1 && (bpmFlashCount % 2 == 0)) ? 255 :
                                (currentMode == BPM_AUTO && bpmColorVariant == 1) ? 0 :
                                (currentMode == BPM_AUTO && bpmColorVariant == 2) ? (uint8_t)(strip.gamma32(strip.ColorHSV((uint16_t)((bpmFlashCount * 10923UL) & 65535))) >> 16) : 255,
                                (currentMode == BPM_AUTO && bpmColorVariant == 1) ? 0 :
                                (currentMode == BPM_AUTO && bpmColorVariant == 2) ? (uint8_t)(strip.gamma32(strip.ColorHSV((uint16_t)((bpmFlashCount * 10923UL) & 65535))) >> 8) : 255,
                                (currentMode == BPM_AUTO && bpmColorVariant == 1 && (bpmFlashCount % 2 == 1)) ? 255 :
                                (currentMode == BPM_AUTO && bpmColorVariant == 1) ? 0 :
                                (currentMode == BPM_AUTO && bpmColorVariant == 2) ? (uint8_t)strip.gamma32(strip.ColorHSV((uint16_t)((bpmFlashCount * 10923UL) & 65535))) : 255
                            )
                        );
                    }
                }

                if(currentMode==BPM_AUTO && activePalette(BPM_AUTO)>0 && outputOn){
                    uint8_t id=activePalette(BPM_AUTO);
                    uint32_t c=saturatedPaletteColor(paletteStopColor(id,bpmFlashCount>0?bpmFlashCount-1:0),id);
                    for(int i=0;i<NUM_LEDS;i++)strip.setPixelColor(i,c);
                }else applyPaletteFrame(currentMode);
                strip.show();
            }
        }
        else {
            uint32_t now = millis();

            if (
                now - lastNormalLED >= 30
            ) {
                lastNormalLED = now;

                if (
                    currentMode ==
                    RAINBOW_MUSIC
                ) {
                    if(waveVariant==0) updateRainbow(); else updateWaveRunners();
                }
                else if (
                    currentMode ==
                    VU_METER
                ) {
                    updateVU();
                }
                else if (currentMode == FIRE_SPARKS) {
                    updateFireSparks();
                }
                else if (currentMode == COMET_BEAT) {
                    updateParticles();
                }

                applyPaletteFrame(currentMode);
                if(currentMode==VU_METER)drawVUPeakDot();
                strip.show();
            }
        }

        vTaskDelay(
            pdMS_TO_TICKS(1)
        );
    }
}

// =====================================================
// DISPLAY
// =====================================================

void drawBar(
    int x,
    int y,
    int w,
    int h,
    float value,
    uint16_t color
) {
    value = constrain(
        value,
        0.0f,
        1.0f
    );

    canvas.fillRoundRect(
        x, y, w, h, 3,
        TFT_DARKGREY
    );

    int filled =
        (int)(w * value);

    if (filled > 0) {
        canvas.fillRoundRect(
            x, y,
            filled, h, 3,
            color
        );
    }
}

void updateScreen() {
    canvas.fillSprite(TFT_BLACK);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextSize(1);

    int w = screenW - 20;

    canvas.setCursor(5, 5);
    canvas.print(modeNames[mode]);

    canvas.drawFastHLine(
        5, 19,
        screenW - 10,
        TFT_DARKGREY
    );

    if (mode == RAINBOW_MUSIC) {
        for (
            int x = 0;
            x < w;
            x += 3
        ) {
            uint16_t hue = (uint16_t)(
                rainbowPhase * 256 +
                x * 65536.0f / w
            );

            uint32_t rgb =
                strip.gamma32(
                    strip.ColorHSV(hue)
                );

            uint16_t color =
                canvas.color565(
                    (rgb >> 16) & 255,
                    (rgb >> 8) & 255,
                    rgb & 255
                );

            int h =
                8 + (int)(vu * 22);

            canvas.fillRect(
                10 + x,
                28,
                3,
                h,
                color
            );
        }

        canvas.setCursor(10, 65);
        canvas.printf(
            "RMS %.0f  GAIN %.1fx",
            rms,
            agcGain
        );

        drawBar(
            10, 82,
            w, 12,
            vu,
            TFT_GREEN
        );
    }
    else if (mode == VU_METER) {
        canvas.setCursor(10, 25);
        canvas.print("COLOR: ");
        canvas.print(
            vuColorNames[vuColorMode]
        );

        if (
            vuColorMode == VU_AUTO
        ) {
            canvas.setCursor(10, 39);
            canvas.print("ACTIVE: ");
            canvas.print(
                autoColorNames[
                    autoColorIndex
                ]
            );
        }

        const int segments = 34;
        const int gap = 2;

        int segW =
            (w -
            (segments - 1) * gap) /
            segments;

        float displayVU =
            expandVU(vu);

        float displayPeak =
            expandVU(vuPeak);

        int active = constrain(
            (int)(
                displayVU * segments
            ),
            0,
            segments
        );

        int peakSeg = constrain(
            (int)(
                displayPeak *
                (segments - 1)
            ),
            0,
            segments - 1
        );

        for (
            int i = 0;
            i < segments;
            i++
        ) {
            int x =
                10 +
                i * (segW + gap);

            int ledIndex =
                i * (NUM_LEDS - 1) /
                (segments - 1);

            uint16_t c =
                i < active
                ? vuScreenColorAt(
                    ledIndex
                )
                : TFT_DARKGREY;

            if (
                i == peakSeg &&
                vuPeak > 0.01f
            ) {
                c =
                    vuScreenColorAt(
                        ledIndex
                    );
            }

            canvas.fillRect(
                x,
                55,
                segW,
                19,
                c
            );
        }

        canvas.setCursor(10, 82);
        canvas.printf(
            "RMS %.0f GAIN %.1fx",
            rms,
            agcGain
        );

        canvas.setCursor(10, 98);
        canvas.printf(
            "VU %d%% PEAK %d%%",
            (int)(
                displayVU * 100
            ),
            (int)(
                displayPeak * 100
            )
        );
    }
    else if (mode == COMET_BEAT) {
        canvas.setCursor(10, 27);
        canvas.print("PARTICLES");
        canvas.setCursor(10, 48);
        if (manualBPM) canvas.printf("MANUAL %.1f", manualBPMValue);
        else if (estimatedBPM > 0) canvas.printf("BPM %.1f", estimatedBPM);
        else canvas.print("SEARCHING BPM");
        canvas.setCursor(10, 70);
        canvas.print(bpmLocked ? "LOCKED" : "LISTENING");
    }
    else if (mode == BPM_AUTO) {
        canvas.setCursor(10, 27);

        if (manualBPM || estimatedBPM > 0.0f) {
            canvas.printf(
                "BPM: %.1f",
                manualBPM ? manualBPMValue : estimatedBPM
            );
        }
        else {
            canvas.print(
                "BPM: SEARCHING"
            );
        }

        canvas.setCursor(10, 44);
        canvas.print(
            bpmLocked
            ? "STATUS: LOCKED"
            : "STATUS: SEARCHING"
        );

        canvas.setCursor(10, 61);
        canvas.printf(
            "CONF: %u / 10",
            (unsigned)
            bpmConfidence
        );

        drawBar(
            10,
            78,
            w,
            10,
            bpmConfidence /
            10.0f,
            TFT_GREEN
        );

        canvas.setCursor(10, 94);
        canvas.printf(
            "BEATS:%lu FLASH:%lu",
            (unsigned long)
            bpmDetectedBeats,
            (unsigned long)
            bpmFlashCount
        );
    }
    else if (mode == ENERGY_STROBE) {
        canvas.setCursor(10, 27);
        canvas.print("ENERGY");

        drawBar(
            10,
            43,
            w,
            14,
            energyStrobeValue,
            TFT_WHITE
        );

        canvas.setCursor(10, 65);
        canvas.printf(
            "AVG %.2f RISE %.2f",
            energyStrobeAverage,
            energyStrobeRise
        );

        canvas.setCursor(10, 82);
        canvas.printf(
            "TRIGGERS: %lu",
            (unsigned long)
            energyTriggerCount
        );

        canvas.setCursor(10, 99);
        canvas.print(
            "DOUBLE FLASH"
        );
    }

    else if (mode == FIRE_SPARKS) {
        canvas.setCursor(10, 35);
        canvas.print("FIRE & SPARKS");
        canvas.setCursor(10, 58);
        canvas.print("MUSIC REACTIVE");
        canvas.setCursor(10, 81);
        canvas.print("FLAME + SPARKS");
    }

    canvas.setTextColor(
        TFT_LIGHTGREY
    );

    canvas.setCursor(
        5,
        screenH - 12
    );

    canvas.print(
        "A: Mode  B: Color"
    );

    canvas.pushSprite(0, 0);
}

// =====================================================
// RESET
// =====================================================

void resetBPM() {
    bpmBass = 0.0f;
    bpmBaseline = 0.0f;

    bpmPrevious2 = 0.0f;
    bpmPrevious1 = 0.0f;

    bpmPrevious1Time = 0;
    bpmDetectorInitialized = false;

    lastBPMOnset = 0;
    bpmDetectedBeats = 0;

    bpmHistoryCount = 0;
    bpmHistoryIndex = 0;

    estimatedBPM = 0.0f;
    beatIntervalMs = 0.0f;

    bpmLocked = false;
    bpmConfidence = 0;

    lastBPMEvidence = 0;
    nextBPMFlash = 0;

    bpmFlashCount = 0;
}

void resetEnergy() {
    energyStrobeValue = 0.0f;
    energyStrobeAverage = 0.0f;
    energyStrobePrevious = 0.0f;
    energyStrobeRise = 0.0f;

    energyInitialized = false;

    lastEnergyTrigger = 0;
    energyTriggerCount = 0;
}

// =====================================================
// WIFI SETUP AND WEB CONTROL (v1.1)
// =====================================================

WebServer webServer(80);
DNSServer dnsServer;
Preferences wifiPreferences;
Preferences ledPreferences;
Preferences palettePreferences;
// Palette selections are declared above the LED task.


const char* SETUP_SSID = "MusicLED-Setup";
const char* SETUP_PASSWORD = "MusicLED123"; // Change before regular use (8+ chars)
const char* HOST_NAME = "musicled";
bool setupAPActive = false;
bool webStarted = false;

void applyModeChange(LedMode newMode);
// Random Play: a selection must last 3 minutes AND music must be silent
// for 3 continuous seconds before the next automatic selection.
static bool randomPlay = false;
static uint32_t randomSelectedAt = 0;
static uint32_t randomSilentSince = 0;
static int randomPreviousKey = -1;

static void selectRandomEffect() {
    // Each effect/submode/color combination is a possible selection.
    // Fire has no color selector; Energy uses its flash-count variant.
    int key, nextMode, sub, color;
    do {
        nextMode = random(0, 6);
        sub = nextMode == 0 ? random(0, 4) : nextMode == 1 ? random(0, 2) :
              nextMode == 2 ? random(0, 6) : nextMode == 4 ? random(1, 5) : 0;
        color = nextMode == 0 || nextMode == 1 || nextMode == 2 ? random(0, 6) :
                nextMode == 3 ? random(0, 3) : 0;
        key = nextMode * 100 + sub * 10 + color;
    } while (key == randomPreviousKey);
    randomPreviousKey = key;
    applyModeChange((LedMode)nextMode);
    if (nextMode == RAINBOW_MUSIC) {
        waveVariant = sub; waveColor = color; resetWaveRunners();
    } else if (nextMode == VU_METER) {
        vuCentered = (sub != 0);
        static const VUColorMode vuPalette[6] = {VU_CLASSIC,VU_BLUE,VU_RED,VU_PURPLE,VU_GOLD,VU_RAINBOW};
        vuColorMode = vuPalette[color];
    } else if (nextMode == COMET_BEAT) {
        particleVariant = sub; particleColor = color; resetParticles();
    } else if (nextMode == BPM_AUTO) {
        bpmColorVariant = color;
    } else if (nextMode == ENERGY_STROBE) {
        energyFlashVariant = sub;
    }
    randomSelectedAt = millis();
    randomSilentSince = 0;
}

static void updateRandomPlay(uint32_t now) {
    if (!randomPlay) return;
    // Use the existing adaptive microphone gate, but require a fresh 3s
    // quiet interval rather than its 1s activity hold time.
    // lastMusicActivity records the last sample above the adaptive noise gate.
    // This measures the full 3-second silence, without adding the 1s hold.
    if (now - randomSelectedAt >= 180000UL &&
        lastMusicActivity != 0 && now - lastMusicActivity >= 3000UL)
        selectRandomEffect();
}

void applyModeChange(LedMode newMode) {
    if (newMode == mode) return;
    clearFlashSchedule();
    resetBPM();
    resetEnergy();
    vuPeak = 0.0f;
    lastPeakUpdate = 0;
    vuPeakHoldUntil = 0;
    silenceStart = 0;
    silenceTriggered = false;
    mode = newMode;
}

String htmlHeader(const char* title) {
    String h = F("<!doctype html><html lang='no'><head><meta charset='utf-8'>"
                 "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                 "<style>body{font:16px system-ui;background:#101827;color:#f2f5fa;"
                 "max-width:520px;margin:30px auto;padding:16px}"                 "button,input,select{font:inherit;padding:12px;border-radius:9px;margin:6px 0}"
                 "button{background:#1677ee;color:white;border:0;cursor:pointer}"
                 "a{color:#80bdff}.panel{background:#202d40;padding:18px;border-radius:14px}"
                 "label{display:block;margin-top:14px}input,select{box-sizing:border-box;width:100%}"
                 "</style><title>");
    h += title;
    h += F("</title></head><body><h2>");
    h += title;
    h += F("</h2><div class='panel'>");
    return h;
}

String htmlFooter() { return F("</div></body></html>"); }

void showSetupPage() {
    String h = htmlHeader("MusicLED Wi-Fi oppsett");
    h += F("<p>Velg et 2,4 GHz-nettverk og skriv inn passordet.</p>"
           "<form method='POST' action='/save'>"
           "<label>Wi-Fi-nettverk</label><select name='ssid'>");
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n && i < 25; i++) {
        String ssid = WiFi.SSID(i);
        ssid.replace("&", "&amp;"); ssid.replace("<", "&lt;");
        ssid.replace(">", "&gt;"); ssid.replace("\"", "&quot;");
        ssid.replace("'", "&#39;");
        h += F("<option value='"); h += ssid; h += F("'>");
        h += ssid; h += F("</option>");
    }
    WiFi.scanDelete();
    h += F("</select><label>Passord</label>"
           "<input name='password' type='password' autocomplete='off' required>"
           "<button type='submit'>Lagre og koble til</button></form>");
    h += htmlFooter();
    webServer.send(200, "text/html; charset=utf-8", h);
}

void showControlPage() {
    String h = F(R"HTML(<!doctype html><html lang="no"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png"><link rel="icon" type="image/png" href="/apple-touch-icon.png"><meta name="theme-color" content="#0b1020"><meta name="apple-mobile-web-app-capable" content="yes"><meta name="apple-mobile-web-app-status-bar-style" content="black-translucent"><meta name="apple-mobile-web-app-title" content="MusicLED"><meta name="mobile-web-app-capable" content="yes"><title>MusicLED</title>
<style>
*{box-sizing:border-box}body{margin:0;background:#0b1020;color:#f4f7ff;font-family:-apple-system,BlinkMacSystemFont,system-ui,sans-serif}
.wrap{max-width:540px;margin:auto;min-height:100dvh;padding:max(60px,env(safe-area-inset-top)) 16px max(22px,env(safe-area-inset-bottom));display:flex;flex-direction:column}
.top{display:flex;justify-content:space-between;align-items:center;margin-bottom:18px}
h1{font-size:23px;letter-spacing:.04em;margin:0}.sub{font-size:12px;color:#8e9db8;margin-top:4px}
.dot{width:9px;height:9px;background:#3bdca4;border-radius:50%;box-shadow:0 0 12px #3bdca4}
.pads{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px}
.pad{aspect-ratio:1/1;min-height:125px;border:1px solid #35405c;border-radius:22px;background:linear-gradient(145deg,#252e45,#151c2f);color:#eaf1ff;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:13px;box-shadow:inset 0 1px #ffffff16,0 7px 0 #080c17;touch-action:manipulation;cursor:pointer;transition:transform .08s,background .1s,border-color .1s}
.pad:active{transform:translateY(5px);box-shadow:inset 0 1px #ffffff16,0 2px 0 #080c17}
.pad.active{background:linear-gradient(145deg,#174a68,#133047);border-color:#55d9ff;box-shadow:inset 0 0 0 1px #55d9ff55,0 6px 0 #071b2b,0 0 18px #28b9fa2e}
.pad .symbol{width:55px;height:55px;display:grid;place-items:center}.symbol svg{width:52px;height:52px;fill:none;stroke:currentColor;stroke-width:2.4;stroke-linecap:round;stroke-linejoin:round}
.pad .name{font-size:13px;font-weight:750;letter-spacing:.065em;text-align:center}.pad .hint{font-size:10px;color:#a1afc6;margin-top:-10px}
.footer{margin-top:auto;padding-top:25px;display:flex;align-items:center;justify-content:space-between;color:#93a2bb;font-size:13px}
.randomBtn{border:1px solid #38455f;background:#202a40;color:#d9e8ff;min-height:44px;padding:0 15px;border-radius:14px;display:flex;align-items:center;gap:9px;font:600 12px system-ui;letter-spacing:.05em;touch-action:manipulation}.randomBtn.on{background:#174a68;border-color:#55d9ff;color:#fff;box-shadow:0 0 12px #28b9fa44}.randomBtn svg{width:19px;height:19px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round;stroke-linejoin:round}
.gear{border:1px solid #38455f;background:#202a40;color:white;width:51px;height:51px;border-radius:15px;display:grid;place-items:center;text-decoration:none}
.gear svg{width:25px;height:25px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round;stroke-linejoin:round}
[hidden]{display:none!important}
.gear{cursor:pointer;touch-action:manipulation}
.paletteGrid{display:grid;grid-template-columns:repeat(8,minmax(0,1fr));gap:4px}
.paletteTile{border:2px solid #34445f;background:#1c2840;color:#dfeaff;border-radius:7px;padding:3px 1px;min-width:0;cursor:pointer;touch-action:manipulation}
.paletteTile.selected{border-color:#65dcff;box-shadow:0 0 0 1px #65dcff88}
.paletteSwatch{display:flex;height:23px;border-radius:4px;overflow:hidden}
.paletteSwatch i{flex:1;min-width:0}
.paletteLabel{display:block;font-size:9px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;margin-top:3px}
.paletteGroup{margin:9px 0}.paletteGroup h3{font-size:12px;margin:0 0 4px}
.paletteHelp{color:#a9b9d4;font-size:11px;margin:6px 0}#paletteView .settingsTop{margin-bottom:6px}#paletteView .settingsTop h2{font-size:18px}#paletteView{padding-bottom:8px}

#settingsView{padding:4px 2px 30px}
.settingsTop{display:flex;align-items:center;gap:16px;margin-bottom:16px}
.settingsTop button,.tapBox button{border:0;border-radius:12px;background:#2366b9;color:white;padding:13px 16px;font:inherit;touch-action:manipulation}
.settingsTop h2{font-size:21px;margin:0}
#settingsView h3{margin:25px 0 10px}
#settingsView label{display:block;margin:16px 0 7px}
#settingsView input[type=range]{width:100%;height:32px}
#settingsView input[type=radio]{width:auto}
.help{font-size:13px;color:#a9b9d4}
.tapBox{background:#15253a;padding:15px;border-radius:12px;text-align:center}
#tapBpm{font-size:32px;font-weight:700;margin:8px}
.tapBox .tapButton{display:block;width:100%;height:90px;font-size:24px;margin:12px 0}
#settingsView a{color:#80bdff}
.saveLed{background:#2366b9;color:white;border:0;border-radius:12px;padding:13px 16px;font:inherit;margin-top:12px}
</style></head><body><main class="wrap"><header class="top"><div><h1>MusicLED</h1></div><div style="text-align:right"><div id="bpmDisplay" style="font-weight:700;font-size:19px">-- BPM</div><div id="bpmSource" class="sub">AUTO</div></div></header>
<section id="homeView"><section class="pads" id="pads">
<button class="pad" data-mode="0" onclick="choose(0)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M8 39c8-22 16-22 24 0s16 22 24 0"/><path d="M8 24c8-22 16-22 24 0s16 22 24 0"/></svg></span><span class="name">WAVE</span><span class="hint">RAINBOW · STANDARD</span></button>
<button class="pad" data-mode="1" onclick="choose(1)"><span class="symbol"><svg viewBox="0 0 64 64"><rect x="10" y="40" width="8" height="14" rx="2"/><rect x="23" y="29" width="8" height="25" rx="2"/><rect x="36" y="18" width="8" height="36" rx="2"/><rect x="49" y="9" width="8" height="45" rx="2"/></svg></span><span class="name">VU METER</span><span class="hint">LEVEL</span></button>
<button class="pad" data-mode="2" onclick="choose(2)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M11 36 Q30 2 57 16 Q40 13 28 30 Q23 36 21 42 Z" fill="currentColor" stroke="none"/><path d="M12 30l4 8 9 1-7 6 2 9-8-5-8 5 2-9-7-6 9-1z" transform="translate(6 -8)" fill="currentColor" stroke="none"/></svg></span><span class="name">PARTICLES</span><span class="hint">RAINBOW · BASS EXPLOSION</span></button>
<button class="pad" data-mode="3" onclick="choose(3)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M9 33h11l7-16 10 31 7-15h11"/><path d="M22 9a25 25 0 1 1-13 43"/></svg></span><span class="name">BPM AUTO</span><span class="hint">WHITE</span></button>
<button class="pad" data-mode="4" onclick="choose(4)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M36 5 14 35h16l-3 24 23-34H34z"/></svg></span><span class="name">ENERGY</span><span class="hint">STROBE</span></button>
<button class="pad" data-mode="5" onclick="choose(5)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M34 5c4 14-6 18-3 27 6-3 9-9 10-14 19 20 15 37-8 40C10 59 8 42 18 29c1 11 7 13 9 13-5-14 0-27 7-37z"/></svg></span><span class="name">FIRE</span><span class="hint">&amp; SPARKS</span></button>
</section><footer class="footer"><button type="button" id="randomBtn" class="randomBtn" onclick="randomPress()" aria-pressed="false"><svg viewBox="0 0 24 24"><path d="M4 7h3c5 0 5 10 10 10h3m-3-3 3 3-3 3M4 17h3c2 0 3-2 4-4m2-3c1-2 2-3 4-3h3m-3-3 3 3-3 3"/></svg><span>RANDOM</span></button><button type="button" class="gear" onclick="showPalettes()" aria-label="Fargepaletter" title="Fargepaletter"><svg viewBox="0 0 24 24" aria-hidden="true"><path d="M2 20a10 10 0 0 1 20 0" stroke="#f44" stroke-width="3" fill="none"/><path d="M5 20a7 7 0 0 1 14 0" stroke="#ffb733" stroke-width="3" fill="none"/><path d="M8 20a4 4 0 0 1 8 0" stroke="#3edc83" stroke-width="3" fill="none"/><path d="M11 20a1 1 0 0 1 2 0" stroke="#559bff" stroke-width="2" fill="none"/></svg></button><button type="button" class="gear" onclick="showSettings()" aria-label="Innstillinger"><svg viewBox="0 0 24 24"><path d="M10 2h4l.8 2.6 2 .9 2.4-1.2 2.8 2.8-1.2 2.4.9 2L24 12l-2.3 1.5-.9 2 1.2 2.4-2.8 2.8-2.4-1.2-2 .9L14 23h-4l-.8-2.6-2-.9-2.4 1.2L2 17.9l1.2-2.4-.9-2L0 12l2.3-1.5.9-2L2 6.1l2.8-2.8 2.4 1.2 2-.9z" transform="translate(1 1) scale(.91)"/><circle cx="12" cy="12" r="3"/></svg></button></footer></section>
<section id="paletteView" hidden>
<header class="settingsTop"><button type="button" onclick="showHome()">← Tilbake</button><h2>Fargepaletter</h2></header>
<div id="paletteGroups"></div><p class="paletteHelp" id="paletteModeName">Velg palett for aktiv effekt.</p><p><button type="button" id="paletteReverseButton" onclick="togglePaletteReverse()">⇅ Snu fargeretning</button></p><p class="paletteHelp" id="paletteStatus"></p><p class="paletteHelp">De fire første palettene er midlertidige globale valg. Andre valg lagres per effekt. LED-fargene aktiveres i steg 2B.</p>
</section>
<section id="settingsView" hidden>
<header class="settingsTop"><button type="button" onclick="showHome()">← Tilbake</button><h2>Innstillinger</h2></header>
<h3>LED &amp; Audio</h3>
<label>Dynamikk: <b id="dynVal">50%</b></label><input id="dynInput" type="range" min="0" max="100" step="5" value="50" oninput="document.getElementById('dynVal').textContent=this.value+'%'" onchange="fetch('/api/dynamics?value='+this.value)">
<p class="help">50 % tilsvarer normal lydfølsomhet.</p>
<label>Lysstyrke: <b id="level">60</b></label><input id="brightInput" type="range" min="1" max="255" value="60" oninput="document.getElementById('level').textContent=this.value" onchange="fetch('/api/brightness?value='+this.value)">
<h3>BPM Control</h3>
<label><input type="radio" name="bpmMode" id="autoMode" onchange="fetch('/api/bpm-mode?manual=0')" checked> AUTO</label>
<label><input type="radio" name="bpmMode" id="manualMode" onchange="fetch('/api/bpm-mode?manual=1')"> MANUAL</label>
<div class="tapBox"><div id="tapBpm">-- BPM</div><button type="button" class="tapButton" onclick="tap()">TAP BEAT</button><button type="button" onclick="fetch('/api/tap-reset').then(refreshBPM)">Nullstill</button></div>
<h3>LED-stripe</h3>
<label for="ledCountInput">Antall LED (1–300)</label><input id="ledCountInput" type="number" min="1" max="300" step="1" value="62" style="width:100%;padding:10px;font:inherit;border-radius:8px">
<label for="ledPinInput">ESP32 GPIO (datapinne)</label><select id="ledPinInput" style="width:100%;padding:10px;font:inherit;border-radius:8px">
<option value="0">GPIO0</option><option value="1" selected>GPIO1 (standard)</option><option value="2">GPIO2</option><option value="3">GPIO3</option><option value="4">GPIO4</option><option value="5">GPIO5</option><option value="6">GPIO6</option><option value="7">GPIO7</option><option value="8">GPIO8</option><option value="9">GPIO9</option><option value="10">GPIO10</option><option value="11">GPIO11</option><option value="12">GPIO12</option><option value="13">GPIO13</option><option value="14">GPIO14</option><option value="15">GPIO15</option><option value="16">GPIO16</option><option value="17">GPIO17</option><option value="18">GPIO18</option><option value="21">GPIO21</option></select>
<p class="help">Velg kun en fysisk tilgjengelig, ledig GPIO på kortet. GPIO19/20 (USB) er utelatt. Endringer lagres og krever omstart.</p>
<button type="button" class="saveLed" onclick="saveLedConfig()">Lagre LED-oppsett og start på nytt</button><p class="help" id="ledConfigStatus"></p>
<h3>Skjerm &amp; App</h3><p class="help">Åpne MusicLED fra Hjem-skjermen på iPhone for webappvisning.</p>
<h3>Nettverk</h3><a href="/wifi">Endre Wi-Fi-nettverk</a>
<h3>Om</h3><p class="help">MusicLED v2.0 – Random Play</p><p><a href="https://github.com/kagjerde/M5-stick-test-LED-strip" target="_blank" rel="noopener noreferrer">GitHub – MusicLED kildekode</a></p>
</section></main>
<script>
let active=)HTML");
    h += String((int)mode);
    h += F(R"HTML(;
function highlight(n){active=n;document.querySelectorAll('[data-mode]').forEach(b=>b.classList.toggle('active',Number(b.dataset.mode)===n));}
const waveColors=['CLASSIC','BLUE','RED','PURPLE','GOLD','RAINBOW'];
const waveVariants=['STANDARD','TOP','BOTTOM','PING PONG'];
let waveIndex=5,waveStyle=0,vuStyle=0;
const vuColors=['CLASSIC','BLUE','RED','PURPLE','GOLD','RAINBOW'];
const particleColors=['CLASSIC','BLUE','RED','PURPLE','GOLD','RAINBOW'];
const particleVariants=['BASS EXPLOSION','DIFFUSION','BREATHE','STREAMER','BOUNCING BALLS','FIREWORKS'];
let particleStyle=0;
const bpmColors=['WHITE','RED / BLUE','RAINBOW'];
let bpmColorIndex=)HTML");
    h += String((int)bpmColorVariant);
    h += F(R"HTML(;
let vuIndex=)HTML");
    // Initial VU color is injected by C++ for consistency with the physical button.
    const int vuIndex = vuColorMode == VU_CLASSIC ? 0 :
        vuColorMode == VU_BLUE ? 1 : vuColorMode == VU_RED ? 2 :
        vuColorMode == VU_PURPLE ? 3 : vuColorMode == VU_GOLD ? 4 : 5;
    h += String(vuIndex);
    h += F(R"HTML(;
let energyCount=)HTML");
    h += String((int)energyFlashVariant);
    h += F(R"HTML(;
let particleIndex=)HTML");
    h += String((int)particleColor);
    h += F(R"HTML(;
let bpmStyle=0;
let randomEnabled=false;
function showRandom(enabled){randomEnabled=enabled;const b=document.getElementById('randomBtn');b.classList.toggle('on',enabled);b.setAttribute('aria-pressed',String(enabled));}
async function syncRandom(){try{const r=await fetch('/api/random-state',{cache:'no-store'});if(!r.ok)return;const d=await r.json();showRandom(d.enabled);highlight(d.mode);waveStyle=d.waveVariant;waveIndex=d.waveColor;vuStyle=d.vuCentered?1:0;vuIndex=d.vuColor;particleStyle=d.particleVariant;particleIndex=d.particleColor;bpmColorIndex=d.bpmColor;energyCount=d.energyFlashes;updateVUHint();updateWaveHint();updateParticleHint();document.querySelector('[data-mode="3"] .hint').textContent=bpmStyle?'HOLD':'BLINK';document.querySelector('[data-mode="4"] .hint').textContent=energyCount+' FLASH'+(energyCount===1?'':'ES');}catch(e){}}
async function randomPress(){try{const r=await fetch('/api/random-next',{cache:'no-store'});if(r.ok)await syncRandom();}catch(e){}}
function showSettings(){
 document.getElementById('paletteView').hidden=true;
 document.getElementById('homeView').hidden=true;
 document.getElementById('settingsView').hidden=false;
 refreshSettings();
}
function showHome(){
 document.getElementById('paletteView').hidden=true;
 document.getElementById('settingsView').hidden=true;
 document.getElementById('homeView').hidden=false;
}

// Palette metadata only (step 2A); no changes to LED rendering or effect algorithms.
const paletteGroups=[
 ['Midlertidig · alle effekter',[
 ['Rainbow',['#f22','#fa0','#ff0','#0d5','#09e','#65f','#c0f','#f3a']],
 ['Party',['#f0a','#fa0','#90f','#0df','#f25','#ff0','#0f9','#f0a']],
 ['Ocean',['#001b54','#0077b6','#00b4d8','#90e0ef','#48cae4','#023e8a','#0077b6','#caf0f8']],
 ['Forest',['#123524','#245c35','#3e8b46','#73a942','#b7d35b','#276749','#194d33','#a0b75e']]
 ]],
 ['Multicolor',[
 ['Sunset',['#27104e','#702b86','#b33986','#f25f5c','#ff914d','#ffcf70','#f76d39','#38164b']],
 ['Lava',['#350000','#9c1600','#ff4000','#ff8c00','#ffe080','#ff5400','#b32100','#350000']],
 ['Aurora',['#08213e','#007c80','#00bb83','#8bffba','#8a53d6','#1bb1a7','#3b358e','#08213e']],
 ['C9',['#f22525','#ffb600','#0ab64a','#2567ed']]
 ]],
 ['Fire farger',[
 ['Neon',['#ff00aa','#00e5ff','#9d4edd','#ffee00']],
 ['Tropical',['#00d1b2','#ffb703','#fb5607','#3a86ff']],
 ['Icefire',['#002b7f','#008cff','#8ceeff','#ffffff']],
 ['Retro',['#f9c74f','#f9844a','#f94144','#577590']],
 ['Candy',['#ff65b3','#b388eb','#66d9e8','#ffe66d']],
 ['Autumn',['#8c2f00','#d65a00','#ffa62b','#ffdf80']],
 ['Night',['#061a40','#214e8a','#6844b8','#c44dce']],
 ['Vintage',['#9c4637','#d29b6f','#e8d8b8','#647e73']]
 ]],
 ['En farge · nyanser',[
 ['Red',['#220000','#620000','#a50000','#ff3030']],
 ['Blue',['#000c35','#002c80','#1464d2','#8bcaff']],
 ['Green',['#00200e','#00662c','#00b85c','#a2ffc1']],
 ['Purple',['#18002b','#4c148c','#9146d8','#e1b7ff']],
 ['Amber',['#321000','#8c3900','#e98100','#ffe29b']],
 ['Cyan',['#002b32','#006b80','#00b9d0','#a4f7ff']],
 ['Pink',['#390a27','#8e195b','#e5489a','#ffd1e8']],
 ['White',['#141414','#525252','#aaaaaa','#ffffff']]
 ]],
 ['To farger · 50/50',[
 ['Red / Blue',['#ff2435','#ff2435','#176bff','#176bff']],
 ['Blue / Gold',['#176bff','#176bff','#ffc22b','#ffc22b']],
 ['Pink / Cyan',['#ff30b4','#ff30b4','#00dbeb','#00dbeb']],
 ['Purple / Green',['#8b45e8','#8b45e8','#30dc79','#30dc79']],
 ['Orange / Teal',['#ff8a28','#ff8a28','#00a6a6','#00a6a6']],
 ['Red / White',['#f5222d','#f5222d','#ffffff','#ffffff']],
 ['Gold / Purple',['#ffd046','#ffd046','#7029bd','#7029bd']],
 ['Blue / White',['#2469e8','#2469e8','#ffffff','#ffffff']]
 ]]
];
let paletteSaved=[0,0,0,0,0,0],paletteTemporary=-1,paletteReversed=[false,false,false,false,false,false];
const paletteNames=['WAVE','VU METER','PARTICLES','BPM AUTO','ENERGY','FIRE'];
function renderPalettes(){
 const root=document.getElementById('paletteGroups');root.replaceChildren();
 const reverseButton=document.getElementById('paletteReverseButton');
 reverseButton.textContent=paletteReversed[active]?'⇅ Reversert · trykk for normal':'⇅ Normal · trykk for reversering';
 document.getElementById('paletteModeName').textContent='Aktiv effekt: '+paletteNames[active]+' · Lagret: '+(paletteSaved[active]===0?'DEFAULT':paletteLabel(paletteSaved[active]));
 let id=1;
 const defaultGroup=document.createElement('section');defaultGroup.className='paletteGroup';
 defaultGroup.innerHTML='<h3>Standard</h3>';
 const defaultBtn=document.createElement('button');defaultBtn.className='paletteTile'+(paletteTemporary<0&&paletteSaved[active]===0?' selected':'');
 defaultBtn.textContent='DEFAULT';defaultBtn.onclick=()=>savePalette(0);defaultGroup.appendChild(defaultBtn);root.appendChild(defaultGroup);
 paletteGroups.forEach((group,g)=>{
 const section=document.createElement('section');section.className='paletteGroup';
 const title=document.createElement('h3');title.textContent=group[0];section.appendChild(title);
 const grid=document.createElement('div');grid.className='paletteGrid';
 group[1].forEach(p=>{
 const pid=id++;const btn=document.createElement('button');
 btn.className='paletteTile'+((paletteTemporary>=0?paletteTemporary===pid:paletteSaved[active]===pid)?' selected':'');
 btn.title=p[0];btn.onclick=()=>savePalette(pid);
 const sw=document.createElement('span');sw.className='paletteSwatch';
 p[1].forEach(c=>{const cell=document.createElement('i');cell.style.background=c;sw.appendChild(cell);});
 const label=document.createElement('span');label.className='paletteLabel';label.textContent=p[0];
 btn.appendChild(sw);btn.appendChild(label);grid.appendChild(btn);
 });section.appendChild(grid);root.appendChild(section);
 });
}
function paletteLabel(id){let n=1;for(const group of paletteGroups)for(const p of group[1]){if(n++===id)return p[0];}return 'DEFAULT';}
async function loadPalettes(){try{
 const r=await fetch('/api/palettes',{cache:'no-store'});if(!r.ok)throw Error('Kunne ikke lese paletter');
 const d=await r.json();paletteSaved=d.saved;paletteTemporary=d.temporary;paletteReversed=d.reverse||[false,false,false,false,false,false];renderPalettes();
}catch(e){document.getElementById('paletteStatus').textContent=e.message;}}
function showPalettes(){
 document.getElementById('homeView').hidden=true;
 document.getElementById('settingsView').hidden=true;
 document.getElementById('paletteView').hidden=false;
 loadPalettes();
}
async function togglePaletteReverse(){try{
 const value=paletteReversed[active]?0:1;
 const r=await fetch("/api/palette-reverse?mode="+active+"&value="+value,{method:"POST",cache:"no-store"});
 if(!r.ok)throw Error(await r.text());
 await loadPalettes();
}catch(e){document.getElementById("paletteStatus").textContent="Feil: "+e.message;}}
async function savePalette(id){try{
 const r=await fetch('/api/palette?mode='+active+'&id='+id,{method:'POST',cache:'no-store'});
 if(!r.ok)throw Error(await r.text());
 document.getElementById('paletteStatus').textContent=id>0&&id<5?'Midlertidig global palett valgt':'Palett lagret for '+paletteNames[active];
 await loadPalettes();
}catch(e){document.getElementById('paletteStatus').textContent='Feil: '+e.message;}}

async function refreshSettings(){
 try{const r=await fetch('/api/settings',{cache:'no-store'});if(!r.ok)return;
 const d=await r.json();
 document.getElementById('dynInput').value=d.dynamics;
 document.getElementById('dynVal').textContent=d.dynamics+'%';
 document.getElementById('brightInput').value=d.brightness;
 document.getElementById('level').textContent=d.brightness;
 document.getElementById(d.manual?'manualMode':'autoMode').checked=true;
 document.getElementById('ledCountInput').value=d.ledCount;
 document.getElementById('ledPinInput').value=d.ledPin;
 }catch(e){}
}
async function saveLedConfig(){
 const count=Number(document.getElementById('ledCountInput').value);
 const pin=Number(document.getElementById('ledPinInput').value);
 const status=document.getElementById('ledConfigStatus');
 if(!Number.isInteger(count)||count<1||count>300){status.textContent='Velg 1–300 LED.';return;}
 status.textContent='Lagrer og starter på nytt…';
 try{const r=await fetch('/api/led-config?count='+count+'&pin='+pin,{method:'POST',cache:'no-store'});
 if(!r.ok)throw Error(await r.text());
 status.textContent='Lagret. Vent noen sekunder, og åpne MusicLED på nytt.';
 }catch(e){status.textContent='Kunne ikke lagre: '+e.message;}
}
async function tap(){try{await fetch('/api/tap',{cache:'no-store'});document.getElementById('manualMode').checked=true;refreshBPM();}catch(e){}}
function updateVUHint(){document.querySelector('[data-mode="1"] .hint').textContent=vuStyle?'CENTER':'NORMAL';}
function updateParticleHint(){document.querySelector('[data-mode="2"] .hint').textContent=particleVariants[particleStyle];}
function updateWaveHint(){document.querySelector('[data-mode="0"] .hint').textContent=waveVariants[waveStyle];}
// v2.1 step 1: single taps only. Active mode cycles its variant; colors remain unchanged.
async function choose(n){try{
 showRandom(false);
 const already=active===n;
 let url='/api/mode?value='+n;
 if(already && n===0)url='/api/wave-variant';
 else if(already && n===1)url='/api/vu-variant';
 else if(already && n===2)url='/api/particle-variant';
 else if(already && n===3)url='/api/bpm-variant';
 // ENERGY cycles flashes through /api/mode. FIRE has no variant.
 const r=await fetch(url,{cache:'no-store'});if(!r.ok)throw Error();
 if(already){
   if(n===0)waveStyle=(waveStyle+1)%waveVariants.length;
   else if(n===1)vuStyle=1-vuStyle;
   else if(n===2)particleStyle=(particleStyle+1)%particleVariants.length;
   else if(n===3)bpmStyle=1-bpmStyle;
   else if(n===4)energyCount=energyCount%4+1;
 }
 highlight(n);updateVUHint();updateWaveHint();updateParticleHint();
 document.querySelector('[data-mode="3"] .hint').textContent='BPM AUTO';
 document.querySelector('[data-mode="4"] .hint').textContent=energyCount+' FLASH'+(energyCount===1?'':'ES');
 document.querySelector('[data-mode="5"] .hint').textContent='FIRE & SPARKS';
}catch(e){console.warn('Ingen kontakt med MusicLED',e);}}
highlight(active);updateVUHint();updateWaveHint();updateParticleHint();
document.querySelector('[data-mode="3"] .hint').textContent='BPM AUTO';
document.querySelector('[data-mode="4"] .hint').textContent=energyCount+' FLASH'+(energyCount===1?'':'ES');
document.querySelector('[data-mode="5"] .hint').textContent='FIRE & SPARKS';
async function refreshBPM(){try{const r=await fetch('/api/bpm',{cache:'no-store'});if(!r.ok)return;const d=await r.json();document.getElementById('bpmDisplay').textContent=d.bpm>0?Math.round(d.bpm)+' BPM':'-- BPM';document.getElementById('bpmSource').textContent=d.manual?'MANUAL':'AUTO';document.getElementById('tapBpm').textContent=d.bpm>0?Math.round(d.bpm)+' BPM':'-- BPM';}catch(e){}}
refreshBPM();syncRandom();setInterval(refreshBPM,3000);setInterval(syncRandom,2000);
</script></body></html>)HTML");
    webServer.send(200, "text/html; charset=utf-8", h);
}

void showSettingsPage() {
    String h = htmlHeader("MusicLED innstillinger");
    h += F(R"HTML(<p><a href="/">&#8592; Tilbake til pads</a></p>
<h3>LED &amp; Audio</h3><label>Dynamikk: <b id="dynVal">)HTML");
    h += String(dynamicsPercent);
    h += F(R"HTML(%</b></label><input type="range" min="0" max="100" step="5" value=")HTML");
    h += String(dynamicsPercent);
    h += F(R"HTML(" oninput="document.getElementById('dynVal').textContent=this.value" onchange="fetch('/api/dynamics?value='+this.value)">
<p style="font-size:12px;color:#9baac3">50% tilsvarer dagens dynamikk.</p>
<label>Lysstyrke: <b id="level">)HTML");
    h += String(currentBrightness);
    h += F(R"HTML(</b></label><input type="range" min="1" max="255" value=")HTML");
    h += String(currentBrightness);
    h += F(R"HTML(" oninput="document.getElementById('level').textContent=this.value" onchange="fetch('/api/brightness?value='+this.value)">
<hr><h3>BPM Control</h3>
<label><input style="width:auto" type="radio" name="bpmMode" value="auto" )HTML");
    if (!manualBPM) h += F("checked");
    h += F(R"HTML( onchange="fetch('/api/bpm-mode?manual=0')"> AUTO</label>
<label><input style="width:auto" type="radio" name="bpmMode" value="manual" )HTML");
    if (manualBPM) h += F("checked");
    h += F(R"HTML( onchange="fetch('/api/bpm-mode?manual=1')"> MANUAL</label>
<div style="text-align:center;padding:12px;background:#15253a;border-radius:12px">
<div id="tapBpm" style="font-size:32px;font-weight:700">-- BPM</div>
<button style="width:100%;height:90px;font-size:24px" onclick="tap()">TAP BEAT</button>
<button onclick="fetch('/api/tap-reset').then(refreshBpm)">Nullstill</button>
</div>
<hr><h3>Skjerm &amp; App</h3>
<button id="fullscreenBtn" onclick="toggleFullscreen()">Fullskjerm av/på</button>
<p id="fullscreenHelp" style="font-size:13px;color:#a9b9d4">På iPhone: Åpne i Safari, velg Del og Legg til på Hjem-skjerm. Åpne deretter fra ikonet.</p>
<hr><h3>Nettverk</h3><p><a href="/wifi">Endre Wi-Fi-nettverk</a></p>
<script>
async function refreshBpm(){try{const r=await fetch('/api/bpm',{cache:'no-store'});const d=await r.json();document.getElementById('tapBpm').textContent=d.bpm>0?Math.round(d.bpm)+' BPM':'-- BPM';}catch(e){}}
async function tap(){await fetch('/api/tap');document.querySelector('input[value="manual"]').checked=true;refreshBpm();}
async function toggleFullscreen(){if(document.fullscreenElement){await document.exitFullscreen();}else if(document.documentElement.requestFullscreen){await document.documentElement.requestFullscreen();}else{document.getElementById('fullscreenHelp').textContent='På iPhone: Safari → Del → Legg til på Hjem-skjerm. Fullskjermknappen støttes ikke av iOS Chrome.';}}
refreshBpm();setInterval(refreshBpm,3000);
</script>)HTML");
    h += htmlFooter();
    webServer.send(200, "text/html; charset=utf-8", h);
}

void beginSetupAP() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(SETUP_SSID, SETUP_PASSWORD);
    dnsServer.start(53, "*", WiFi.softAPIP());
    setupAPActive = true;
    Serial.print("Setup AP: "); Serial.println(SETUP_SSID);
    Serial.print("Setup IP: "); Serial.println(WiFi.softAPIP());
}

void beginWiFi() {
    wifiPreferences.begin("musicled-wifi", false);
    String ssid = wifiPreferences.getString("ssid", "");
    String password = wifiPreferences.getString("password", "");
    WiFi.setHostname(HOST_NAME);
    if (!ssid.isEmpty()) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
        Serial.print("Connecting to "); Serial.println(ssid);
        uint32_t started = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - started < 10000) {
            delay(100);
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("MusicLED IP: "); Serial.println(WiFi.localIP());
        if (MDNS.begin(HOST_NAME)) {
            MDNS.addService("http", "tcp", 80);
            Serial.println("Web: http://musicled.local");
        }
    } else {
        beginSetupAP();
    }

    webServer.on("/apple-touch-icon.png", HTTP_GET, []() {
        webServer.sendHeader("Cache-Control", "public, max-age=3600");
        webServer.setContentLength(sizeof(MUSICLED_ICON_PNG));
        webServer.send(200, "image/png", "");
        webServer.client().write(MUSICLED_ICON_PNG, sizeof(MUSICLED_ICON_PNG));
    });
    webServer.on("/", HTTP_GET, []() {
        webServer.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
        if (setupAPActive) showSetupPage();
        else showControlPage();
    });
    webServer.on("/settings", HTTP_GET, []() {
        if (setupAPActive) showSetupPage();
        else { webServer.sendHeader("Location", "/", true); webServer.send(302, "text/plain", ""); }
    });
    webServer.on("/api/settings", HTTP_GET, []() {
        String json = String("{\"dynamics\":") + String((int)dynamicsPercent) +
            ",\"brightness\":" + String((int)currentBrightness) +
            ",\"manual\":" + (manualBPM ? "true" : "false") +
            ",\"ledCount\":" + String(NUM_LEDS) +
            ",\"ledPin\":" + String((int)ledPin) + "}";
        webServer.sendHeader("Cache-Control", "no-store");
        webServer.send(200, "application/json", json);
    });
    webServer.on("/api/palettes", HTTP_GET, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        String json = "{\"saved\":[";
        for (uint8_t i=0;i<6;i++) { if (i) json += ","; json += String(effectPalette[i]); }
        json += "],\"temporary\":" + String((int)temporaryGlobalPalette) + ",\"reverse\":[";
        for (uint8_t i=0;i<6;i++) { if (i) json += ","; json += paletteReverse[i] ? "true" : "false"; }
        json += "]}";
        webServer.sendHeader("Cache-Control", "no-store");
        webServer.send(200, "application/json", json);
    });
    webServer.on("/api/palette-reverse", HTTP_POST, []() {
        if (setupAPActive) { webServer.send(403,"text/plain","Setup mode"); return; }
        if (!webServer.hasArg("mode") || !webServer.hasArg("value")) { webServer.send(400,"text/plain","Missing arguments"); return; }
        String m=webServer.arg("mode"),v=webServer.arg("value");
        if (m.length()!=1 || !isDigit(m[0]) || (v!="0" && v!="1")) { webServer.send(400,"text/plain","Invalid arguments"); return; }
        int index=m.toInt();if(index>=6) { webServer.send(400,"text/plain","Invalid mode"); return; }
        paletteReverse[index]=(v=="1");
        String key="reverse"+String(index);
        palettePreferences.putBool(key.c_str(),paletteReverse[index]);
        paletteRevision++;
        webServer.send(200,"text/plain","OK");
    });
    webServer.on("/api/palette", HTTP_POST, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        if (!webServer.hasArg("mode") || !webServer.hasArg("id")) {
            webServer.send(400, "text/plain", "Missing mode/id"); return;
        }
        String m = webServer.arg("mode"), p = webServer.arg("id");
        for (unsigned i=0;i<m.length();i++) if (!isDigit(m[i])) { webServer.send(400,"text/plain","Invalid mode"); return; }
        for (unsigned i=0;i<p.length();i++) if (!isDigit(p[i])) { webServer.send(400,"text/plain","Invalid palette"); return; }
        int modeId=m.toInt(), paletteId=p.toInt();
        if (modeId<0 || modeId>=6 || paletteId<0 || paletteId>32) {
            webServer.send(400,"text/plain","Out of range"); return;
        }
        if (paletteId>=1 && paletteId<=4) {
            temporaryGlobalPalette=(int8_t)paletteId;
        } else {
            temporaryGlobalPalette=-1;
            effectPalette[modeId]=(uint8_t)paletteId;
            String key="effect"+String(modeId);
            palettePreferences.putUChar(key.c_str(),(uint8_t)paletteId);
        }
        paletteRevision++;
        webServer.send(200,"text/plain","OK");
    });
    webServer.on("/api/led-config", HTTP_POST, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        if (!webServer.hasArg("count") || !webServer.hasArg("pin")) {
            webServer.send(400, "text/plain", "Missing count/pin"); return;
        }
        String countArg = webServer.arg("count");
        String pinArg = webServer.arg("pin");
        for (unsigned i=0; i<countArg.length(); ++i) if (!isDigit(countArg[i])) {
            webServer.send(400, "text/plain", "Invalid LED count"); return;
        }
        for (unsigned i=0; i<pinArg.length(); ++i) if (!isDigit(pinArg[i])) {
            webServer.send(400, "text/plain", "Invalid GPIO"); return;
        }
        int count = countArg.toInt();
        int pin = pinArg.toInt();
        if (count < 1 || count > MAX_LEDS || pin < 0 || pin > 21 || pin == 19 || pin == 20) {
            webServer.send(400, "text/plain", "LED count or GPIO out of range"); return;
        }
        ledPreferences.putUShort("count", (uint16_t)count);
        ledPreferences.putUChar("pin", (uint8_t)pin);
        webServer.send(200, "text/plain", "Saved, restarting");
        webServer.client().flush();
        delay(350);
        ESP.restart();
    });
    webServer.on("/wifi", HTTP_GET, showSetupPage);
    webServer.on("/save", HTTP_POST, []() {
        String ssid = webServer.arg("ssid");
        String password = webServer.arg("password");
        if (ssid.isEmpty() || ssid.length() > 32 || password.length() > 63) {
            webServer.send(400, "text/plain", "Ugyldig SSID/passord"); return;
        }
        wifiPreferences.putString("ssid", ssid);
        wifiPreferences.putString("password", password);
        webServer.send(200, "text/html; charset=utf-8",
            "<h2>Lagret!</h2><p>Enheten starter paa nytt. Koble telefonen til hjemmenettverket.</p>");
        delay(500);
        ESP.restart();
    });
    webServer.on("/api/random-next", HTTP_GET, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        randomPlay = true;
        // Clear pending manual commands to prevent them overriding this selection.
        requestedMode = -1;
        requestedColor = requestedWaveColor = requestedWaveVariant = false;
        requestedVUVariant = requestedCometColor = requestedBPMColor = false;
        requestedParticleVariant = requestedEnergyCycle = false;
        selectRandomEffect();
        webServer.sendHeader("Cache-Control", "no-store");
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/random-state", HTTP_GET, []() {
        int vuc = vuColorMode == VU_CLASSIC ? 0 : vuColorMode == VU_BLUE ? 1 :
            vuColorMode == VU_RED ? 2 : vuColorMode == VU_PURPLE ? 3 :
            vuColorMode == VU_GOLD ? 4 : 5;
        String json = String("{\"enabled\":") + (randomPlay ? "true" : "false") +
            ",\"mode\":" + String((int)mode) +
            ",\"waveVariant\":" + String((int)waveVariant) +
            ",\"waveColor\":" + String((int)waveColor) +
            ",\"vuCentered\":" + (vuCentered ? "true" : "false") +
            ",\"vuColor\":" + String(vuc) +
            ",\"particleVariant\":" + String((int)particleVariant) +
            ",\"particleColor\":" + String((int)particleColor) +
            ",\"bpmColor\":" + String((int)bpmColorVariant) +
            ",\"energyFlashes\":" + String((int)energyFlashVariant) + "}";
        webServer.sendHeader("Cache-Control", "no-store");
        webServer.send(200, "application/json", json);
    });
    webServer.on("/api/mode", HTTP_GET, []() {
        randomPlay = false;
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        int value = webServer.arg("value").toInt();
        if (!webServer.hasArg("value") || value < 0 || value > 5) {
            webServer.send(400, "text/plain", "Invalid mode"); return;
        }
        if (value == ENERGY_STROBE && mode == ENERGY_STROBE) requestedEnergyCycle = true;
        else requestedMode = value;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/wave-color", HTTP_GET, []() {
        randomPlay = false;
        requestedWaveColor=true;webServer.send(200,"text/plain","OK");
    });
    webServer.on("/api/wave-variant", HTTP_GET, []() {
        randomPlay = false;
        requestedWaveVariant=true;webServer.send(200,"text/plain","OK");
    });
    webServer.on("/api/vu-variant", HTTP_GET, []() {
        randomPlay = false;
        requestedVUVariant=true;webServer.send(200,"text/plain","OK");
    });
    webServer.on("/api/color", HTTP_GET, []() {
        randomPlay = false;
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        requestedColor = true;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/comet-color", HTTP_GET, []() {
        randomPlay = false;
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        requestedCometColor = true;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/particle-variant", HTTP_GET, []() {
        randomPlay = false; requestedParticleVariant=true; webServer.send(200,"text/plain","OK"); });
    webServer.on("/api/bpm-variant", HTTP_GET, []() {
        randomPlay=false;
        bpmDisplayVariant=1-bpmDisplayVariant;
        paletteRevision++;
        webServer.send(200,"text/plain","OK");
    });
    webServer.on("/api/bpm-color", HTTP_GET, []() {
        randomPlay = false;
        requestedBPMColor = true;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/dynamics", HTTP_GET, []() {
        if (!webServer.hasArg("value")) { webServer.send(400, "text/plain", "Missing"); return; }
        dynamicsPercent = (uint8_t)constrain(webServer.arg("value").toInt(), 0, 100);
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/bpm-mode", HTTP_GET, []() {
        manualBPM = webServer.arg("manual") == "1";
        nextBPMFlash = 0;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/tap", HTTP_GET, []() {
        uint32_t now = millis();
        if (lastTapTime == 0 || now - lastTapTime > 2500) tapCount = 0;
        lastTapTime = now;
        if (tapCount < 6) tapTimes[tapCount++] = now;
        else {
            for (int i = 1; i < 6; i++) tapTimes[i-1] = tapTimes[i];
            tapTimes[5] = now;
        }
        if (tapCount >= 2) {
            float bpm = 60000.0f * (tapCount - 1) / (tapTimes[tapCount-1] - tapTimes[0]);
            if (bpm >= 40 && bpm <= 240) manualBPMValue = bpm;
        }
        manualBPM = true;
        nextBPMFlash = 0;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/tap-reset", HTTP_GET, []() {
        tapCount = 0; lastTapTime = 0;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/bpm", HTTP_GET, []() {
        float bpm = manualBPM ? manualBPMValue : estimatedBPM;
        String json = String("{\"bpm\":") + String(bpm, 1)
          + ",\"manual\":" + (manualBPM ? "true" : "false") + "}";
        webServer.sendHeader("Cache-Control", "no-store");
        webServer.send(200, "application/json", json);
    });
    webServer.on("/api/brightness", HTTP_GET, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        int value = webServer.arg("value").toInt();
        if (!webServer.hasArg("value") || value < 1 || value > 255) {
            webServer.send(400, "text/plain", "Invalid brightness"); return;
        }
        requestedBrightness = (uint8_t)value;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.onNotFound([]() {
        if (setupAPActive) {
            webServer.sendHeader("Location", "http://192.168.4.1/", true);
            webServer.send(302, "text/plain", "");
        } else webServer.send(404, "text/plain", "Not found");
    });
    webServer.begin();
    webStarted = true;
}

void handleWeb() {
    if (!webStarted) return;
    if (setupAPActive) dnsServer.processNextRequest();
    webServer.handleClient();
    if (!setupAPActive && WiFi.status() != WL_CONNECTED) {
        beginSetupAP();
    }
}

// =====================================================
// BUTTONS
// =====================================================

void handleButtons() {
    if (M5.BtnA.wasPressed()) {
        randomPlay = false;
        requestedMode = ((int)mode + 1) % 6;
    }
    if (M5.BtnB.wasPressed()) randomPlay = false;
    if (mode == ENERGY_STROBE && M5.BtnB.wasPressed()) {
        requestedEnergyCycle = true;
    }

    if (mode == COMET_BEAT && M5.BtnB.wasPressed()) {
        particleColor=(particleColor+1)%6;
    }
    if (
        mode == VU_METER &&
        M5.BtnB.wasPressed()
    ) {
        nextVUColor();

        silenceStart = 0;
        silenceTriggered = false;
    }
}

// =====================================================
// SETUP
// =====================================================

void setup() {
    Serial.begin(115200);
    randomSeed(esp_random());

    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Display.setRotation(1);

    screenW =
        M5.Display.width();

    screenH =
        M5.Display.height();

    canvas.setColorDepth(16);

    if (!canvas.createSprite(
        screenW,
        screenH
    )) {
        M5.Display.fillScreen(
            TFT_BLACK
        );

        M5.Display.println(
            "CANVAS ERROR"
        );

        while (true) {
            delay(1000);
        }
    }

    // Load saved LED configuration before initializing the NeoPixel driver.
    palettePreferences.begin("musicled-pal", false);
    for (uint8_t i=0;i<6;i++) {
        String key = "effect" + String(i);
        uint8_t value = palettePreferences.getUChar(key.c_str(), 0);
        effectPalette[i] = (value >= 5 && value <= 32) ? value : 0;
        String reverseKey="reverse"+String(i);
        paletteReverse[i]=palettePreferences.getBool(reverseKey.c_str(),false);
    }
    ledPreferences.begin("musicled-led", false);
    NUM_LEDS = constrain((int)ledPreferences.getUShort("count", DEFAULT_NUM_LEDS), 1, (int)MAX_LEDS);
    int storedPin = (int)ledPreferences.getUChar("pin", DEFAULT_LED_PIN);
    // GPIO 19/20 are USB on ESP32-S3; exclude them from the UI/configuration.
    ledPin = (storedPin >= 0 && storedPin <= 21 && storedPin != 19 && storedPin != 20)
        ? (uint8_t)storedPin : DEFAULT_LED_PIN;
    strip.updateLength(NUM_LEDS);
    strip.setPin(ledPin);
    strip.begin();
    strip.setBrightness(
        BRIGHTNESS
    );

    strip.clear();
    strip.show();

    auto micCfg =
        M5.Mic.config();

    micCfg.sample_rate =
        SAMPLE_RATE;

    micCfg.stereo = false;

    M5.Mic.config(micCfg);

    if (!M5.Mic.begin()) {
        M5.Display.fillScreen(
            TFT_BLACK
        );

        M5.Display.println(
            "MIC ERROR"
        );

        while (true) {
            delay(1000);
        }
    }

    xTaskCreatePinnedToCore(
        ledTask,
        "LEDTask",
        4096,
        nullptr,
        2,
        &ledTaskHandle,
        0
    );

    Serial.println(
        "Music Controller - BPM Auto v2"
    );

    beginWiFi();
    updateScreen();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
    M5.update();
    handleButtons();
    if (requestedMode >= 0) {
        int next = requestedMode;
        requestedMode = -1;
        applyModeChange((LedMode)next);
    }
    if (requestedEnergyCycle) {
        requestedEnergyCycle = false;
        energyFlashVariant = (energyFlashVariant % 4) + 1;
    }
    if (requestedWaveColor) {requestedWaveColor=false;waveColor=(waveColor+1)%6;}
    if (requestedWaveVariant) {requestedWaveVariant=false;waveVariant=(waveVariant+1)%4;resetBPM();resetWaveRunners();}
    if (requestedVUVariant) {requestedVUVariant=false;vuCentered=!vuCentered;}
    if (requestedColor) {
        requestedColor = false;
        if (mode != VU_METER) applyModeChange(VU_METER);
        nextVUColor();
        silenceStart = 0;
        silenceTriggered = false;
    }
    if (requestedParticleVariant) {requestedParticleVariant=false;if(mode!=COMET_BEAT)applyModeChange(COMET_BEAT);particleVariant=(particleVariant+1)%6;resetParticles();}
    if (requestedCometColor) {
        requestedCometColor = false;
        if (mode != COMET_BEAT) applyModeChange(COMET_BEAT);
        particleColor=(particleColor+1)%6;
    }
    if (requestedBPMColor) {
        requestedBPMColor = false;
        if (mode != BPM_AUTO) applyModeChange(BPM_AUTO);
        bpmColorVariant = (bpmColorVariant + 1) % 3;
    }
    handleWeb();

    analyzeAudio();
    handleWeb(); // service requests again after the blocking microphone read
    uint32_t audioNow = millis();
    // Adaptive microphone noise gate. RMS above a fixed threshold alone
    // can falsely classify fan/electronic noise as continuous music.
    if (!musicGateInitialized) {
        musicNoiseFloor = rms;
        musicGateInitialized = true;
    }
    // Follow falling noise quickly and rising noise very slowly.
    float noiseAlpha = (rms < musicNoiseFloor) ? 0.08f : 0.0003f;
    musicNoiseFloor += (rms - musicNoiseFloor) * noiseAlpha;
    float activityThreshold = max(BPM_MIN_RMS, musicNoiseFloor * 1.8f + 130.0f);
    if (rms > activityThreshold) lastMusicActivity = audioNow;
    audioIsActive = lastMusicActivity != 0 && audioNow - lastMusicActivity < MUSIC_SILENCE_MS;
    updateRandomPlay(audioNow);

    if (
        (mode == RAINBOW_MUSIC && waveVariant == 0) ||
        mode == VU_METER
    ) {
        updatePeak();
    }

    if (mode == BPM_AUTO || mode == COMET_BEAT || (mode == RAINBOW_MUSIC && waveVariant != 0)) {
        updateBPMClock();
    }

    uint32_t now = millis();

    if (
        now - lastScreen >= 40
    ) {
        lastScreen = now;
        updateScreen();
    }
}