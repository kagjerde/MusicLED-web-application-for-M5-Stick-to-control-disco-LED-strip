
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

// =====================================================
// HARDWARE
// =====================================================

#define LED_PIN 1
#define NUM_LEDS 68
#define BRIGHTNESS 60

#define SAMPLE_RATE 16000
#define SAMPLES 512

Adafruit_NeoPixel strip(
    NUM_LEDS,
    LED_PIN,
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
volatile int requestedMode = -1;
volatile bool requestedColor = false;
volatile bool requestedCometColor = false;
volatile uint8_t requestedBrightness = BRIGHTNESS;
uint8_t currentBrightness = BRIGHTNESS;

const char* modeNames[] = {
    "RAINBOW MUSIC",
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

// Comet Beat: multiple overlapping comets, each with a 30-LED fading tail.
// A new comet is launched for every detected/predicted BPM beat.
// Travel speed follows tempo: one beat advances the head halfway up the strip.
static const int COMET_TAIL_LEDS = 30;
static const int COMET_MAX_ACTIVE = 8;
uint32_t cometStarts[COMET_MAX_ACTIVE] = {0};
uint8_t cometWriteIndex = 0;
uint32_t cometLastBeat = 0;

void resetComets() {
    for (int n = 0; n < COMET_MAX_ACTIVE; n++) cometStarts[n] = 0;
    cometWriteIndex = 0;
    cometLastBeat = bpmFlashCount;
}

void updateComet() {
    const uint32_t now = millis();

    // BPM counter is incremented by the audio/clock loop.
    // Each increment creates a separate comet; no restart of older comets.
    if (cometLastBeat != bpmFlashCount) {
        uint32_t beats = bpmFlashCount - cometLastBeat;
        cometLastBeat = bpmFlashCount;
        if (beats > COMET_MAX_ACTIVE) beats = COMET_MAX_ACTIVE;
        for (uint32_t n = 0; n < beats; n++) {
            cometStarts[cometWriteIndex] = now ? now : 1;
            cometWriteIndex = (cometWriteIndex + 1) % COMET_MAX_ACTIVE;
        }
    }

    // At 120 BPM, one beat is 500 ms; the head advances 34 LEDs
    // during that time, so the previous comet is about halfway
    // when the next launches. Use 500 ms until tempo is established.
    float interval = beatIntervalMs > 0.0f ? beatIntervalMs : 500.0f;
    interval = constrain(interval, 280.0f, 1200.0f);
    const float ledsPerMs = (NUM_LEDS * 0.5f) / interval;

    uint8_t red[NUM_LEDS] = {0};
    uint8_t green[NUM_LEDS] = {0};
    uint8_t blue[NUM_LEDS] = {0};

    for (int n = 0; n < COMET_MAX_ACTIVE; n++) {
        if (!cometStarts[n]) continue;
        float head = (now - cometStarts[n]) * ledsPerMs;
        if (head >= NUM_LEDS + COMET_TAIL_LEDS) {
            cometStarts[n] = 0;
            continue;
        }
        for (int i = 0; i < NUM_LEDS; i++) {
            float distance = head - i;
            if (distance < 0 || distance >= COMET_TAIL_LEDS) continue;

            // Bright head, smooth fading tail, no sudden LED jumps.
            float intensity = 1.0f - distance / COMET_TAIL_LEDS;
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

volatile uint32_t flashStart[2] = {0, 0};
volatile uint32_t flashEnd[2] = {0, 0};

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
                (NUM_LEDS - 1)
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
    }
    else {
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
    bpmDetectedBeats++;

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
        if (mode == COMET_BEAT) bpmFlashCount++;
        requestFlash(FLASH_SINGLE);
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
    if (
        (mode != BPM_AUTO && mode != COMET_BEAT) ||
        !bpmLocked ||
        beatIntervalMs <= 0.0f
    ) {
        return;
    }

    uint32_t now = millis();

    if (now - lastBPMEvidence > BPM_HOLD_MS) {
        // Behold BPM og fortsett med intern klokke.
        // Ikke nullstill rytmen.
        bpmConfidence = 4;
    }   

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
            (uint32_t)beatIntervalMs;

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
                beatIntervalMs;
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
        mode == RAINBOW_MUSIC ||
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
            (mode == BPM_AUTO || mode == COMET_BEAT) &&
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
        (mode == BPM_AUTO || mode == COMET_BEAT) &&
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

        updateBPMDetector(
            bassNormalized,
            dt
        );
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

        strip.setPixelColor(
            i,
            strip.Color(r, g, b)
        );
    }
}

void updateVU() {
    float displayVU =
        expandVU(vu);

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
uint8_t fireHeat[NUM_LEDS] = {};

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
        int y = random(min(NUM_LEDS, 8));
        fireHeat[y] = (uint8_t)min(255L, (long)fireHeat[y] + random(150, 256));
    }

    // Sparse music-reactive sparks, decaying independently of the fire.
    static uint8_t spark[NUM_LEDS] = {};
    float activity = constrain((signalRms - 250.0f) / 4500.0f, 0.0f, 1.0f);
    for (int i = 0; i < NUM_LEDS; ++i) {
        spark[i] = (spark[i] > 38) ? spark[i] - 38 : 0;
    }
    if (activity > 0.08f && random(1000) < (int)(activity * 220.0f)) {
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

void ledTask(void* parameter) {
    LedMode previousMode = RAINBOW_MUSIC;

    bool outputOn = false;

    uint32_t lastNormalLED = 0;

    while (true) {
        if (currentBrightness != requestedBrightness) {
            currentBrightness = requestedBrightness;
            strip.setBrightness(currentBrightness);
        }
        LedMode currentMode = mode;

        if (currentMode != previousMode) {
            strip.clear();
            strip.show();

            outputOn = false;
            if (currentMode == COMET_BEAT) resetComets();
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
                    flashStart[1] =
                        commandTime +
                        ENERGY_SECOND_FLASH_DELAY_MS;

                    flashEnd[1] =
                        flashStart[1] +
                        ENERGY_FLASH_MS;

                    flashCount = 2;
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

            if (shouldBeOn != outputOn) {
                outputOn = shouldBeOn;

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
                                255, 255, 255
                            )
                        );
                    }
                }

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
                    updateRainbow();
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
                    updateComet();
                }

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
        canvas.print("COMET BEAT");
        canvas.setCursor(10, 48);
        if (estimatedBPM > 0) canvas.printf("BPM %.1f", estimatedBPM);
        else canvas.print("SEARCHING BPM");
        canvas.setCursor(10, 70);
        canvas.print(bpmLocked ? "LOCKED" : "LISTENING");
    }
    else if (mode == BPM_AUTO) {
        canvas.setCursor(10, 27);

        if (estimatedBPM > 0.0f) {
            canvas.printf(
                "BPM: %.1f",
                estimatedBPM
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

const char* SETUP_SSID = "MusicLED-Setup";
const char* SETUP_PASSWORD = "MusicLED123"; // Change before regular use (8+ chars)
const char* HOST_NAME = "musicled";
bool setupAPActive = false;
bool webStarted = false;

void applyModeChange(LedMode newMode) {
    if (newMode == mode) return;
    clearFlashSchedule();
    resetBPM();
    resetEnergy();
    vuPeak = 0.0f;
    lastPeakUpdate = 0;
    silenceStart = 0;
    silenceTriggered = false;
    mode = newMode;
}

String htmlHeader(const char* title) {
    String h = F("<!doctype html><html lang='no'><head><meta charset='utf-8'>"
                 "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                 "<style>body{font:16px system-ui;background:#101827;color:#f2f5fa;"
                 "max-width:520px;margin:30px auto;padding:16px}"
                 "button,input,select{font:inherit;padding:12px;border-radius:9px;margin:6px 0}"
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
<meta name="theme-color" content="#0b1020"><title>MusicLED Pads</title>
<style>
*{box-sizing:border-box}body{margin:0;background:#0b1020;color:#f4f7ff;font-family:-apple-system,BlinkMacSystemFont,system-ui,sans-serif}
.wrap{max-width:540px;margin:auto;min-height:100dvh;padding:22px 16px max(22px,env(safe-area-inset-bottom));display:flex;flex-direction:column}
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
.gear{border:1px solid #38455f;background:#202a40;color:white;width:51px;height:51px;border-radius:15px;display:grid;place-items:center;text-decoration:none}
.gear svg{width:25px;height:25px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round;stroke-linejoin:round}
#message{min-height:20px}
</style></head><body><main class="wrap"><header class="top"><div><h1>MUSICLED</h1><div class="sub">LIVE PERFORMANCE PADS</div></div><div class="dot"></div></header>
<section class="pads" id="pads">
<button class="pad" data-mode="0" onclick="choose(0)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M8 39c8-22 16-22 24 0s16 22 24 0"/><path d="M8 24c8-22 16-22 24 0s16 22 24 0"/></svg></span><span class="name">RAINBOW</span><span class="hint">MUSIC</span></button>
<button class="pad" data-mode="1" onclick="choose(1)"><span class="symbol"><svg viewBox="0 0 64 64"><rect x="10" y="40" width="8" height="14" rx="2"/><rect x="23" y="29" width="8" height="25" rx="2"/><rect x="36" y="18" width="8" height="36" rx="2"/><rect x="49" y="9" width="8" height="45" rx="2"/></svg></span><span class="name">VU METER</span><span class="hint">LEVEL</span></button>
<button class="pad" data-mode="2" onclick="choose(2)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M10 48c15 0 19-28 42-32"/><circle cx="52" cy="16" r="6"/><path d="M7 55l15-17M15 56l14-13"/></svg></span><span class="name">COMET</span><span class="hint">CYAN</span></button>
<button class="pad" data-mode="3" onclick="choose(3)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M9 33h11l7-16 10 31 7-15h11"/><path d="M22 9a25 25 0 1 1-13 43"/></svg></span><span class="name">BPM AUTO</span><span class="hint">BEAT</span></button>
<button class="pad" data-mode="4" onclick="choose(4)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M36 5 14 35h16l-3 24 23-34H34z"/></svg></span><span class="name">ENERGY</span><span class="hint">STROBE</span></button>
<button class="pad" data-mode="5" onclick="choose(5)"><span class="symbol"><svg viewBox="0 0 64 64"><path d="M34 5c4 14-6 18-3 27 6-3 9-9 10-14 19 20 15 37-8 40C10 59 8 42 18 29c1 11 7 13 9 13-5-14 0-27 7-37z"/></svg></span><span class="name">FIRE</span><span class="hint">&amp; SPARKS</span></button>
</section><footer class="footer"><span id="message">Klar</span><a class="gear" href="/settings" aria-label="Innstillinger"><svg viewBox="0 0 24 24"><path d="M10 2h4l.8 2.6 2 .9 2.4-1.2 2.8 2.8-1.2 2.4.9 2L24 12l-2.3 1.5-.9 2 1.2 2.4-2.8 2.8-2.4-1.2-2 .9L14 23h-4l-.8-2.6-2-.9-2.4 1.2L2 17.9l1.2-2.4-.9-2L0 12l2.3-1.5.9-2L2 6.1l2.8-2.8 2.4 1.2 2-.9z" transform="translate(1 1) scale(.91)"/><circle cx="12" cy="12" r="3"/></svg></a></footer></main>
<script>
let active=)HTML");
    h += String((int)mode);
    h += F(R"HTML(;
function highlight(n){active=n;document.querySelectorAll('[data-mode]').forEach(b=>b.classList.toggle('active',Number(b.dataset.mode)===n));}
const vuColors=['CLASSIC','BLUE','RED','PURPLE','GOLD','RAINBOW'];
const cometColors=['CYAN','BLUE','PURPLE','RED','GOLD','RAINBOW'];
let vuIndex=)HTML");
    // Initial VU color is injected by C++ for consistency with the physical button.
    const int vuIndex = vuColorMode == VU_CLASSIC ? 0 :
        vuColorMode == VU_BLUE ? 1 : vuColorMode == VU_RED ? 2 :
        vuColorMode == VU_PURPLE ? 3 : vuColorMode == VU_GOLD ? 4 : 5;
    h += String(vuIndex);
    h += F(R"HTML(;
let cometIndex=)HTML");
    h += String((int)cometColor);
    h += F(R"HTML(;
function updateVUHint(){document.querySelector('[data-mode="1"] .hint').textContent=vuColors[vuIndex];}
async function choose(n){const msg=document.getElementById('message');try{
const vuCycle=(n===1&&active===1);
const cometCycle=(n===2&&active===2);
const url=vuCycle?'/api/color':cometCycle?'/api/comet-color':'/api/mode?value='+n;
const r=await fetch(url,{cache:'no-store'});
if(!r.ok)throw Error();if(vuCycle)vuIndex=(vuIndex+1)%vuColors.length;
if(cometCycle)cometIndex=(cometIndex+1)%cometColors.length;
highlight(n);updateVUHint();document.querySelector('[data-mode="2"] .hint').textContent=cometColors[cometIndex];
msg.textContent=vuCycle?'VU: '+vuColors[vuIndex]:cometCycle?'COMET: '+cometColors[cometIndex]:'Modus valgt';
}catch(e){msg.textContent='Ingen kontakt';}}
highlight(active);updateVUHint();document.querySelector('[data-mode="2"] .hint').textContent=cometColors[cometIndex];
</script></body></html>)HTML");
    webServer.send(200, "text/html; charset=utf-8", h);
}

void showSettingsPage() {
    String h = htmlHeader("MusicLED innstillinger");
    h += F("<p><a href='/'>← Tilbake til pads</a></p>"
           "<label>Lysstyrke: <span id='level'>");
    h += String(currentBrightness);
    h += F("</span></label><input type='range' min='1' max='255' value='");
    h += String(currentBrightness);
    h += F("' oninput='document.getElementById(\"level\").textContent=this.value' "
           "onchange='fetch(\"/api/brightness?value=\"+this.value)'>"
           "<p><a href='/wifi'>Endre Wi-Fi-nettverk</a></p>");
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

    webServer.on("/", HTTP_GET, []() {
        if (setupAPActive) showSetupPage();
        else showControlPage();
    });
    webServer.on("/settings", HTTP_GET, []() {
        if (setupAPActive) showSetupPage(); else showSettingsPage();
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
    webServer.on("/api/mode", HTTP_GET, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        int value = webServer.arg("value").toInt();
        if (!webServer.hasArg("value") || value < 0 || value > 5) {
            webServer.send(400, "text/plain", "Invalid mode"); return;
        }
        requestedMode = value;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/color", HTTP_GET, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        requestedColor = true;
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/api/comet-color", HTTP_GET, []() {
        if (setupAPActive) { webServer.send(403, "text/plain", "Setup mode"); return; }
        requestedCometColor = true;
        webServer.send(200, "text/plain", "OK");
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
        requestedMode = ((int)mode + 1) % 6;
    }

    if (mode == COMET_BEAT && M5.BtnB.wasPressed()) {
        nextCometColor();
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
    if (requestedColor) {
        requestedColor = false;
        if (mode != VU_METER) applyModeChange(VU_METER);
        nextVUColor();
        silenceStart = 0;
        silenceTriggered = false;
    }
    if (requestedCometColor) {
        requestedCometColor = false;
        if (mode != COMET_BEAT) applyModeChange(COMET_BEAT);
        nextCometColor();
    }
    handleWeb();

    analyzeAudio();

    if (
        mode == RAINBOW_MUSIC ||
        mode == VU_METER
    ) {
        updatePeak();
    }

    if (mode == BPM_AUTO || mode == COMET_BEAT) {
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
