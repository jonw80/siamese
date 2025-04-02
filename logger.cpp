#include "Logger.h"

namespace logger {

// Constructor for Channel
Channel::Channel(const char* name, Level minLevel);

// Update kLevelStrings to match the Level enum
static const char* kLevelStrings[(int)Level::Count] = {
    "Off", "Error", "Warning", "Info", "Debug"
};

// Update kLevelChars to match the Level enum
static const char kLevelChars[(int)Level::Count] = {
    'O', 'E', 'W', 'I', 'D'
};

// Ensure the arrays match the Level enum
static_assert((int)Level::Count == 7, "Update this switch");

} // namespace logger
