#pragma once
#include <string>
#include <map>
struct Preferences {
  std::map<std::string, long> values;
  void begin(const char *, bool) {}
  long getLong(const char *key, long fallback) { return values.count(key) ? values[key] : fallback; }
  bool getBool(const char *key, bool fallback) { return getLong(key, fallback); }
  void putLong(const char *key, long value) { values[key] = value; }
  void putBool(const char *key, bool value) { values[key] = value; }
};
