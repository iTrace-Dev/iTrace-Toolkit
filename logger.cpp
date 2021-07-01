///
/// This file is required because, for whatever reason, QT throws a fit
/// when static members are defined inside header files
///
#include "logger.h"
Logger* Logger::internal_inst = nullptr;
