template <class T> class DataVar {
  T value;
  T lastValue;

public:
  DataVar(T initValue) {
    value = initValue;
    lastValue = initValue;
  };
  T get() { return value; };
  void set(T newValue) {
    lastValue = value;
    value = newValue;
  };
  bool hasChanged() { return lastValue == value; };
};

class DataVarRAvg {
  int value = 0;
  int lastValue = 0;
  long int sum = 0;
  int readingCount = 0;

public:
  int get();
  double getAverage();
  void set(int newValue);
  void clear();
  bool hasChanged();
};
