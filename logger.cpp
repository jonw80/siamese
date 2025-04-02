#include "Logger.h"

namespace logger {

// Constructor for Channel
Channel::Channel(const char* name, Level minLevel)
    : ChannelName(name), ChannelMinLevel(minLevel) // Corrected member names
{
    // Initialize any additional members if needed
}

} // namespace logger
