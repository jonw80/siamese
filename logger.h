#ifndef LOGGER_H
#define LOGGER_H

namespace logger {

enum class Level {
    Info,
    Warning,
    Error
};

class Channel {
public:
    Channel(const char* name, Level level);
    // ...other methods...
private:
    const char* name;
    Level level;
};

} // namespace logger

#endif // LOGGER_H
