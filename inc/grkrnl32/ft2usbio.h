
#ifndef _FT2USBIO_INC_
#define _FT2USBIO_INC_

#ifndef RC_INVOKED

#include <gkio_base.h>
#include <ke_defs.h>
#include <ftd2xx.h>

#pragma pack(push,1)


#define FT2USBIO_RAW_FLAG_BEGIN     0x2A
#define FT2USBIO_RAW_FLAG_END       0x3B

/*Типы данных*/

#define FT2USB_DT_GRANIT            0x30  // Данные - Гранит
#define FT2USB_DT_GRANIT_CONNECT    0x31  // Данные - Гранит Обрыв связи
#define FT2USB_DT_TM512             0x40  // Данные ТМ512
#define FT2USB_DT_TM512_CONNECT     0x41  // Данные ТМ512  Обрыв связи
#define FT2USB_DT_VRTF3             0x50  // Данные ВРТФ-3
#define FT2USB_DT_VRTF3_CONNECT     0x51  // Данные ВРТФ-3 Обрыв связи

#define FT2USB_DT_FILLER            0x0E  //Заполнитель

//Канал 255
//IN
#define FT2USB_DT_RESTART            0x0F  //Сообщение о перезапуске
#define FT2USB_DT_BADCOMMAND         0xF1  //Ошибка команды
#define FT2USB_DT_BADCHANNEL         0xF2  //Ошибка номера канале    data[0] = Номер неправльного канала
#define FT2USB_DT_INTERNAL_ERROR     0x0D  //Ошибка внутреннего обмена
#define FT2USB_DT_SYNCERR            0xFE  //Ошибка синхронизации 2A - 3B data[0] - Номер пакета
//OUT
#define FT2USB_DT_RESET              0xF0 //Команда перезапуска устройства



#define MT_GRSCIO                   (MT_IO+1)

/*
   0x8X                         Ошибка      1  cmd, 2 err_chanel 3 - sync  15 internal
   0x4X                         Инф         1  пререзапуск
   0x1X                         данные      0 = Гранит, 1 - ТМ512 2- ВРТФ-3
   0ч18X
   0x0X                         Кjманда

*/

/*Кадр от устройства*/
typedef struct _usb_dev_kadr
{
 BYTE       chanel;
 BYTE       data_len;
 BYTE       data_type;
 BYTE       data[24];
}usb_dev_kadr,*lpusb_dev_kadr;


/*Пакет от устройства*/
typedef struct _usb_dev_rxpacket
{
 //BYTE         packet_type;
 BYTE           packet_len;
 BYTE           packet_num;
 usb_dev_kadr   kadrs[4];
}usb_dev_rxpacket,* lpusb_dev_rxpacket;

#define FT2USB_RX_RAWDATA_SIZE ((sizeof(usb_dev_rxpacket)<<1)+2*sizeof(BYTE))
#define FT2USB_TX_RAWDATA_SIZE ((sizeof(usb_dev_txpacket)<<1)+2*sizeof(BYTE))

typedef struct _usb_dev_txpacket
{
 //BYTE         packet_type;
 BYTE           packet_len;
 BYTE           packet_num;
 usb_dev_kadr   kadrs[2];
}usb_dev_txpacket,* lpusb_dev_txpacket;


#pragma pack(pop)
#endif // RC_INVOKED
 #define  IDS_REPORT_TITLE     1000
 #define  IDS_REPORT_TMPL1     1001
 #define  IDS_REPORT_TMPL2     1002
 #define  IDS_REPORT_TMPL3     1003
 #define  IDS_REPORT_TMPL4     1004
 #define  IDS_REPORT_TMPL5     1005
 #define  IDS_REPORT_TMPL6     1006
 #define  IDS_REPORT_TMPL7     1007
 #define  IDS_REPORT_TMPL8     1008
 #define  IDS_REPORT_TMPL9     1009
 #define  IDS_REPORT_TMPL10    1010
 #define  IDS_REPORT_TMPL11    1011
 #define  IDS_REPORT_TMPL12    1012
 #define  IDS_INACTIVITY_RESET 1500
 #define  IDS_FTD2_ERROR     2000



#endif

