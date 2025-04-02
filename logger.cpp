#include "Logger.h"

namespace logger {

// Ensure the arrays match the Level enum
static_assert((int)Level::Count == 7, "Update this switch");

} // namespace logger
