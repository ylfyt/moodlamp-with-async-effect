#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>

#define NUM_LEDS 63
#define PIN 5
#define MAXBRIGHTNESS 127
#define MAXEFFECT 17
#define BUTTON 2

struct State
{
    byte randomMode;
    byte selectedEffect;
    byte randomDuration;
    byte beforeRandomEffect;
    byte brightness;
};

CRGB leds[NUM_LEDS];
CRGB le[10];
byte prevEffect = 0;
byte selectedEffect = 0;

byte randomEffect = 12;
byte beforeRandom = 0;
byte randomMode = 0;
byte duration = 15;

byte brightness = 50;

bool prevState = false;
bool state = false;

unsigned long curr = 0;
unsigned long prev = 0;
bool delayTime = false;

unsigned long prevTC = 0;
int jTC = 0;
int qTC = 0;

unsigned long prevTCR = 0;
int jTCR = 0;
int qTCR = 0;

unsigned long prevRC = 0;
int jRC = 0;

unsigned long prevRL = 0;
int jRL = 0;
int posRL = 1;
int colorRL = 0;

unsigned long prevRGBL = 0;
int jRGBL = 0;
int kRGBL = 0;
bool risingRGBL = true;

int kFIO = 0;
int colorFIO = 0;
bool risingFIO = true;

unsigned long prevCW = 0;
int jCW = 0;
int colorCW = 0;

unsigned long prevMR = 0;
int iMR = 0;
bool firstMR = true;

unsigned long prevS = 0;
int jS = 0;
bool flashDelay = false;

unsigned long prevTR = 0;
int iTR = 0;
bool firstTR = true;

unsigned long prevSS = 0;
int PixelSS = 0;
int speedDelaySS = 100;
bool EndSparkle = false;
bool sparkleDelay = false;
bool firstSS = true;

unsigned long prevF = 0;

byte tag, data;

//unsigned long prev2 = 0;

void loadState();
void showStrip();
void changeEffect();
void theaterChaseRainbow2(int speedDelay);
void theaterChase2(int speedDelay);
byte *Wheel(byte WheelPos);
void rainbowCycle2(int speedDelay);
void colorWipe2(int speedDelay);
void RunningLights2(int WaveDelay);
void SnowSparkle2(byte red, byte green, byte blue, int SparkleDelay);
void Sparkle(byte red, byte green, byte blue, int SpeedDelay);
void TwinkleRandom2(int Count, int speedDelay, boolean OnlyOne);
void Strobe2(int StrobeCount, int speedDelay, int EndPause);
void FadeInOut2();
void RGBLoop2(int speedDelay);
void SameColor(byte red, byte green, byte blue);
void saveState();
void resetState();
void changeRandomEffect();
void changeEffect();
void setAll(byte red, byte green, byte blue);
void setPixel(int Pixel, byte red, byte green, byte blue);
void fadeToBlack(int ledNo, byte fadeValue);
void meteorRain2(int speedDelay);
void setPixelHeatColor(int Pixel, byte temperature, int fireColor);
void Fire(int Cooling, int Sparking, int speedDelay, int fireColor);

void setup()
{
    loadState();
    Serial.begin(9600);
    FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    pinMode(BUTTON, INPUT);
    //attachInterrupt (digitalPinToInterrupt (BUTTON), changeEffect, CHANGE); // pressed
}

