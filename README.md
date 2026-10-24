# MusicLED 🎵

**Musikkstyrt WS2812 LED-stripe med M5StickS3 og mobilvennlig webkontroll.**

MusicLED bruker den innebygde mikrofonen i M5StickS3 til å styre en adresserbar LED-stripe. Velg effekter direkte fra iPhone eller en annen enhet på samme Wi-Fi-nettverk.

![MusicLED drum-pad-kontroll](docs/images/drum-pads.png)

> **Versjon:** v1.2 (prosjektets GitHub-versjon). Bildene er illustrasjoner av grensesnittet og LED-effektene, ikke fotografier av ferdig maskinvare.

## Funksjoner

- **Seks store drum-pads**, arrangert 2 × 3 for mobilskjerm.
- **Live modusbytte** uten å laste inn siden på nytt.
- **Gjenta trykk på VU Meter** for å bla mellom fargevarianter.
- **Gjenta trykk på Comet** for å bla mellom fargevarianter.
- **Egen innstillingsside** via tannhjul, blant annet lysstyrke og Wi-Fi.
- **Wi-Fi-oppsett via tilgangspunkt**, med lagring av nettverksinformasjon.
- **Lokal webadresse:** `http://musicled.local` (dersom mDNS fungerer på nettverket).
- Ingen kontinuerlig lydnivåmåler på websiden.

## LED-effekter

| Pad | Effekt | Beskrivelse |
|---|---|---|
| 🌈 Rainbow Music | Regnbue | Fargerik effekt som reagerer på musikken |
| 📊 VU Meter | Nivåmåler | Klassisk grønn–gul–rød skala, samt blå, rød, lilla, gull og regnbue |
| ☄️ Comet Beat | Kometer | Nye kometer startes på BPM-slag; omtrent 30 LED-er lang hale og overlappende kometer |
| 💓 BPM Auto | Rytme | Musikkstyrt rytmeeffekt basert på detektert tempo |
| ⚡ Energy Strobe | Energi | Dynamiske lysblink basert på musikken |
| 🔥 Fire & Sparks | Flamme | Simulert ild med musikkstyrte gnister |

### Comet Beat

![Illustrasjon av Comet Beat](docs/images/comet-beat.png)

En ny komet starter ved begynnelsen av stripen på hvert registrerte eller beregnede BPM-slag. Kometene fortsetter uavhengig av hverandre, slik at flere kan være synlige samtidig. Hastigheten tilpasses tempoet; målet er at en komet er omtrent halvveis når neste starter.

**Farger:** cyan, blå, lilla, rød, gull og regnbue. Trykk på den aktive Comet-paden igjen for neste farge.

### Fire & Sparks

![Illustrasjon av Fire & Sparks](docs/images/fire-sparks.png)

En flammeeffekt i varme farger går kontinuerlig. Musikk kan utløse korte, lyse gnister over flammen.

## Maskinvare

- **M5StickS3** (ESP32-S3) med innebygd mikrofon og skjerm
- **WS2812-kompatibel LED-stripe**, konfigurert for **68 LED-er**
- **Ekstern, regulert 5 V strømforsyning** dimensjonert for LED-stripen
- Datatilkobling fra **GPIO1** på M5StickS3

### Kobling

```text
M5StickS3 GPIO1  ─────────── DIN på WS2812
M5StickS3 GND    ─────┬───── GND på WS2812
                      │
5 V strømforsyning − ─┘
5 V strømforsyning + ─────── +5V på WS2812
```

**Viktig:** Felles jord er nødvendig. Ikke forsyn hele LED-stripen fra M5StickS3. Bruk riktig dataretning (`DIN`, ikke `DOUT`). En seriemotstand på datalinjen og kondensator ved LED-stripeinngangen anbefales. Vurder nivåskifter dersom 3,3 V datasignal ikke er stabilt med din 5 V stripe.

## Kom i gang

1. Installer **Visual Studio Code** og **PlatformIO**.
2. Åpne prosjektmappen i PlatformIO.
3. Kontroller `platformio.ini` og installer prosjektets biblioteker: **M5Unified**, **Adafruit NeoPixel** og **arduinoFFT**.
4. Kontroller innstillingene øverst i `src/main.cpp`:

   ```cpp
   #define LED_PIN 1
   #define NUM_LEDS 68
   #define BRIGHTNESS 60
   ```

5. Bygg prosjektet med **Build**, og last det opp med **Upload**.
6. Ved første oppstart, koble telefonen til Wi-Fi-nettverket **`MusicLED-Setup`** med passordet **`MusicLED123`**.
7. Åpne **`http://192.168.4.1`**, og skriv inn hjemmenettverkets Wi-Fi-informasjon.
8. Når enheten har koblet seg til hjemmenettverket, åpne **`http://musicled.local`** på telefonen. Hvis adressen ikke fungerer, bruk IP-adressen som vises i seriellmonitoren.

> **Sikkerhet:** Endre standardpassordet for oppsett-AP i kildekoden før vanlig bruk. Webkontrollen er laget for et betrodd lokalnettverk, ikke for eksponering mot internett.

## Betjening

På hovedsiden velger du effekt med ett trykk. Den aktive knappen markeres tydelig. Når **VU Meter** eller **Comet** allerede er valgt, blar et nytt trykk til neste fargevariant.

Trykk på **tannhjulet nederst til høyre** for innstillinger, inkludert lysstyrke og Wi-Fi.

## Prosjektstatus

Prosjektet er utviklet iterativt. Kometfunksjonen er nylig justert til å starte **en ny komet for hvert BPM-slag**, med **30 LED-er lang hale** og mulighet for flere kometer samtidig. Ytterligere finjustering av BPM-følsomhet og hastighet kan være aktuelt.

## Teknologi

**ESP32-S3 · Arduino · PlatformIO · M5Unified · Adafruit NeoPixel · arduinoFFT · Wi-Fi · WebServer · mDNS**

---

Laget som et hobbyprosjekt for å utforske musikkstyrt LED-belysning.
