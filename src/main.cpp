
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
    FFT_3BAND,
    BPM_AUTO,
    ENERGY_STROBE
};

volatile LedMode mode = RAINBOW_MUSIC;
volatile int requestedMode = -1;
volatile uint8_t requestedBrightness = BRIGHTNESS;
uint8_t currentBrightness = BRIGHTNESS;

const char* modeNames[] = {
    "RAINBOW MUSIC",
    "VU METER",
    "FFT 3-BAND",
    "BPM AUTO",
    "ENERGY STROBE"
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
    VU_WHITE,
    VU_RAINBOW,
    VU_CLASSIC
};

VUColorMode vuColorMode = VU_AUTO;

const int VU_COLOR_MODE_COUNT = 7;

int autoColorIndex = 0;

const char* vuColorNames[] = {
    "AUTO",
    "BLUE",
    "RED",
    "PURPLE",
    "WHITE",
    "RAINBOW",
    "CLASSIC VU"
};

const char* autoColorNames[] = {
    "BLUE",
    "RED",
    "PURPLE",
    "WHITE",
    "RAINBOW",
    "CLASSIC VU"
};

const uint32_t AUTO_SILENCE_MS = 1000;
const float AUTO_SILENCE_THRESHOLD = 300.0f;

uint32_t silenceStart = 0;
bool silenceTriggered = false;

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
            return strip.Color(255, 255, 255);

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
        mode != BPM_AUTO ||
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
            mode == BPM_AUTO &&
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
        mode == BPM_AUTO &&
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
                else {
                    updateFFT();
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
    else if (mode == FFT_3BAND) {
        const char* labels[3] = {
            "BASS",
            "MID",
            "TREBLE"
        };

        uint16_t colors[3] = {
            TFT_BLUE,
            TFT_GREEN,
            TFT_ORANGE
        };

        for (int b = 0; b < 3; b++) {
            int y =
                27 + b * 29;

            canvas.setCursor(10, y);
            canvas.print(labels[b]);

            canvas.setCursor(90, y);
            canvas.printf(
                "%.2f",
                levels[b]
            );

            drawBar(
                10,
                y + 12,
                w,
                10,
                levels[b],
                colors[b]
            );
        }
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
    String h = htmlHeader("MusicLED Control");
    h += F("<p>Wi-Fi: "); h += WiFi.SSID(); h += F("<br>IP: ");
    h += WiFi.localIP().toString(); h += F("</p>");
    h += F("<label>LED-modus</label><select id='mode' onchange='setMode()'>");
    for (int i = 0; i < 5; i++) {
        h += F("<option value='"); h += i; h += F("'");
        if ((int)mode == i) h += F(" selected");
        h += F(">"); h += modeNames[i]; h += F("</option>");
    }
    h += F("</select><label>Lysstyrke: <span id='level'>");
    h += String(currentBrightness);
    h += F("</span></label><input type='range' min='1' max='255' value='");
    h += String(currentBrightness);
    h += F("' oninput='document.getElementById(\"level\").textContent=this.value' "
           "onchange='fetch(\"/api/brightness?value=\"+this.value)'>"
           "<p><a href='/wifi'>Endre Wi-Fi</a></p>"
           "<script>function setMode(){fetch('/api/mode?value='+document.getElementById('mode').value)}</script>");
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
        Serial.print("MusicLED IP: ");
        Serial.println(WiFi.localIP());

        if (MDNS.begin("musicled")) {
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
        if (!webServer.hasArg("value") || value < 0 || value > 4) {
            webServer.send(400, "text/plain", "Invalid mode"); return;
        }
        requestedMode = value;
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
        requestedMode = ((int)mode + 1) % 5;
    }

    if (
        mode == VU_METER &&
        M5.BtnB.wasPressed()
    ) {
        vuColorMode = (VUColorMode)(
            ((int)vuColorMode + 1) %
            VU_COLOR_MODE_COUNT
        );

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
    handleWeb();

    analyzeAudio();

    if (
        mode == RAINBOW_MUSIC ||
        mode == VU_METER
    ) {
        updatePeak();
    }

    if (mode == BPM_AUTO) {
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
