/*
  Перечислние типов модемов
*/
#ifndef _MODEM_TYPES_INC_
#define _MODEM_TYPES_INC_

#define MT_MODEM_DEFAULT  1

#define MT_MODEM_TYPE(x) (MT_MODEMMER|(x&0xFF))
//Тип модуля реализующего модем
#define MT_MODEM        MT_MODEM_TYPE(MT_MODEM_DEFAULT)
#define IS_MOD_MODEM(v) ((v&0xFF00)== MT_MODEMMER)

#define MT_MODEM_TCP      MT_MODEM_TYPE(2)
#define MT_MODEM_GRANIT   MT_MODEM_TYPE(3)
#define MT_MODEM_ATONY    MT_MODEM_TYPE(4)
#define MT_MODEM_VTD      MT_MODEM_TYPE(5)
#define MT_MODEM_MODBUS   MT_MODEM_TYPE(6)
#define MT_MODEM_CPC152   MT_MODEM_TYPE(7)
#define MT_MODEM_IEC60870 MT_MODEM_TYPE(8)
#define MT_MODEM_OPC      MT_MODEM_TYPE(9)



#endif


 