// *** REPLACE FROM HERE ***
void loop()
{
    if (Serial.available() >= 2)
    {
        tag = Serial.read();
        delay(2);
        data = Serial.read();
        delay(2);

        Serial.print(tag);
        Serial.print('|');
        Serial.println(data);

        if (tag == 0)
        {
            randomMode = data;
            if (randomMode)
            {
                prev = millis();
                beforeRandom = selectedEffect;
            }
            else
            {
                selectedEffect = beforeRandom;
            }
        }
        else if (tag == 1)
            brightness = data;
        else if (tag == 2)
            selectedEffect = data;
        else if (tag == 3)
            duration = data;
        else if (tag == 7 && data == 0)
            saveState();
    }

    changeEffect();

    changeRandomEffect();

    if (prevEffect != selectedEffect)
    {
        resetState();
    }
    prevEffect = selectedEffect;

    FastLED.setBrightness(((float)brightness / 100) * MAXBRIGHTNESS);

    switch (selectedEffect)
    {

    case 0:
    {
        // RGBLoop - no parameters
        RGBLoop2(3);
        break;
    }

    case 1:
    {
        // FadeInOut - Color (red, green. blue)
        FadeInOut2();
        break;
    }

    case 2:
    {
        // Strobe - Color (red, green, blue), number of flashes, flash speed, end pause
        Strobe2(10, 50, 1000);
        break;
    }

    case 3:
    {
        // TwinkleRandom - twinkle count, speed delay, only one (true/false)
        TwinkleRandom2(20, 100, false);
        break;
    }

    case 4:
    {
        // Sparkle - Color (red, green, blue), speed delay
        Sparkle(random(0, 255), random(0, 255), random(0, 255), 0);
        break;
    }

    case 5:
    {
        // SnowSparkle - Color (red, green, blue), sparkle delay, speed delay
        SnowSparkle2(0x10, 0x10, 0x10, 20);
        break;
    }

    case 6:
    {
        // Running Lights - Color (red, green, blue), wave dealy
        RunningLights2(50);
        break;
    }

    case 7:
    {
        // colorWipe - Color (red, green, blue), speed delay
        colorWipe2(50);
        break;
    }

    case 8:
    {
        // rainbowCycle - speed delay
        rainbowCycle2(20);
        break;
    }

    case 9:
    {
        // Color Red
        SameColor(0xff, 0x00, 0x00);
        break;
    }

    case 10:
    {
        //FastLED.setBrightness( ((float)brightness/100) * MAXBRIGHTNESS * 0.25f );
        // Color warm light
        SameColor(0xe4, 0x70, 0x25);
        break;
    }

    case 11:
    {
        // theatherChase - Color (red, green, blue), speed delay
        theaterChase2(50);
        break;
    }

    case 12:
    {
        // theaterChaseRainbow - Speed delay
        theaterChaseRainbow2(50);
        break;
    }

    case 13:
    {
        // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay
        meteorRain2(30);
        break;
    }

    case 14:
    {
        // Fire - Cooling rate, Sparking rate, speed delay
        Fire(55, 120, 15, 0); // RED
        break;
    }

    case 15:
    {
        // Fire - Cooling rate, Sparking rate, speed delay
        Fire(55, 120, 15, 1); // GREEN
        break;
    }

    case 16:
    {
        // Fire - Cooling rate, Sparking rate, speed delay
        Fire(55, 120, 15, 2); // Blue
        break;
    }
    }
}

void changeEffect()
{
    state = digitalRead(BUTTON);
    if (state == HIGH && state != prevState)
    {
        if (randomMode)
        {
            randomMode = 0;
            selectedEffect = beforeRandom;
        }
        else
            selectedEffect++;

        if (selectedEffect >= MAXEFFECT)
        {
            selectedEffect = 0;
        }
    }
    prevState = state;
}

void changeRandomEffect()
{
    if (randomMode)
    {
        curr = millis();
        if (curr - prev >= duration * 1000)
        {
            prev = curr;
            randomSeed(analogRead(0));
            randomEffect = random(100) % MAXEFFECT;
        }

        selectedEffect = randomEffect;
    }
}

void resetState()
{
    delayTime = false;

    prevTC = millis();
    jTC = 0;
    qTC = 0;

    prevTCR = millis();
    jTCR = 0;
    qTCR = 0;

    prevRC = millis();
    jRC = 0;

    prevRL = millis();
    jRL = 0;
    posRL = 1;
    colorRL = 0;

    prevRGBL = millis();
    jRGBL = 0;
    kRGBL = 0;
    risingRGBL = true;

    kFIO = 0;
    colorFIO = 0;
    risingFIO = true;

    prevCW = millis();
    jCW = 0;
    colorCW = 0;

    prevMR = millis();
    iMR = 0;
    firstMR = true;

    prevS = millis();
    jS = 0;
    flashDelay = false;

    prevTR = millis();
    iTR = 0;
    firstTR = true;

    prevSS = millis();
    PixelSS = 0;
    speedDelaySS = 100;
    EndSparkle = false;
    sparkleDelay = false;
    firstSS = true;

    prevF = millis();
}

void saveState()
{
    State S;

    S.randomMode = randomMode;
    if (randomMode)
    {
        S.selectedEffect = randomEffect;
        S.beforeRandomEffect = beforeRandom;
    }
    else
    {
        S.selectedEffect = selectedEffect;
        S.beforeRandomEffect = 0;
    }

    S.randomDuration = duration;
    S.brightness = brightness;

    EEPROM.put(0, S);
}

