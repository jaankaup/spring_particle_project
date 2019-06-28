#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <stdexcept>
#include <string>

/**
 * Yksinkertainen luokka loggausta varten.
 *
 * Esim. Log myLogger("WARNING", std::cout);
 * myLogger.log("TÃmÃ on %. loggaus. Loggaaminen on % kivaa. Tulostetaan && merkki.", 145, "MELKO");
 *
 * tulostus: WARNING: TÃmÃ on 145. loggaus. Loggaaminen on MELKO kivaa. Tulostetaan & merkki.
 *
 * %-merkki korvataan siis merkkijonon jÃlkeen tulevilla argumenteilla. Jos &-merkkien ja argumenttien mÃÃrÃ eivÃt vastaa, log funktio heittÃÃ poikkeuksen.
 */

class Log
{
  public:

    // Singleton methods.
    static Log& getInfo();
    static Log& getDebug();
    static Log& getWarning();
    static Log& getError();

    void log(const char* str) {
      if (pDisabled) return;
      printPrefix();

      while (*str)
      {
        if (*str == '%')
        {
          if (*(str+1) == '%')
          {
            ++str;
          }
          else
          {
            throw std::runtime_error("Log() : invalid format string: missing arguments.");
          }
        }
        os_ << *str++;
      }
      printPrefix_ = true;
      printEOF_ = false;
      os_ << std::endl;
    }

    template <typename T, typename... Args>
      void log(const char* str, T val, Args... args)
      {
        if (pDisabled) return;
        printPrefix();

        while (*str)
        {
          if (*str == '%')
          {
            if (*(str+1) == '%')
            {
              ++str;
            }
            else
            {
              os_ << val;
              ++str;
              log(str, args...);
              return;
            }
          }
          os_ << *str++;
        }
        printPrefix_ = true;
        if (printEOF_)
        {
          os_ << std::endl;
        }
      }


  private:
    Log(const std::string& prefix, std::ostream& output) : prefix_(prefix), os_(output) {};
    ~Log() {};

    std::string prefix_;
    std::ostream& os_;
    bool printPrefix_ = true;
    bool printEOF_ = false;
    bool pDisabled = false;

    void printPrefix()
    {
      if (printPrefix_)
      {
        os_ << prefix_;
        printPrefix_ = false;
      }
    }

};

#endif //LOG_H
