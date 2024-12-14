#pragma once

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

class StopWatch
{
public:
  StopWatch() : start_(std::chrono::system_clock::now()) {};

  void reset() { start_ = std::chrono::system_clock::now(); }

  std::chrono::milliseconds getDuration()
  {
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
  }

private:
  std::chrono::system_clock::time_point start_;
};
