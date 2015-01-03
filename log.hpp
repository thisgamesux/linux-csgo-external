#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <linux/limits.h>
#include <unistd.h>

namespace log
{
    extern void init();
    extern void put(std::string str);
};