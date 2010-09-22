#include "dcmtkLogger.h"

void dcmtkLogger::debug(OFString message)
{
    BaseLogger::getInstance()->debug(message.c_str());
}

void dcmtkLogger::error(OFString message)
{
    BaseLogger::getInstance()->error(message.c_str());
}

void dcmtkLogger::info(OFString message)
{
    BaseLogger::getInstance()->info(message.c_str());
}

void dcmtkLogger::warning(OFString message)
{
    BaseLogger::getInstance()->warning(message.c_str());
}