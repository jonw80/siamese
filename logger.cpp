#include "Logger.h"

namespace logger {

// Constructor for Channel
Channel::Channel(const char* name, Level minLevel)
    : ChannelName(name), ChannelMinLevel(minLevel)
{
    // Initialize any additional members if needed
}

// Update kLevelStrings to match the Level enum
static const char* kLevelStrings[(int)Level::Count] = {
    "Debug", "Info", "Warning", "Error", "Off"
};

// Update kLevelChars to match the Level enum
static const char kLevelChars[(int)Level::Count] = {
    'D', 'I', 'W', 'E', 'O'
};

// Ensure the arrays match the Level enum
static_assert((int)Level::Count == 5, "Update this switch");

} // namespace logger
