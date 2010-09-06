#include "dcmtkLogger.h"

void dcmtkLogger::debug(OFString message)
{
    Logger::getInstance()->debug(message.c_str());
}

void dcmtkLogger::error(OFString message)
{
    Logger::getInstance()->error(message.c_str());
}

void dcmtkLogger::info(OFString message)
{
    Logger::getInstance()->info(message.c_str());
}

void dcmtkLogger::warning(OFString message)
{
    Logger::getInstance()->warning(message.c_str());
}