void loadState()
{
    State S;
    EEPROM.get(0, S);

    randomMode = S.randomMode;
    if (S.randomMode)
    {
        randomEffect = S.selectedEffect;
        beforeRandom = S.beforeRandomEffect;
    }
    else
    {
        selectedEffect = S.selectedEffect;
        beforeRandom = 0;
    }

    duration = S.randomDuration;
    brightness = S.brightness;
}

// *************************
// ** LEDEffect Functions **
// *************************

void SameColor(byte red, byte green, byte blue)
{
    setAll(red, green, blue);
    showStrip();
    delay(3);
}

void RGBLoop2(int speedDelay)
{
    if (!delayTime)
        switch (jRGBL)
        {
        case 0:
            setAll(kRGBL, 0, 0);
            break;
        case 1:
            setAll(0, kRGBL, 0);
            break;
        case 2:
            setAll(0, 0, kRGBL);
            break;
        }
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevRGBL >= speedDelay)
    {
        delayTime = false;
        if (risingRGBL)
            kRGBL++;
        else
            kRGBL--;
    }

    if (kRGBL >= 256)
    {
        risingRGBL = false;
        kRGBL = 255;
        delayTime = false;
    }

    if (kRGBL <= -1)
    {
        risingRGBL = true;
        kRGBL = 0;
        jRGBL++;
        delayTime = false;
    }

    if (jRGBL >= 3)
    {
        risingRGBL = true;
        kRGBL = 0;
        jRGBL = 0;
        delayTime = false;
    }
}

void FadeInOut2()
{
    byte red, green, blue;
    float r, g, b;

    if (colorFIO == 0)
    {
        red = 0x00;
        green = 0x00;
        blue = 0xff;
    }
    else if (colorFIO == 1)
    {
        red = 0xff;
        green = 0xff;
        blue = 0xff;
    }
    else
    {
        red = 0xff;
        green = 0x00;
        blue = 0x00;
    }

    r = (kFIO / 256.0) * red;
    g = (kFIO / 256.0) * green;
    b = (kFIO / 256.0) * blue;
    setAll(r, g, b);
    showStrip();

    if (risingFIO)
        kFIO++;
    else
        kFIO--;

    if (kFIO >= 256)
    {
        risingFIO = false;
        kFIO = 255;
    }

    if (kFIO <= -1)
    {
        risingFIO = true;
        kFIO = 0;
        colorFIO++;
    }

    if (colorFIO >= 3)
    {
        risingFIO = true;
        colorFIO = 0;
        kFIO = 0;
    }
}

void Strobe2(int StrobeCount, int speedDelay, int EndPause)
{
    byte red = 0xff;
    byte green = 0xff;
    byte blue = 0xff;

    if (!delayTime && !flashDelay && jS < StrobeCount)
        setAll(red, green, blue);

    if (!delayTime && flashDelay && jS < StrobeCount)
        setAll(0, 0, 0);

    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevS >= speedDelay && jS < StrobeCount)
    {
        prevS = curr;
        delayTime = false;
        if (flashDelay)
            jS++;
        flashDelay = !flashDelay;
    }

    if (jS >= StrobeCount)
    {
        curr = millis();
        if (curr - prevS >= EndPause)
        {
            prevS = curr;
            delayTime = false;
            flashDelay = false;
            jS = 0;
        }
    }
}

void TwinkleRandom2(int Count, int speedDelay, boolean OnlyOne)
{
    if (firstTR)
    {
        firstTR = false;
        setAll(0, 0, 0);
    }

    if (!delayTime && iTR < Count)
        setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevTR >= speedDelay && iTR < Count)
    {
        prevTR = curr;
        delayTime = false;
        if (OnlyOne)
            setAll(0, 0, 0);
        iTR++;
    }

    if (iTR >= Count)
    {
        curr = millis();
        if (curr - prevTR >= speedDelay)
        {
            prevTR = curr;
            delayTime = false;
            iTR = 0;
            firstTR = true;
        }
    }
}

// 08
void Sparkle(byte red, byte green, byte blue, int SpeedDelay)
{
    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delay(SpeedDelay);
    setPixel(Pixel, 0, 0, 0);
}

void SnowSparkle2(byte red, byte green, byte blue, int SparkleDelay)
{
    if (firstSS)
    {
        speedDelaySS = random(100, 1000);
        PixelSS = random(NUM_LEDS);
        firstSS = false;
        setAll(red, green, blue);
    }

    if (!delayTime && !sparkleDelay && !EndSparkle)
        setPixel(PixelSS, 0xff, 0xff, 0xff);

    if (!delayTime && sparkleDelay && !EndSparkle)
    {
        setPixel(PixelSS, red, green, blue);
        EndSparkle = true;
    }

    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevSS >= SparkleDelay && !EndSparkle)
    {
        prevSS = curr;
        delayTime = false;
        sparkleDelay = !sparkleDelay;
    }

    if (EndSparkle)
    {
        curr = millis();
        if (curr - prevSS >= speedDelaySS)
        {
            prevSS = curr;
            EndSparkle = false;
            sparkleDelay = false;
            delayTime = false;
            firstSS = true;
        }
    }
}

