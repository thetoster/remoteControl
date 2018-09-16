#ifndef DEBUG_H
#define DEBUG_H

#if LOG_ENABLED!=0
  #warning Debug Logs Enabled!
  #define LOG(x) Serial.print(x)
  #define LOG_LN(x) Serial.println(x)
#else
  #define LOG(x) ;
  #define LOG_LN(x) ;
#endif

#endif
