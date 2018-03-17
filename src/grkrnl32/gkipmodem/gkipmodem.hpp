#ifndef __GKIPMODEM_HPP_INC
#define __GKIPMODEM_HPP_INC

#include "gkipmodem.h"
#include <modem.hpp>

class ip_line: public modem_line
{
 protected:
 public   :
 ip_line(DWORD number);
};

class TGkIPModem:public TModemBase
{
 protected:
 public   :
 TGkIPModem();

};

#endif
 