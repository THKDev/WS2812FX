/**
 * Port of WS2821FX to run under ESP8266_RTOS
 * Origin repo: https://github.com/kitesurfer1404/WS2812FX
 * 
 * minimize modifications on the original WS2812FX implementation
 */

#pragma once

#include <cstdint>
#include <cstdlib>

#include <NeoPixelBrightnessBus.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_system.h>
#include <logging.h>

// old style flash access
#define PROGMEM
#define F(x)  (x)
typedef char __FlashStringHelper;

typedef uint16_t neoPixelType;
typedef bool     boolean;
typedef uint8_t  byte;
#define max      std::max
#define min      std::min

template<typename T>
T random() {
    return esp_random();
}

template<typename T>
T random(const T& maxVal) {
    return (random<T>() % maxVal) + 1;
}

template<typename T>
T random(const T& minVal, const T& maxVal) {
    return minVal + random<T>((maxVal - minVal));
}


// http://arduino.cc/en/Reference/constrain
template<class T1, class T2, class T3>
const T1 constrain(const T1& x, const T2& a, const T3& b) {
    if(x < a)
        return a;
    else if(b < x)
        return b;
    else
        return x;
}

// typedef NeoPixelBus configuration
typedef NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> NPB_IMPL;

// Adafruit_NeoPixel compatibility layer
// https://github.com/adafruit/Adafruit_NeoPixel
class Adafruit_NeoPixel : public NPB_IMPL {
    public:
        Adafruit_NeoPixel(const uint16_t numberOfLeds, const uint8_t pinNumber = 2, const neoPixelType type = 0)
        : NPB_IMPL(numberOfLeds, pinNumber),
          numLEDs(numberOfLeds) {
        }
        
        void begin() {
            NPB_IMPL::Begin();
            NPB_IMPL::Show();
            numBytes = NPB_IMPL::PixelsSize();
        }
        
        void show() {
            NPB_IMPL::Show();
        }
        
        void delay(uint32_t ms) {
            vTaskDelay(ms / portTICK_PERIOD_MS);
        }
        
        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
            NPB_IMPL::SetPixelColor(n, RgbColor(r, g, b));
        }

        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
            auto color = RgbColor(r, g, b);
            NPB_IMPL::SetPixelColor(n, color);
        }

        void setPixelColor(uint16_t n, uint32_t c) {
            uint8_t r = (c >> 16) & 0xFF;
            uint8_t g = (c >>  8) & 0xFF;
            uint8_t b =  c        & 0xFF;
            auto color = RgbColor(r, g, b);
            NPB_IMPL::SetPixelColor(n, color);
        }
        
        uint32_t getPixelColor(uint16_t n) const {
            auto color = NPB_IMPL::GetPixelColor(n);
            uint32_t c = 0;
            c |= color.R << 16;
            c |= color.G << 8;
            c |= color.B;
            return c;
        }
        
        void clear() {
            NPB_IMPL::ClearTo(RgbColor(0, 0, 0));
        }
        
        uint16_t numPixels(void) const {
            return numLEDs;
        }
        
        void setBrightness(uint8_t b) {
            brightness = b;
            NPB_IMPL::SetBrightness(b);
        }
        
        uint8_t getBrightness() {
            brightness = NPB_IMPL::GetBrightness();
            return brightness;
        }
        
        uint8_t* getPixels() {
            return NPB_IMPL::Pixels();
        }
        
        void updateLength(uint16_t n) {
        }
        
        template<typename T>        
        T sine8(const T& x) const {
            return _sineTable[x]; // 0-255 in, 0-255 out
        }
        
        template<typename T>
        T gamma8(const T& x) const {
            return _gammaTable[x]; // 0-255 in, 0-255 out        
        }
        
    protected:
        const uint16_t numLEDs;
        uint16_t numBytes;
        uint8_t brightness;
        static constexpr uint8_t  rOffset = NeoGrbFeature::PixelSize;
        static constexpr uint8_t  wOffset = NeoGrbFeature::PixelSize;
        
    private:
        static const uint8_t _sineTable[];
        static const uint8_t _gammaTable[];
};
