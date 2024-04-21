#ifndef ANSI_COLORS_H
#define ANSI_COLORS_H

#include <string>

namespace Colors {
    // Reset
    const std::string Reset = "\033[0m";

    // Foreground Colors
    const std::string Black = "\033[30m";
    const std::string Red = "\033[31m";
    const std::string Green = "\033[32m";
    const std::string Yellow = "\033[33m";
    const std::string Blue = "\033[34m";
    const std::string Magenta = "\033[35m";
    const std::string Cyan = "\033[36m";
    const std::string White = "\033[37m";

    // Background Colors
    const std::string BBlack = "\033[40m";
    const std::string BRed = "\033[41m";
    const std::string BGreen = "\033[42m";
    const std::string BYellow = "\033[43m";
    const std::string BBlue = "\033[44m";
    const std::string BMagenta = "\033[45m";
    const std::string BCyan = "\033[46m";
    const std::string BWhite = "\033[47m";
}

#endif // ANSI_COLORS_H