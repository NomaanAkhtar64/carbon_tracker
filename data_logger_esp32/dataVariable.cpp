#include "dataVariable.h"
#include <cmath>

double decimalRound(double number) { return round(number * 100) / 100; }

int DataVarRAvg::get() { return value; }
double DataVarRAvg::getAverage() {
  return decimalRound(sum / (double)readingCount);
}
void DataVarRAvg::set(int newValue) {
  lastValue = value;
  value = newValue;
  sum += newValue;
  readingCount += 1;
};
void DataVarRAvg::clear() {
  readingCount = 0;
  value = 0;
  sum = 0;
  lastValue = 0;
};
bool DataVarRAvg::hasChanged() { return lastValue == value; };
