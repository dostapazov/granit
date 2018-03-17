#ifndef OPC_SERV32_CONSTS
#define  OPC_SERV32_CONSTS


#define  EVENT_CHANGE_SETTINGS        "$OPC_GRKERNEL_CHANGE_SETTINGS$"

#define   CMD_RUN_OPC                 "-run_opc"
#define   CMD_REG_OPC                 "-reg"
#define   CMD_UNREG_OPC               "-unreg"
#define   CMD_CONFIG_OPC              "-config"

#define   OPC_PROG_ID                 "GRKERNEL OPC SERVER"
#define   OPC_GUID                    "{11FC0C56-1DE6-45F5-A940-37B6DAFDA76D}"
#define   REG_OPC_SERVER_KEY_TMPL     "SOFTWARE\\CLASSES\\%s\\"
#define   REG_OPC_SERVER_CONFIG       "CONFIG"

#define   REGSTR_OTD_SERVER_ADDR      "OTD_SERVER_ADDR"
#define   REGSTR_OPC_SERVER_NAME      "OPC_SRV_NAME"

#define   REGBIN_ORDER_KADRS          "OTD_ORDER_KADRS"

#define   REGDW_OTD_SERVER_PORT       "OTD_SERVER_PORT"
#define   REGDW_OPC_REFRESH_RATE      "OPC_REFRESH_RATE"
#define   REGDW_OPC_REFRESH_RATE_MIN  "OPC_REFRESH_RATE_MIN"
#define   REGDW_OPC_MAX_QUEUE_COUNT   "OPC_MAX_QUEUE_COUNT"
#define   REGDW_OPC_BRANCH_SEP        "OPC_BRANCH_SEP"
#define   REGDW_OPC_TAGS_COUNT        "OPC_TAGS_COUNT"
#define   REGDW_OPC_TAG_NAME_MODE     "OPC_TAG_NAME_MODE"
#define   REGDW_OPC_USE_CONSOLE       "OPC_USE_CONSOLE"

#define   REGDW_OPC_NO_CLIENT_TERMINATE_DELAY "NoClientTerminateDelay"

#endif
