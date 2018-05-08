/*
 * Plausi.cpp
 *
 */

#include <string>
#include <deque>
#include <utility>
#include <ctime>
#include <cstdlib>

#include "Plausi.h"

Plausi::Plausi(double maxPower, int window) :
        _maxPower(maxPower), _window(window), _value(-1.), _time(0) {
}

bool Plausi::check(const std::string& value, time_t time) {

  if (value.length() != 7) {
    // exactly 7 digits
    printf("Plausi rejected: exactly 7 digits");
    return false;
  }
  if (value.find_first_of('?') != std::string::npos) {
    // no '?' char
    printf("Plausi rejected: no '?' char");
    return false;
  }

  double dval = atof(value.c_str()) / 10.;
  _queue.push_back(std::make_pair(time, dval));

  if (_queue.size() < _window) {
    printf("Plausi rejected: not enough values: %zu", _queue.size());
    return false;
  }
  if (_queue.size() > _window) {
    _queue.pop_front();
  }

  // iterate through queue and check that all values are ascending
  // and consumption of energy is less than limit
  std::deque<std::pair<time_t, double> >::const_iterator it = _queue.begin();
  time = it->first;
  dval = it->second;
  ++it;
  while (it != _queue.end()) {
    if (it->second < dval) {
      // value must be >= previous value
      printf("Plausi rejected: value must be >= previous value");
      return false;
    }
    double power = (it->second - dval) / (it->first - time) * 3600.;
    if (power > _maxPower) {
      // consumption of energy must not be greater than limit
      printf("Plausi rejected: consumption of energy %.3f must not be greater than limit %.3f", power, _maxPower);
      return false;
    }
    time = it->first;
    dval = it->second;
    ++it;
  }

  // values in queue are ok: use the center value as candidate, but test again with latest checked value
time_t candTime = _queue.at(_window/2).first;
double candValue = _queue.at(_window/2).second;
if (candValue < _value) {
  printf("Plausi rejected: value must be >= previous checked value");
  return false;
}
double power = (candValue - _value) / (candTime - _time) * 3600.;
if (power > _maxPower) {
  printf("Plausi rejected: consumption of energy (checked value) %.3f must not be greater than limit %.3f", power, _maxPower);
  return false;
}

// everything is OK -> use the candidate value
_time = candTime;
_value = candValue;
  printf("Plausi accepted: %.1f of %s", _value, ctime(&_time));
return true;
}

double Plausi::getCheckedValue() {
    return _value;
}

time_t Plausi::getCheckedTime() {
    return _time;
}

std::string Plausi::queueAsString() {
    std::string str;
    char buf[20];
    str += "[";
    std::deque<std::pair<time_t, double> >::const_iterator it = _queue.begin();
    for (; it != _queue.end(); ++it) {
        sprintf(buf, "%.1f", it->second);
        str += buf;
        str += ", ";
    }
    str += "]";
    return str;
}


