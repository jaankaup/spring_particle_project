#include "log.h"

Log& Log::getInfo()
{
  static Log instance("INFO: ",std::cout);
  return instance;
}

Log& Log::getDebug()
{
    static Log instance("DEBUG: ",std::cout);
	/*
  #ifndef DEBUG_PROJECT
    instance.pDisabled = true;
  #endif
  */
  return instance;
}

Log& Log::getWarning()
{
  static Log instance("WARNING: ",std::cout);
  return instance;
}

Log& Log::getError()
{
  static Log instance("ERROR: ",std::cout);
  return instance;
}


