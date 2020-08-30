
#include "ProgressBar.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

ProgressBar::ProgressBar(unsigned p_max, const std::string p_name,
                         unsigned p_PBwidth)
    : max(p_max), count(0), name(p_name), PBwidth(p_PBwidth)
{
    nchars = (int)log10(max) + 1;
    printProgress();
}

void ProgressBar::printProgress()
{
    static char PBchar = '=';
    static char PBchar2 = '>';
    static char PBchar3 = '.';

    float percentage = (float)count / max;
    unsigned lpad = percentage * PBwidth;
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss << "\r";
    ss << name << " " << std::setfill(' ') << std::setw(nchars) << count << "/"
       << max << " [";
    unsigned i = 0;
    for (; i < lpad; ++i)
        ss << PBchar;
    if (i < PBwidth)
    {
        ss << PBchar2;
        ++i;
    }
    for (; i < PBwidth; ++i)
        ss << PBchar3;
    ss << "]";
    std::cout << ss.str() << std::flush;
}

// Define prefix increment operator.
ProgressBar &ProgressBar::operator++()
{
    if (count < max)
        ++count;
    printProgress();
    return *this;
}

// Define postfix increment operator.
ProgressBar ProgressBar::operator++(int)
{
    ProgressBar temp = *this;
    printProgress();
    if (count < max)
        count++;
    return temp;
}