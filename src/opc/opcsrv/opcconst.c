#pragma hdrstop
#include <ole2.h>

const LPCWSTR OPC_CATEGORY_DESCRIPTION_DA10	=	L"OPC Data Access Servers Version 1.0";
const LPCWSTR OPC_CATEGORY_DESCRIPTION_DA20	=	L"OPC Data Access Servers Version 2.0";
const LPCWSTR OPC_CATEGORY_DESCRIPTION_DA30	=	L"OPC Data Access Servers Version 3.0";
const LPCWSTR OPC_CATEGORY_DESCRIPTION_XMLDA10	=	L"OPC XML Data Access Servers Version 1.0";
const DWORD OPC_READABLE	=	0x1;
const DWORD OPC_WRITEABLE	=	0x2;
const DWORD OPC_BROWSE_HASCHILDREN	=	0x1;
const DWORD OPC_BROWSE_ISITEM	=	0x2;
const LPCWSTR OPC_TYPE_SYSTEM_OPCBINARY	=	L"OPCBinary";
const LPCWSTR OPC_TYPE_SYSTEM_XMLSCHEMA	=	L"XMLSchema";
const LPCWSTR OPC_CONSISTENCY_WINDOW_UNKNOWN	=	L"Unknown";
const LPCWSTR OPC_CONSISTENCY_WINDOW_NOT_CONSISTENT	=	L"Not Consistent";
const LPCWSTR OPC_WRITE_BEHAVIOR_BEST_EFFORT	=	L"Best Effort";
const LPCWSTR OPC_WRITE_BEHAVIOR_ALL_OR_NOTHING	=	L"All or Nothing";

const WORD OPC_QUALITY_MASK	=	0xc0;
const WORD OPC_STATUS_MASK	=	0xfc;
const WORD OPC_LIMIT_MASK	=	0x3;
const WORD OPC_QUALITY_BAD	=	0;
const WORD OPC_QUALITY_UNCERTAIN	=	0x40;
const WORD OPC_QUALITY_GOOD	=	0xc0;
const WORD OPC_QUALITY_CONFIG_ERROR	=	0x4;
const WORD OPC_QUALITY_NOT_CONNECTED	=	0x8;
const WORD OPC_QUALITY_DEVICE_FAILURE	=	0xc;
const WORD OPC_QUALITY_SENSOR_FAILURE	=	0x10;
const WORD OPC_QUALITY_LAST_KNOWN	=	0x14;
const WORD OPC_QUALITY_COMM_FAILURE	=	0x18;
const WORD OPC_QUALITY_OUT_OF_SERVICE	=	0x1c;
const WORD OPC_QUALITY_WAITING_FOR_INITIAL_DATA	=	0x20;
const WORD OPC_QUALITY_LAST_USABLE	=	0x44;
const WORD OPC_QUALITY_SENSOR_CAL	=	0x50;
const WORD OPC_QUALITY_EGU_EXCEEDED	=	0x54;
const WORD OPC_QUALITY_SUB_NORMAL	=	0x58;
const WORD OPC_QUALITY_LOCAL_OVERRIDE	=	0xd8;
const WORD OPC_LIMIT_OK	=	0;
const WORD OPC_LIMIT_LOW	=	0x1;
const WORD OPC_LIMIT_HIGH	=	0x2;
const WORD OPC_LIMIT_CONST	=	0x3;

