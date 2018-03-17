
#ifndef GRSCFRM_RH_INC
#define GRSCFRM_RH_INC

#define STR_DIAG_BEGIN                1000
#define STR_DIAG_NODATA              (STR_DIAG_BEGIN+0)
#define STR_DIAG_PUCONNECT           (STR_DIAG_BEGIN+1)
#define STR_DIAG_PURESPOND           (STR_DIAG_BEGIN+2)
#define STR_DIAG_RESERV1             (STR_DIAG_BEGIN+3)
#define STR_DIAG_CPCONNECT           (STR_DIAG_BEGIN+4)
#define STR_DIAG_CPRESPOND           (STR_DIAG_BEGIN+5)
#define STR_DIAG_MODRESPOND          (STR_DIAG_BEGIN+6)
#define STR_DIAG_MODTXCHANGES        (STR_DIAG_BEGIN+7)

#define STR_DIAG_RESERV2             (STR_DIAG_BEGIN+8)
#define STR_DIAG_RESERV3             (STR_DIAG_BEGIN+9)
#define STR_DIAG_RESERV4             (STR_DIAG_BEGIN+10)
#define STR_DIAG_RESERV5             (STR_DIAG_BEGIN+11)
#define STR_DIAG_RESERV6             (STR_DIAG_BEGIN+12)
#define STR_DIAG_RESERV7             (STR_DIAG_BEGIN+13)
#define STR_DIAG_RESERV8             (STR_DIAG_BEGIN+14)
#define STR_DIAG_REMOVED             (STR_DIAG_BEGIN+15)

#define STR_PART_DIAG_PARAM          (STR_DIAG_BEGIN+16)
#define STR_PART_DIAG_RESERV1        (STR_DIAG_BEGIN+17)
#define STR_PART_DIAG_RESERV2        (STR_DIAG_BEGIN+18)
#define STR_PART_DIAG_RESERV3        (STR_DIAG_BEGIN+19)

#define STR_DIAG_STATE_NOTRECEIPT    (STR_DIAG_BEGIN+20)
#define STR_DIAG_STATE_TUTR_ERROR    (STR_DIAG_BEGIN+21)
#define STR_DIAG_STATE_RESERV1       (STR_DIAG_BEGIN+22)
#define STR_DIAG_STATE_RESERV2       (STR_DIAG_BEGIN+23)

#define STR_STATE_CALL_SENDED        (STR_DIAG_BEGIN+24)
#define STR_STATE_RESERV2            (STR_DIAG_BEGIN+25)
#define STR_STATE_RESERV3            (STR_DIAG_BEGIN+26)
#define STR_STATE_RESERV4            (STR_DIAG_BEGIN+27)
#define STR_STATE_RESERV5            (STR_DIAG_BEGIN+28)
#define STR_STATE_RESERV6            (STR_DIAG_BEGIN+29)
#define STR_STATE_TUTR_ACTIVE        (STR_DIAG_BEGIN+30)
#define STR_STATE_TUTR_PREPARE       (STR_DIAG_BEGIN+31)

#define STR_PDIAG_NODATA             (STR_DIAG_BEGIN+40)
#define STR_PDIAG_ERROR              (STR_DIAG_BEGIN+41)
#define STR_PDIAG_OUTSCALE           (STR_DIAG_BEGIN+42)
#define STR_PDIAG_TUTR_ERROR         (STR_DIAG_BEGIN+43)

#define STR_PDIAG_TUTR_DESCRIPT      (STR_DIAG_BEGIN+44)
#define STR_PSTATE_RESERV1           (STR_DIAG_BEGIN+45)
#define STR_PSTATE_TUTR_ACTIVE       (STR_DIAG_BEGIN+46)
#define STR_PSTATE_TUTR_PREPARE      (STR_DIAG_BEGIN+47)





#define STR_MON_BEGIN                (2000)

#define STR_MON_RXTYPE               (STR_MON_BEGIN+0)
#define STR_MON_TXTYPE               (STR_MON_BEGIN+1)

#define STR_MON_ERR0                 (STR_MON_BEGIN+2)
#define STR_MON_ERR1                 (STR_MON_BEGIN+3)
#define STR_MON_ERR2                 (STR_MON_BEGIN+4)
#define STR_MON_ERR3                 (STR_MON_BEGIN+5)
#define STR_MON_ERR4                 (STR_MON_BEGIN+6)
#define STR_MON_ERR5                 (STR_MON_BEGIN+7)
#define STR_MON_ERR6                 (STR_MON_BEGIN+8)
#define STR_MON_ERR7                 (STR_MON_BEGIN+9)

#define STR_MON_KADRTYPE0            (STR_MON_BEGIN+10)
#define STR_MON_KADRTYPE1            (STR_MON_BEGIN+11)
#define STR_MON_KADRTYPE2            (STR_MON_BEGIN+12)
#define STR_MON_KADRTYPE3            (STR_MON_BEGIN+13)
#define STR_MON_KADRTYPE4            (STR_MON_BEGIN+14)
#define STR_MON_KADRTYPE5            (STR_MON_BEGIN+15)
#define STR_MON_KADRTYPE6            (STR_MON_BEGIN+16)
#define STR_MON_KADRTYPE7            (STR_MON_BEGIN+17)
#define STR_MON_KADRTYPE8            (STR_MON_BEGIN+18)
#define STR_MON_KADRTYPE9            (STR_MON_BEGIN+19)
#define STR_MON_KADRTYPE10           (STR_MON_BEGIN+20)
#define STR_MON_KADRTYPE11           (STR_MON_BEGIN+21)
#define STR_MON_KADRTYPE12           (STR_MON_BEGIN+22)
#define STR_MON_KADRTYPE13           (STR_MON_BEGIN+23)
#define STR_MON_KADRTYPE14           (STR_MON_BEGIN+24)
#define STR_MON_KADRTYPE15           (STR_MON_BEGIN+25)

