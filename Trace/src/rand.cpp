
#include "rand.hpp"

std::random_device rand::rd;
std::mt19937 rand::gen(rd());
std::uniform_int_distribution<> rand::dist(0, INT_MAX - 1);
