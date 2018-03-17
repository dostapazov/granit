#ifndef __MODBUSPROTO_H__
#define __MODBUSPROTO_H__


#ifdef _WINDOWS_
 #include <windef.h>
 #include <ke_defs.h>
#else
 #include <lin_ke_defs.h>
#endif

#pragma pack(push,1)

#ifndef ANYSIZE_ARRAY
#define ANYSIZE_ARRAY 1
#endif


struct MODBUS_SHORT
{
 union{
  struct{
  unsigned char hi_val;
  unsigned char lo_val;
  };
  unsigned short val;
 };
};

//typedef MODBUS_SHORT* LPMODBUS_SHORT;

#define LPMODBUS_SHORT MODBUS_SHORT*

struct MODBUS_DATA
{
  unsigned char command;
  unsigned char data[ANYSIZE_ARRAY];
};

typedef  MODBUS_DATA* LPMODBUS_DATA;
//#define  LPMODBUS_DATA MODBUS_DATA*

struct MODBUS_FRAME
{
  unsigned char addr;
  MODBUS_DATA   data;
};

typedef MODBUS_FRAME*  LPMODBUS_FRAME;

#define LPMODBUS_FRAME MODBUS_FRAME*

struct MODBUS_TCP_HEADER
{
 unsigned short frame_number;
 unsigned short proto_id;
 unsigned short frame_len;
};

//#define LPMODBUS_TCP_HEADER MODBUS_TCP_HEADER*
typedef  MODBUS_TCP_HEADER* LPMODBUS_TCP_HEADER;


struct MODBUS_TCP_FRAME
{
  MODBUS_TCP_HEADER hdr ;
  MODBUS_FRAME      modbus_frame;
};

//#define LPMODBUS_TCP_FRAME MODBUS_TCP_FRAME*
typedef  MODBUS_TCP_FRAME* LPMODBUS_TCP_FRAME;



struct MODBUS_STD_REQUEST
{
  MODBUS_SHORT   req_from;
  MODBUS_SHORT   req_count;
};

#define  LPMODBUS_STD_REQUEST MODBUS_STD_REQUEST*


struct MODBUS_PRESET_DATA
{
 MODBUS_SHORT   preset_number;
 MODBUS_SHORT   value;
};

#define LPMODBUS_PRESET_DATA MODBUS_PRESET_DATA*




#define init_tcp_frame(ptr,fr_number) ((LPMODBUS_TCP_FRAME)ptr)->hdr.frame_number = fr_number,((LPMODBUS_TCP_FRAME)ptr)->hdr.proto_id=((LPMODBUS_TCP_FRAME)ptr)->hdr.frame_len=0
#define modbus_set_short(ptr,val) ((LPMODBUS_SHORT)ptr)->lo_val = (((unsigned char)val)&0xFF),((LPMODBUS_SHORT)ptr)->hi_val = (((unsigned char)(val>>8))&0xFF)
#define modbus_get_short(ptr) (((LPMODBUS_SHORT)ptr)->lo_val | ((((unsigned short) ((LPMODBUS_SHORT)ptr)->hi_val )<<8)&0xFF00))

#define get_modbus_data_len(ptr) (int)(((LPMODBUS_FRAME)ptr)->data.data[0])


/*Флаг ошибки*/

#define MODBUS_RESPOND_ERROR_FLAG       0x80
#define MODBUS_RESPOND_COMMAND_MASK(cmd)  (cmd&~(MODBUS_RESPOND_ERROR_FLAG))

#define MODBUS_READ_DISCRETE_OUTPUT     0x01
#define MODBUS_READ_DISCRETE_INPUT      0x02
#define MODBUS_READ_HOLDING_REGISTERS   0x03
#define MODBUS_READ_INPUT_REGISTERS     0x04



#define MODBUS_FORCE_SINGLE_COIL        0x05
#define MODBUS_FORCE_ON                 0xFF00
#define MODBUS_FORCE_OFF                0x0000

#define MODBUS_PRESET_SINGLE_REGISTER   0x06