const DWORD OPC_PROPERTY_DATATYPE	=	1;
const DWORD OPC_PROPERTY_VALUE	=	2;
const DWORD OPC_PROPERTY_QUALITY	=	3;
const DWORD OPC_PROPERTY_TIMESTAMP	=	4;
const DWORD OPC_PROPERTY_ACCESS_RIGHTS	=	5;
const DWORD OPC_PROPERTY_SCAN_RATE	=	6;
const DWORD OPC_PROPERTY_EU_TYPE	=	7;
const DWORD OPC_PROPERTY_EU_INFO	=	8;
const DWORD OPC_PROPERTY_EU_UNITS	=	100;
const DWORD OPC_PROPERTY_DESCRIPTION	=	101;
const DWORD OPC_PROPERTY_HIGH_EU	=	102;
const DWORD OPC_PROPERTY_LOW_EU	=	103;
const DWORD OPC_PROPERTY_HIGH_IR	=	104;
const DWORD OPC_PROPERTY_LOW_IR	=	105;
const DWORD OPC_PROPERTY_CLOSE_LABEL	=	106;
const DWORD OPC_PROPERTY_OPEN_LABEL	=	107;
const DWORD OPC_PROPERTY_TIMEZONE	=	108;
const DWORD OPC_PROPERTY_CONDITION_STATUS	=	300;
const DWORD OPC_PROPERTY_ALARM_QUICK_HELP	=	301;
const DWORD OPC_PROPERTY_ALARM_AREA_LIST	=	302;
const DWORD OPC_PROPERTY_PRIMARY_ALARM_AREA	=	303;
const DWORD OPC_PROPERTY_CONDITION_LOGIC	=	304;
const DWORD OPC_PROPERTY_LIMIT_EXCEEDED	=	305;
const DWORD OPC_PROPERTY_DEADBAND	=	306;
const DWORD OPC_PROPERTY_HIHI_LIMIT	=	307;
const DWORD OPC_PROPERTY_HI_LIMIT	=	308;
const DWORD OPC_PROPERTY_LO_LIMIT	=	309;
const DWORD OPC_PROPERTY_LOLO_LIMIT	=	310;
const DWORD OPC_PROPERTY_CHANGE_RATE_LIMIT	=	311;
const DWORD OPC_PROPERTY_DEVIATION_LIMIT	=	312;
const DWORD OPC_PROPERTY_SOUND_FILE	=	313;
const DWORD OPC_PROPERTY_TYPE_SYSTEM_ID	=	600;
const DWORD OPC_PROPERTY_DICTIONARY_ID	=	601;
const DWORD OPC_PROPERTY_TYPE_ID	=	602;
const DWORD OPC_PROPERTY_DICTIONARY	=	603;
const DWORD OPC_PROPERTY_TYPE_DESCRIPTION	=	604;
const DWORD OPC_PROPERTY_CONSISTENCY_WINDOW	=	605;
const DWORD OPC_PROPERTY_WRITE_BEHAVIOR	=	606;
const DWORD OPC_PROPERTY_UNCONVERTED_ITEM_ID	=	607;
const DWORD OPC_PROPERTY_UNFILTERED_ITEM_ID	=	608;
const DWORD OPC_PROPERTY_DATA_FILTER_VALUE	=	609;
const LPCWSTR OPC_PROPERTY_DESC_DATATYPE	=	L"Item Canonical Data Type";
const LPCWSTR OPC_PROPERTY_DESC_VALUE	=	L"Item Value";
const LPCWSTR OPC_PROPERTY_DESC_QUALITY	=	L"Item Quality";
const LPCWSTR OPC_PROPERTY_DESC_TIMESTAMP	=	L"Item Timestamp";
const LPCWSTR OPC_PROPERTY_DESC_ACCESS_RIGHTS	=	L"Item Access Rights";
const LPCWSTR OPC_PROPERTY_DESC_SCAN_RATE	=	L"Server Scan Rate";
const LPCWSTR OPC_PROPERTY_DESC_EU_TYPE	=	L"Item EU Type";
const LPCWSTR OPC_PROPERTY_DESC_EU_INFO	=	L"Item EU Info";
const LPCWSTR OPC_PROPERTY_DESC_EU_UNITS	=	L"EU Units";
const LPCWSTR OPC_PROPERTY_DESC_DESCRIPTION	=	L"Item Description";
const LPCWSTR OPC_PROPERTY_DESC_HIGH_EU	=	L"High EU";
const LPCWSTR OPC_PROPERTY_DESC_LOW_EU	=	L"Low EU";
const LPCWSTR OPC_PROPERTY_DESC_HIGH_IR	=	L"High Instrument Range";
const LPCWSTR OPC_PROPERTY_DESC_LOW_IR	=	L"Low Instrument Range";
const LPCWSTR OPC_PROPERTY_DESC_CLOSE_LABEL	=	L"Contact Close Label";
const LPCWSTR OPC_PROPERTY_DESC_OPEN_LABEL	=	L"Contact Open Label";
const LPCWSTR OPC_PROPERTY_DESC_TIMEZONE	=	L"Item Timezone";
const LPCWSTR OPC_PROPERTY_DESC_CONDITION_STATUS	=	L"Condition Status";
const LPCWSTR OPC_PROPERTY_DESC_ALARM_QUICK_HELP	=	L"Alarm Quick Help";
const LPCWSTR OPC_PROPERTY_DESC_ALARM_AREA_LIST	=	L"Alarm Area List";
const LPCWSTR OPC_PROPERTY_DESC_PRIMARY_ALARM_AREA	=	L"Primary Alarm Area";
const LPCWSTR OPC_PROPERTY_DESC_CONDITION_LOGIC	=	L"Condition Logic";
const LPCWSTR OPC_PROPERTY_DESC_LIMIT_EXCEEDED	=	L"Limit Exceeded";
const LPCWSTR OPC_PROPERTY_DESC_DEADBAND	=	L"Deadband";
const LPCWSTR OPC_PROPERTY_DESC_HIHI_LIMIT	=	L"HiHi Limit";
const LPCWSTR OPC_PROPERTY_DESC_HI_LIMIT	=	L"Hi Limit";
const LPCWSTR OPC_PROPERTY_DESC_LO_LIMIT	=	L"Lo Limit";
const LPCWSTR OPC_PROPERTY_DESC_LOLO_LIMIT	=	L"LoLo Limit";
const LPCWSTR OPC_PROPERTY_DESC_CHANGE_RATE_LIMIT	=	L"Rate of Change Limit";
const LPCWSTR OPC_PROPERTY_DESC_DEVIATION_LIMIT	=	L"Deviation Limit";
const LPCWSTR OPC_PROPERTY_DESC_SOUND_FILE	=	L"Sound File";
const LPCWSTR OPC_PROPERTY_DESC_TYPE_SYSTEM_ID	=	L"Type System ID";
const LPCWSTR OPC_PROPERTY_DESC_DICTIONARY_ID	=	L"Dictionary ID";
const LPCWSTR OPC_PROPERTY_DESC_TYPE_ID	=	L"Type ID";
const LPCWSTR OPC_PROPERTY_DESC_DICTIONARY	=	L"Dictionary";
const LPCWSTR OPC_PROPERTY_DESC_TYPE_DESCRIPTION	=	L"Type Description";
const LPCWSTR OPC_PROPERTY_DESC_CONSISTENCY_WINDOW	=	L"Consistency Window";
const LPCWSTR OPC_PROPERTY_DESC_WRITE_BEHAVIOR	=	L"Write Behavior";
const LPCWSTR OPC_PROPERTY_DESC_UNCONVERTED_ITEM_ID	=	L"Unconverted Item ID";
const LPCWSTR OPC_PROPERTY_DESC_UNFILTERED_ITEM_ID	=	L"Unfiltered Item ID";
const LPCWSTR OPC_PROPERTY_DESC_DATA_FILTER_VALUE	=	L"Data Filter Value";