#define STR_MON_KADRINF0             (STR_MON_BEGIN+30)
#define STR_MON_KADRINF1             (STR_MON_BEGIN+31)
#define STR_MON_KADRINF2             (STR_MON_BEGIN+32)
#define STR_MON_KADRINF3             (STR_MON_BEGIN+33)
#define STR_MON_KADRINF4             (STR_MON_BEGIN+34)
#define STR_MON_KADRINF5             (STR_MON_BEGIN+35)
#define STR_MON_KADRINF6             (STR_MON_BEGIN+36)
#define STR_MON_KADRINF7             (STR_MON_BEGIN+37)
#define STR_MON_KADRINF8             (STR_MON_BEGIN+38)
#define STR_MON_KADRINF9             (STR_MON_BEGIN+39)
#define STR_MON_KADRINF10            (STR_MON_BEGIN+40)
#define STR_MON_KADRINF11            (STR_MON_BEGIN+41)
#define STR_MON_KADRINF12            (STR_MON_BEGIN+42)
#define STR_MON_KADRINF13            (STR_MON_BEGIN+43)
#define STR_MON_KADRINF14            (STR_MON_BEGIN+44)
#define STR_MON_KADRINF15            (STR_MON_BEGIN+45)

#define STR_MON_CALLAFB00            (STR_MON_BEGIN+50)
#define STR_MON_CALLAFB01            (STR_MON_BEGIN+51)
#define STR_MON_CALLAFB02            (STR_MON_BEGIN+52)
#define STR_MON_CALLAFB03            (STR_MON_BEGIN+53)
#define STR_MON_CALLAFB04            (STR_MON_BEGIN+54)
#define STR_MON_CALLAFB05            (STR_MON_BEGIN+55)
#define STR_MON_CALLAFB06            (STR_MON_BEGIN+56)
#define STR_MON_CALLAFB07            (STR_MON_BEGIN+57)
#define STR_MON_CALLAFB08            (STR_MON_BEGIN+58)
#define STR_MON_CALLAFB09            (STR_MON_BEGIN+59)
#define STR_MON_CALLAFB10            (STR_MON_BEGIN+60)
#define STR_MON_CALLAFB11            (STR_MON_BEGIN+61)
#define STR_MON_CALLAFB12            (STR_MON_BEGIN+62)
#define STR_MON_CALLAFB13            (STR_MON_BEGIN+63)
#define STR_MON_CALLAFB14            (STR_MON_BEGIN+64)
#define STR_MON_CALLAFB15            (STR_MON_BEGIN+65)



#define STR_MON_TUTR_DETAIL          (STR_MON_BEGIN+299)
#define STR_MON_DETAIL1_1            (STR_MON_BEGIN+300)
#define STR_MON_DETAIL1_2            (STR_MON_BEGIN+301)
#define STR_MON_DETAIL1_3            (STR_MON_BEGIN+302)
#define STR_MON_DETAIL2_1            (STR_MON_BEGIN+303)
#define STR_MON_DETAIL2_2            (STR_MON_BEGIN+304)
#define STR_MON_DETAIL2_3            (STR_MON_BEGIN+305)

#define STR_MON_DETAIL3_1            (STR_MON_BEGIN+306)
#define STR_MON_DETAIL3_2            (STR_MON_BEGIN+307)
#define STR_MON_DETAIL3_3            (STR_MON_BEGIN+308)

#define STR_MON_FA0                  (STR_MON_BEGIN+320)
#define STR_MON_FA1                  (STR_MON_BEGIN+321)
#define STR_MON_FA2                  (STR_MON_BEGIN+322)
#define STR_MON_FAALL                (STR_MON_BEGIN+323)

#define STR_MON_FILTER_TEMPL         (STR_MON_BEGIN+325)

#define STR_TU_BEGIN   3000
#define STR_TU_CMD1    (STR_TU_BEGIN+ 0)
#define STR_TU_CMD2    (STR_TU_BEGIN+ 1)
#define STR_TU_CMD3    (STR_TU_BEGIN+ 2)

#define STR_TU_STATE1  (STR_TU_BEGIN+ 3)
#define STR_TU_STATE2  (STR_TU_BEGIN+ 4)
#define STR_TU_STATE3  (STR_TU_BEGIN+ 5)
#define STR_TU_STATE4  (STR_TU_BEGIN+ 6)
#define STR_TU_STATE5  (STR_TU_BEGIN+ 7)
#define STR_TU_STATE6  (STR_TU_BEGIN+ 8)

#define STR_FA_TS       3100
#define STR_FA_TIT      3101
#define STR_FA_TII      3102


#endif

