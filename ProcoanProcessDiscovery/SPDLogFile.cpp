#include "stdafx.h"

#include "SPDLogFile.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


SPDLogFile::SPDLogFile(char *fileName)
{
	// change log pattern
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	
	// Set the default logger to file logger
	auto file_logger = spdlog::basic_logger_mt("PPDLogger", fileName);
	spdlog::set_default_logger(file_logger);
}


SPDLogFile::~SPDLogFile()
{
	spdlog::drop_all();
	spdlog::shutdown();
	
}
