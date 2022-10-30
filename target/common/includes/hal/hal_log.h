#pragma once

enum class LogLevel { Error, Warning, Info, Debug, Trace };

void hal_log_init();

void hal_log_raw(const char* text);

void hal_log(LogLevel level, const char* tag, const char* format, ...);