void RunningLights2(int WaveDelay)
{
    byte red, green, blue;

    if (colorRL == 0)
    {
        red = 0xff;
        green = 0x00;
        blue = 0x00;
    }
    else if (colorRL == 1)
    {
        red = 0xff;
        green = 0xff;
        blue = 0xff;
    }
    else
    {
        red = 0x00;
        green = 0x00;
        blue = 0xff;
    }

    if (!delayTime)
        for (int i = 0; i < NUM_LEDS; i++)
        {
            // sine wave, 3 offset waves make a rainbow!
            //float level = sin(i+Position) * 127 + 128;
            //setPixel(i,level,0,0);
            //float level = sin(i+Position) * 127 + 128;
            setPixel(i, ((sin(i + posRL) * 127 + 128) / 255) * red,
                     ((sin(i + posRL) * 127 + 128) / 255) * green,
                     ((sin(i + posRL) * 127 + 128) / 255) * blue);
        }
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevRL >= WaveDelay)
    {
        prevRL = curr;
        delayTime = false;
        jRL++;
        posRL++;
    }

    if (jRL >= NUM_LEDS * 2)
    {
        jRL = 0;
        posRL = 1;
        delayTime = false;
        colorRL++;
    }

    if (colorRL >= 3)
    {
        colorRL = 0;
        jRL = 0;
        posRL = 1;
        delayTime = false;
    }
}

void colorWipe2(int speedDelay)
{
    byte red, green, blue;

    if (colorCW == 0)
    {
        red = 0x00;
        green = 0xff;
        blue = 0x00;
    }
    else if (colorCW == 1)
    {
        red = 0x00;
        green = 0x00;
        blue = 0xff;
    }
    else
    {
        red = 0xff;
        green = 0x00;
        blue = 0x00;
    }

    if (!delayTime)
        setPixel(jCW, red, green, blue);
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevCW >= speedDelay)
    {
        prevCW = curr;
        jCW++;
        delayTime = false;
    }

    if (jCW >= NUM_LEDS)
    {
        jCW = 0;
        delayTime = false;
        colorCW++;
    }

    if (colorCW >= 3)
    {
        colorCW = 0;
        jCW = 0;
        delayTime = false;
    }
}

void rainbowCycle2(int speedDelay)
{
    byte *c;

    if (!delayTime)
        for (int i = 0; i < NUM_LEDS; i++)
        {
            c = Wheel(((i * 256 / NUM_LEDS) + jRC) & 255);
            setPixel(i, *c, *(c + 1), *(c + 2));
        }
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevRC >= speedDelay)
    {
        prevRC = curr;
        delayTime = false;
        jRC++;
    }

    if (jRC >= 256 * 5)
    {
        jRC = 0;
        delayTime = false;
    }
}

// used by rainbowCycle and theaterChaseRainbow
byte *Wheel(byte WheelPos)
{
    static byte c[3];

    if (WheelPos < 85)
    {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    }
    else
    {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }

    return c;
}

void theaterChase2(int speedDelay)
{
    byte red = 0xff;
    byte green = 0x00;
    byte blue = 0x00;

    if (!delayTime)
        for (int i = 0; i < NUM_LEDS; i = i + 3)
        {
            setPixel(i + qTC, red, green, blue); //turn every third pixel on
        }
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevTC >= speedDelay)
    {
        prevTC = curr;
        for (int i = 0; i < NUM_LEDS; i = i + 3)
            setPixel(i + qTC, 0, 0, 0); //turn every third pixel off
        delayTime = false;
        qTC++;
    }

    if (qTC >= 3)
    {
        qTC = 0;
        jTC++;
        delayTime = false;
    }

    if (jTC >= 10)
    {
        jTC = 0;
        qTC = 0;
        delayTime = false;
    }
}

