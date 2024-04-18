#include "mapUtils.h"

#include "algorithm"
#include "random"

int randRange(int min, int max, bool isInclusive) {
  if (min > max) std::swap(min, max);
  if (min == max) return min;
  static std::random_device rd;   // Obtain a random number from hardware
  static std::mt19937 gen(rd());  // Seed the generator
  int distrMax = isInclusive ? max : max - 1;
  std::uniform_int_distribution<> distr(min, distrMax);  // Define the range

  return distr(gen);
}