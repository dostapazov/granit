#ifndef OPC_CODES_H
#define OPC_CODES_H
    
// excerption from opc_da.h (see latest version on www.opcfoundatin.org)

#define    OPC_QUALITY_MASK            0xC0
#define    OPC_STATUS_MASK             0xFC
#define    OPC_LIMIT_MASK              0x03
#define    OPC_QUALITY_BAD             0x00
#define    OPC_QUALITY_UNCERTAIN       0x40
#define    OPC_QUALITY_GOOD            0xC0
#define    OPC_QUALITY_CONFIG_ERROR    0x04
#define    OPC_QUALITY_NOT_CONNECTED   0x08
#define    OPC_QUALITY_DEVICE_FAILURE  0x0c
#define    OPC_QUALITY_SENSOR_FAILURE  0x10
#define    OPC_QUALITY_LAST_KNOWN      0x14
#define    OPC_QUALITY_COMM_FAILURE    0x18
#define    OPC_QUALITY_OUT_OF_SERVICE  0x1C
#define    OPC_QUALITY_LAST_USABLE     0x44
#define    OPC_QUALITY_SENSOR_CAL      0x50
#define    OPC_QUALITY_EGU_EXCEEDED    0x54
#define    OPC_QUALITY_SUB_NORMAL      0x58
#define    OPC_QUALITY_LOCAL_OVERRIDE  0xD8
#define    OPC_LIMIT_OK                0x00
#define    OPC_LIMIT_LOW               0x01
#define    OPC_LIMIT_HIGH              0x02
#define    OPC_LIMIT_CONST             0x03

#endif