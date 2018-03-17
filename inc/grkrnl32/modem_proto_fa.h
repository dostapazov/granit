
#ifndef __MODEM_PROTO_FA_INC__
#define __MODEM_PROTO_FA_INC__
//Функциональные адреса данных протокола модема

#define FA_CONTROL                    1
#define FA_OTD                        10
#define FA_OTD_ALARM_ARCHIVE          15 //архивные данные OTD
#define FA_OTD_MEDIUM                 20
#define FA_OTD_MEDIUM_ALARM_ARCHIVE   25
#define FA_SECURITY                   30


#define FA_ALL                        0xFFFF

#define FA_DEVICE_RAW                 0x8000
#define FA_DEVICE_RAW_CPC152          (FA_DEVICE_RAW|0x0001)


#endif
