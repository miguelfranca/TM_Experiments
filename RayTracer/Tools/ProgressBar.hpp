// By Tiago Franca

#pragma once

#include <string>

class ProgressBar
{
  public:
    ProgressBar(unsigned p_max, const std::string p_name = "",
                unsigned p_PBwidth = 60);

    ProgressBar &operator++();   // Prefix increment operator.
    ProgressBar operator++(int); // Postfix increment operator.

    void printProgress();

  private:
    unsigned max;
    unsigned count;
    std::string name;
    unsigned nchars;
    unsigned PBwidth;
};
