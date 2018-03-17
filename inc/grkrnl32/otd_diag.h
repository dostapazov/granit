/*-----------------------------------------------------------------------------
 03.02.2004 15:24:16 
 Модуль:   otd_diag
 Автор:    Остапенко Д.В.
 Назначение: Описания диагностики

-----------------------------------------------------------------------------*/

#ifndef OTD_DIAG_H
#define OTD_DIAG_H 1

/*
 Диагностика - средство для обеспечения определения работоспособности объекта(ов)
 а равно и достоверности в текущий момент времени.
 при состоянии диагностики == 0 всё работает и всё достоверно


 Диагностика бывает групповой и индивидуальной. Индивидуальная подразумевает
 персональные неисправности объектов, такие как выход измерения за границы шкалы.
 Диагностикой занимается программа непосредственно работающая с "железом"
*/




/*Диагностика модуля*/

// Полная недостоверность

#define OTD_DIAG_GOOD                  0
#define OTD_DIAG_NODATA                0x80000000 /*Нет данных*/
#define OTD_DIAG_PUCONNECT             0x40000000 /*Нет связи с пу*/
#define OTD_DIAG_PURESPOND             0x20000000 /*ПУ не отвечает*/
#define OTD_DIAG_PURESERV1             0x10000000
#define OTD_DIAG_CPCONNECT             0x08000000 /*Нет связи с КП*/
#define OTD_DIAG_CPRESPOND             0x04000000 /*КП не отвечает*/
#define OTD_DIAG_MODRESPOND            0x02000000 /*Модуль не отвечает*/
#define OTD_DIAG_MODTXCHANGES          0x01000000 /*Модуль не выходит на передачу по изменениям*/

#define OTD_DIAG_PUMASK                (OTD_DIAG_PUCONNECT|OTD_DIAG_PURESPOND|OTD_DIAG_PURESERV1)
#define OTD_DIAG_CPMASK                (OTD_DIAG_PUMASK|OTD_DIAG_CPCONNECT|OTD_DIAG_CPRESPOND)

#define OTD_DIAG_RESERV2               0x00800000
#define OTD_DIAG_RESERV3               0x00400000
#define OTD_DIAG_RESERV4               0x00200000
#define OTD_DIAG_RESERV5               0x00100000
#define OTD_DIAG_RESERV6               0x00080000
#define OTD_DIAG_RESERV7               0x00040000
#define OTD_DIAG_RESERV8               0x00020000
#define OTD_DIAG_REMOVED               0x00010000 /*Удалён из базы*/  

#define OTD_DIAG_MASK                  0xFFFF0000 /*Маска полной недостоверности */

/*Частичная недостоверность*/
#define OTD_PART_DIAG_PARAM            0x00008000 /*Один из параметров недостоверен*/
#define OTD_PART_DIAG_RESERV1          0x00004000
#define OTD_PART_DIAG_RESERV2          0x00002000
#define OTD_PART_DIAG_RESERV3          0x00001000

#define OTD_DIAG_PART_MASK             0x0000F000 /*Маска частичной недостоверности*/
#define OTD_DIAG_FULL_MASK            (OTD_DIAG_MASK|OTD_DIAG_PART_MASK) /*Маска диагностики*/


#define OTD_DIAG_STATE_NOTRECEIPT      0x00000800 /*Модуль не квитируется*/
#define OTD_DIAG_STATE_TUTR_ERROR      0x00000400 /*Ошибка операции ТУ/ТР*/
#define OTD_DIAG_STATE_TUTR_DESCRIPT   0x00000200 //Отсутствует описатель ТУ/ТР*/
#define OTD_DIAG_STATE_RESERV2         0x00000100


#define OTD_DIAG_STATE_MASK            0x00000F00  // Маска состояний параметров
#define OTD_DIAG_STATE_TUTR            0x00000700

/*Состояния параметров*/
#define OTD_STATE_RESERV1              0x00000080
#define OTD_STATE_RESERV2              0x00000040
#define OTD_STATE_RESERV3              0x00000020
#define OTD_STATE_RESERV4              0x00000010
#define OTD_STATE_RESERV5              0x00000008
/*Отправлена команда ОТКЛ меньше*/
#define OTD_STATE_TUTR_OFF_LESS        0x00000004
/*Отправлена команда ВКЛ больше*/
#define OTD_STATE_TUTR_ON_MORE         0x00000002
/*ТУ ТР Активно*/
#define OTD_STATE_TUTR_ACTIVE          (OTD_STATE_TUTR_ON_MORE|OTD_STATE_TUTR_OFF_LESS)

 /*Подготовка операции  ТУ/ТР*/