#define MODBUS_FETCH_COMMEVENT_CTRL     0x0B
#define MODBUS_FETCH_COMMEVENT_LOG      0x0C

#define MODBUS_DISCRETE_BITS            1
#define MODBUS_ANALOG_BITS              16

/*Коды ошибок при ответе*/
#define MODBUS_ERR_ILLEGAL_FUNCTION       1
#define MODBUS_ERR_ILLEGAL DATA_ADDRESS   2
#define MODBUS_ERR_ILLEGAL_DATA_VALUE     3
#define MODBUS_ERR_SLAVE_DEVICE_FAILURE   4
#define MODBUS_ERR_ACKNOWLEDGE            5
#define MODBUS_ERR_SLAVE_DEVICE_BUSY      6
#define MODBUS_ERR_NEGATIVE_ACKNOWLEDGE   7
#define MODBUS_ERR_MEMORY_PARITY_ERROR    8

#ifdef __cplusplus

extern "C"
{
#endif

unsigned short EXPORT modbus_calc_crc          (unsigned char * buff,int blen);
           int EXPORT modbus_protect_crc       (void * data  ,int data_len,int buff_size,int * new_len);
           int EXPORT modbus_check_crc         (void * data  ,int data_len);

           int EXPORT modbus_ascii2bin         (char * src,int src_len,unsigned char * dst,int dst_len,int hi_val_last);
           int EXPORT modbus_bin2ascii         (unsigned char * src,int src_len,char * dst,int dst_len,int hi_val_last);
           int EXPORT modbus_format_query      (void * dst,int dst_len,int addr  ,int command,int v_start, int v_count,int * need_len);
           int EXPORT modbus_format_preset     (void * dst,int dst_len,int addr,int command,int number,unsigned short value,int * need_len);
           #define modbus_format_set_coil(dst,dst_len,addr,swon)  modbus_format_preset(dst,dst_len,addr,MODBUS_FORCE_SINGLE_COIL,swon ? MODBUS_FORCE_ON : MODBUS_FORCE_OFF,NULL);

           int EXPORT modbus_get_discrete      (LPMODBUS_DATA  data,int number,int * state );
           int EXPORT modbus_get_analog        (LPMODBUS_DATA  data,int number,int * astate);
           int EXPORT modbus_set_discrete      (LPMODBUS_DATA  data,int number,int   state );
           int EXPORT modbus_set_analog        (LPMODBUS_DATA  data,int number,int   astate);

           int EXPORT modbus_format_respond    (void * dst   ,int dst_len,int addr,int command,int count,int bits,int * need_len);


#ifdef __cplusplus
}
        inline unsigned short modbus_word(unsigned short x)            { return ((x&0x00FF)<<8)| ((x&0xFF00)>>8); }
        inline int modbus_get_tcp_length(LPMODBUS_TCP_FRAME  tcp_frame){ return sizeof(tcp_frame->hdr)+modbus_word(tcp_frame->hdr.frame_len); }
        inline int modbus_is_respond_error(LPMODBUS_FRAME mf)          { return (mf->data.command&MODBUS_RESPOND_ERROR_FLAG) ? (int)mf->data.data[0] :0 ;  }
        inline int modbus_get_respond_length(LPMODBUS_FRAME mf)        { return (modbus_is_respond_error(mf)) ? (int)0 : (int)mf->data.data[0]; }


#else
     #define modbus_get_tcp_length(ptr)   ( modbus_word(((LPMODBUS_TCP_FRAME)ptr)->hdr.frame_length)+sizeof(MODBUS_TCP_HEADER) )
     #define modbus_is_respond_error(ptr) (((LPMODBUS_FRAME)ptr)->data.command&MODBUS_RESPOND_ERROR_FLAG) ? (int)((LPMODBUS_FRAME)ptr)->data.data[0] : (int) 0 )
     #define modbus_word(x) ( ((x&0x00FF)<<8)| ((x&0xFF00)>>8) )
#endif







#pragma pack(pop)

#endif