// 16
void theaterChaseRainbow2(int speedDelay)
{
    byte *c;

    if (!delayTime)
        for (int i = 0; i < NUM_LEDS; i = i + 3)
        {
            c = Wheel((i + jTCR) % 255);
            setPixel(i + qTCR, *c, *(c + 1), *(c + 2)); //turn every third pixel on
        }

    delayTime = true;
    showStrip();

    curr = millis();
    if (curr - prevTCR >= speedDelay)
    {
        for (int k = 0; k < NUM_LEDS; k = k + 3)
        {
            setPixel(k + qTCR, 0, 0, 0);
        }
        delayTime = false;
        prevTCR = curr;
        qTCR++;
    }

    if (qTCR >= 3)
    {
        delayTime = false;
        qTCR = 0;
        jTCR++;
    }

    if (jTCR >= 256)
    {
        delayTime = false;
        jTCR = 0;
        qTCR = 0;
    }
}

// 20 21 22
void Fire(int Cooling, int Sparking, int speedDelay, int fireColor)
{
    if (!delayTime)
    {
        static byte heat[NUM_LEDS];
        int cooldown;

        // Step 1.  Cool down every cell a little
        for (int i = 0; i < NUM_LEDS; i++)
        {
            cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

            if (cooldown > heat[i])
            {
                heat[i] = 0;
            }
            else
            {
                heat[i] = heat[i] - cooldown;
            }
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = NUM_LEDS - 1; k >= 2; k--)
        {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' near the bottom
        if (random(255) < Sparking)
        {
            int y = random(7);
            heat[y] = heat[y] + random(160, 255);
            //heat[y] = random(160,255);
        }

        // Step 4.  Convert heat to LED colors
        for (int j = 0; j < NUM_LEDS; j++)
        {
            setPixelHeatColor(j, heat[j], fireColor);
        }
    }
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevF >= speedDelay)
    {
        prevF = curr;
        delayTime = false;
    }
}

void setPixelHeatColor(int Pixel, byte temperature, int fireColor)
{
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;              // scale up to 0..252

    // figure out which third of the spectrum we're in:
    // Fire RED
    if (fireColor == 0)
    {
        if (t192 > 0x80)
        { // hottest
            setPixel(Pixel, 255, 255, heatramp);
        }
        else if (t192 > 0x40)
        { // middle
            setPixel(Pixel, 255, heatramp, 0);
        }
        else
        { // coolest
            setPixel(Pixel, heatramp, 0, 0);
        }
    }
    // FIRE GREEN
    else if (fireColor == 1)
    {
        if (t192 > 0x80)
        { // hottest
            setPixel(Pixel, 255, 255, heatramp);
        }
        else if (t192 > 0x40)
        { // middle
            setPixel(Pixel, heatramp, 255, 0);
        }
        else
        { // coolest
            setPixel(Pixel, 0, heatramp, 0);
        }
    }
    // FIRE BLUE
    else
    {
        if (t192 > 0x80)
        { // hottest
            setPixel(Pixel, 255, heatramp, 255);
        }
        else if (t192 > 0x40)
        { // middle
            setPixel(Pixel, heatramp, 0, 255);
        }
        else
        { // coolest
            setPixel(Pixel, 0, 0, heatramp);
        }
    }
}

void meteorRain2(int speedDelay)
{
    byte red = 0xff;
    byte green = 0xff;
    byte blue = 0xff;

    byte meteorSize = 10;
    byte meteorTrailDecay = 64;
    bool meteorRandomDecay = true;

    if (firstMR)
    {
        firstMR = false;
        setAll(0, 0, 0);
    }

    if (!delayTime)
    {
        // fade brightness all LEDs one step
        for (int j = 0; j < NUM_LEDS; j++)
        {
            if ((!meteorRandomDecay) || (random(10) > 5))
            {
                fadeToBlack(j, meteorTrailDecay);
            }
        }

        // draw meteor
        for (int j = 0; j < meteorSize; j++)
        {
            if ((iMR - j < NUM_LEDS) && (iMR - j >= 0))
            {
                setPixel(iMR - j, red, green, blue);
            }
        }
    }
    showStrip();
    delayTime = true;

    curr = millis();
    if (curr - prevMR >= speedDelay)
    {
        prevMR = curr;
        iMR++;
        delayTime = false;
    }

    if (iMR >= NUM_LEDS * 2)
    {
        iMR = 0;
        delayTime = false;
        firstMR = true;
    }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;

    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
    g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
    b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

    strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}

// *** REPLACE TO HERE ***

// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
void showStrip()
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    FastLED.show();
#endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[Pixel].r = red;
    leds[Pixel].g = green;
    leds[Pixel].b = blue;
#endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        setPixel(i, red, green, blue);
    }
    showStrip();
}

// =======================================================================================================
// =======================================================================================================
// =======================================================================================================
// =======================================================================================================
