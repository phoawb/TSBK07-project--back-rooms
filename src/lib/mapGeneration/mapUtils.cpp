#include "mapUtils.h"

#include "algorithm"
#include "random"

int randRange(int min, int max) {
  if (min > max) std::swap(min, max);
  if (min == max) return min;
  static std::random_device rd;                         // Obtain a random number from hardware
  static std::mt19937 gen(rd());                        // Seed the generator
  std::uniform_int_distribution<> distr(min, max - 1);  // Define the range

  return distr(gen);
}