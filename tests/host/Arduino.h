#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <string>
using std::min;
using std::max;
using boolean = bool;
using byte = uint8_t;
constexpr int HIGH = 1, LOW = 0, OUTPUT = 1, INPUT_PULLUP = 2;
extern uint32_t hostMs;
extern int hostPins[32];
inline unsigned long millis() { return hostMs; }
inline unsigned long micros() { return hostMs * 1000UL; }
inline void delayMicroseconds(unsigned int) {}
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int pin) { return hostPins[pin]; }
inline void yield() {}
template<class T, class L, class H> T constrain(T v, L lo, H hi) { return min(max(v, T(lo)), T(hi)); }
struct HostSerial {
  std::string input, output;
  void begin(unsigned long) {}
  int available() { return !input.empty(); }
  int read() { char c = input.front(); input.erase(0, 1); return c; }
  void print(const char *s) { output += s; }
  void print(char c) { output += c; }
  void print(uint8_t n) { output += std::to_string(n); }
};
extern HostSerial Serial;
