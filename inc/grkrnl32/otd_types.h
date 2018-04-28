#ifndef OTD_TYPES_H
#define OTD_TYPES_H



 /*���������*/
 #define OTD_ADDR_ALLMOD 0xFF000000
 #define OTD_ADDR_ALLFA  0x00FF0000
 #define OTD_ADDR_ALLCP  0x0000FF00
 #define OTD_ADDR_ALLPU  0x000000FF
 #define OTD_ADDR_ALL    (OTD_ADDR_ALLMOD|OTD_ADDR_ALLFA|OTD_ADDR_ALLCP|OTD_ADDR_ALLPU)
 #define OTD_ADDR_MAXVALUE    255


 /*���� ������ */

 #define OTD_TYPE_MASK           0x7000 //����� ������� �����
 #define OTD_BIT_TYPE            0x1000
 #define OTD_BYTE_TYPE           0x2000
 #define OTD_FLOAT_TYPE          0x4000
 #define OTD_SIGNED_FLOAT_TYPE   0xC000 //Float type always signed
 #define OTD_SIGNED_TYPE         0x8000

 #define OTD_GET_TYPE(type)   (type&OTD_TYPE_MASK)
 #define OTD_TYPE_SIGNED(TYPE)(type&OTD_SIGNED_TYPE)


 #define OTD_TYPE_LEN  0x0FFF //����� ����� ����

 #define OTD_DISCRETE        (OTD_BIT_TYPE |1)
 #define OTD_BYTE            (OTD_BYTE_TYPE|1)
 #define OTD_ANALOG_BYTE     (OTD_BYTE_TYPE|1)
 #define OTD_ANALOG_WORD     (OTD_BYTE_TYPE|2)
 #define OTD_ANALOG_SHORT    (OTD_BYTE_TYPE|OTD_SIGNED_TYPE|2)
 #define OTD_ANALOG_DWORD    (OTD_BYTE_TYPE|4)
 #define OTD_ANALOG_LONG     (OTD_BYTE_TYPE|OTD_SIGNED_TYPE|4)
 #define OTD_ANALOG_QWORD    (OTD_BYTE_TYPE|8)
 #define OTD_ANALOG_LONGLONG (OTD_BYTE_TYPE|OTD_SIGNED_TYPE|8)
 #define OTD_POINTER         (OTD_BYTE_TYPE|sizeof(LPVOID))
 #define OTD_FLOAT           (OTD_FLOAT_TYPE|sizeof(float))
 #define OTD_SIGNED_FLOAT    (OTD_SIGNED_FLOAT_TYPE|sizeof(float))
 #define OTD_DOUBLE          (OTD_FLOAT_TYPE|sizeof(double))
 #define OTD_SIGNED_DOUBLE   (OTD_SIGNED_FLOAT_TYPE|sizeof(double))
 #define OTD_TUTR_COMMAND    (OTD_BYTE_TYPE|4)
 #define OTD_TR_COMMAND_SET  (OTD_BYTE_TYPE|(4+8))


/*��������������� ���� ������*/
#define OTD_BLOCKTYPE_TEXT      1
#define OTD_BLOCKTYPE_ERROR     2
#define OTD_BLOCKTYPE_ALARM     3
#define OTD_BLOCKTYPE_START_OSCILLOGRAMM 6






/*���� �� ��� ���*/

#define OTD_FA_CALL          0x80 /*�����*/
#define OTD_FA_ORDER         0x40 /*��������*/
#define OTD_FA_QUERY         (OTD_FA_ORDER|OTD_FA_CALL)
#define OTD_FA_RECEIPT       0x20 /*���������*/
#define OTD_FA_CTRL          0x10 /*����������/�������������*/
#define OTD_FA_SETUP         0x08 /*��������� ��������� ������������*/

#define OTD_FA_DISCRETE      0x00 /* ���������� ��������       */
#define OTD_FA_ANALOG        0x01 /* ���������� ��������       */
#define OTD_FA_ANALOG_INTEGR 0x02 /* ���������� ��������1      */
#define OTD_FA_INFORMATION   0x04 /* ������ ����������         */

#define OTD_FA_DICRETE_CTRL  (OTD_FA_CTRL|OTD_FA_DISCRETE) /* ���������� �����������/������������� ����� ��    */
#define OTD_FA_ANALOG_ADJ    (OTD_FA_CTRL|OTD_FA_ANALOG)   /* ������������� */
#define OTD_FA_ANALOG1_ADJ   (OTD_FA_CTRL|OTD_FA_ANALOG1)   /* ������������� */

#define OTD_FA_ALL           0x07
#define OTD_FA_DATA_MASK     0x07


//������� �������� ��

#define OTD_FA_ISQUERY(x)     ((x!= OTD_ADDR_MAXVALUE) && (x&OTD_FA_QUERY) )
#define OTD_FA_ISALL(x)       ((x&OTD_FA_ALL)==OTD_FA_ALL)
#define OTD_FA_ISQUERY_ALL(x) (OTD_FA_ISQUERY(x) &&  OTD_FA_ISALL(x))
#define OTD_FA_ISCTRL(x)      ((x&OTD_FA_CTRL ) && (0xF8!=(x&0xF8)) && !OTD_FA_ISALL(x))

#define OTD_FA_ISSETUP(x)     ((x!= OTD_ADDR_MAXVALUE) && (x&OTD_FA_SETUP) && 0==(x&~(OTD_FA_ALL|OTD_FA_SETUP)))

#define OTD_FA_ISDATA(x)      (!(x&~OTD_FA_DATA_MASK))

#endif