#define OTD_STATE_TUTR_PREPARE         0x00000001

#define OTD_STATE_TUTR_MASK           (OTD_STATE_TUTR_ON_MORE|OTD_STATE_TUTR_OFF_LESS|OTD_STATE_TUTR_CANCEL|OTD_STATE_TUTR_PREPARE)

#define OTD_STATE_MASK                 0x000000FF


/*Диагностика параметров*/

#define OTD_PDIAG_NODATA               0x80 /*Нет данных*/
#define OTD_PDIAG_ERROR                0x40 /*Параметр недостоверен возможно обрыв цепей*/
#define OTD_PDIAG_TINKLING             0x20 /*Дребезг контактов*/
#define OTD_PDIAG_OUTSCALE             OTD_PDIAG_TINKLING /*Выход за границы шкалы*/

#define OTD_PDIAG_TUTR_DESCRIPT        0x10
#define OTD_PDIAG_TUTR_FAIL            0x08    /*Неуспешное ТУ/ТР */

//#define OTD_PDIAG_TUTR_ERROR

/*Состояние   параметров*/

/*Отправлена команда ОТКЛ меньше*/
#define OTD_PSTATE_TUTR_OFF_LESS        OTD_STATE_TUTR_OFF_LESS
/*Отправлена команда ВКЛ больше*/
#define OTD_PSTATE_TUTR_ON_MORE         OTD_STATE_TUTR_ON_MORE

#define OTD_PSTATE_TR_SET               (OTD_PSTATE_TUTR_ON_MORE|OTD_PSTATE_TUTR_OFF_LESS)

 /*Подготовка операции  ТУ/ТР*/
#define OTD_PSTATE_TUTR_PREPARE         OTD_STATE_TUTR_PREPARE


/*ТУ ТР Активно* (OTD_PSTATE_TUTR_ON_MORE|OTD_PSTATE_TUTR_OFF_LESS)*/
#define OTD_PSTATE_TUTR_ACTIVE          OTD_STATE_TUTR_ACTIVE

#define OTD_PSTATE_TUTR_MASK            (OTD_PSTATE_TUTR_ACTIVE|OTD_PSTATE_TUTR_PREPARE) /*Маска персонального состояния*/
#define OTD_PDIAG_MASK                (~(OTD_PSTATE_TUTR_MASK|OTD_PDIAG_TUTR_FAIL|OTD_PDIAG_TUTR_DESCRIPT)) /*Маска персональной диагностики*/

#pragma pack(push,1)
typedef union _otd_diag
{
  DWORD diag;
  struct{WORD wpdstate,wdiag;};
  struct{
         BYTE    ds_tutr_prepare :1;//OTD_DIAG_STATE_TUTR_PREPARE
         BYTE    ds_tutr_on_more :1;//OTD_DIAG_STATE_TUTR_ACTIVE
         BYTE    ds_tutr_off_less:1;//OTD_DIAG_STATE_TUTR_ACTIVE
         BYTE    ds_reserv1_5    :4;//OTD_DIAG_STATE_RESERV1-OTD_DIAG_STATE_RESERV5
         BYTE    ds_call_sended  :1;//OTD_DIAG_STATE_CALL_SENDED

         BYTE    ps_reserv1_2   :2;//OTD_STATE_RESERV1
         BYTE    ps_tutr_error  :1;//OTD_STATE_TUTR_ERROR
         BYTE    ps_notreceipt  :1;//OTD_STATE_NOTRECEIPT

         BYTE    pd_reserv1_3   :3;//OTD_PDIAG_RESERV1-OTD_PDIAG_RESERV3
         BYTE    part_param     :1;//OTD_DIAG_PART_PARAM

         BYTE    reserv2_9      :8;//OTD_DIAG_RESERV2 - OTD_DIAG_RESERV9
         BYTE    mod_txchanges  :1;//OTD_DIAG_MODTXCHANGES
         BYTE    mod_respond    :1;//OTD_DIAG_MODRESPOND
         BYTE    cp_respond     :1;//OTD_DIAG_CPRESPOND
         BYTE    cp_connect     :1;//OTD_DIAG_CPCONNECT
         BYTE    reserv1        :1;//OTD_DIAG_RESERV1
         BYTE    pu_respond     :1;//OTD_DIAG_PURESPOND
         BYTE    pu_connect     :1;//OTD_DIAG_PUCONNECT
         BYTE    no_data        :1;//OTD_DIAG_NODATA
        };
} otd_diag, *lpotd_diag;


#pragma pack(pop)



#endif



