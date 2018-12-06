#include <Adafruit_NeoPixel.h>

// issue is solved with C++17
// https://stackoverflow.com/a/46719572
constexpr uint8_t Adafruit_NeoPixel::_sineTable[];
constexpr uint8_t Adafruit_NeoPixel::_gammaTable[];
