#ifndef __CPC152DEV_INC__
#define __CPC152DEV_INC__

 #ifndef RC_INVOKED
 #include <modemmer.h>
 #include <otd.h>
 #include <fftw3.h>
 #include <cpc152proto.h>
  #pragma pack (push,1)

  #define CCFL_PUNUMBER        0x800000
  #define CCFL_PUNAME          0x400000
  #define CCFL_LINES           0x200000

  #define REGDW_CPC152_PUNUMBER L"pu_number"
  #define REGSTR_CPC152_PUNAME  L"pu_name"
  #define REGBIN_CPC152_LINES   L"lines"

 #define CPC152_CPP_EXCEPT  1
 #define CPC152_SEH_EXCEPT  1

 typedef  struct _CPC152_ANALOG_PARAM
{
  int      exception_rised;
  double   RMS;
  double   val_max;
  double   val_min;
  double   val_ampl;
  double   val_effective;
  double   offset;
  double   ampl;
  double   freq;
  double   phase;
  double * input;
  double * dft_input;
  //double * calc_input;
   #ifdef   __cplusplus
  _CPC152_ANALOG_PARAM(int cnt)
  {
   bzero(this,sizeof(*this)); alloc(cnt);
  }

  ~_CPC152_ANALOG_PARAM(){ alloc(0); }

   void  alloc(int cnt)
   {
	if(dft_input)       delete [] dft_input;
	   dft_input      = NULL;
        if(input) delete [] input;
           input = NULL;
	if(cnt)
	 {
           input     = (double*)new double[cnt];
           dft_input = (double*)new double[cnt];
         }
   }

  #endif
}CPC152_ANALOG_PARAM, *LPCPC152_ANALOG_PARAM;


#define MNF_NOTIFY_CPC152_SCAN_DATA 0x00080000

  typedef struct _CPC152DEV_CONFIG
  {
    DWORD   dw_size;
    DWORD   modem_number;
    DWORD   pu_number;
    wchar_t pu_name[128];

  }CPC152DEV_CONFIG,*LPCPC152DEV_CONFIG;

  #define MCD_SIZE ( sizeof(MODULE_CONFIG_DATA)+ sizeof(CPC152DEV_CONFIG)-sizeof(BYTE))

  typedef struct _CPC152_LINE_CONFIG
  {
    DWORD   dw_size;
    DWORD   mask;
    wchar_t ip_addr[256];
    int     ip_port;
    int     cp_number;
    wchar_t cp_name[64];
	double  freq;           //Частота сети
	double  samples_freq;   //Частота дискретизации
	int     samples_count;  //Обрабатывать кол-во сканов
	int     dft_frame_size; //Размер фрейма DFT
	int     resamping;      //увеличение частоты сканирования полиномом Лагранжа2
    double  freq_gate;
    double  dft_percent;
    int     calc_varint;       //  Вариант вычисления 0 -RMS 1 - effective 2 - amplituda 3 -max 4 - min
    BOOL    corr_enable;       //  Разрешение коррекции
    int     corr_max_kvants;   //  Квантов при максимальном входном значении
    int     corr_must_kvants;  //  Должно быть квантов
    int     corr_zero;         //  Коррекция 0
  }CPC152_LINE_CONFIG, *LPCPC152_LINE_CONFIG;

  #define CPC152_LINE_MASK_IPADDR         0x00001
  #define CPC152_LINE_MASK_IPPORT         0x00002
  #define CPC152_LINE_MASK_CP_NUMBER      0x00004
  #define CPC152_LINE_MASK_CP_NAME        0x00008
  #define CPC152_LINE_MASK_FREQ           0x00010
  #define CPC152_LINE_MASK_SAMPLES_FREQ   0x00020
  #define CPC152_LINE_MASK_TIME           0x00040
  #define CPC152LINE_MASK_FRAME_SIZE      0x00080
  #define CPC152LINE_MASK_CALC_VARIANT    0x00100
  #define CPC152LINE_MASK_CORR_ENABLE     0x00200
  #define CPC152LINE_MASK_CORR_MAXKVANTS  0x00400
  #define CPC152LINE_MASK_CORR_MUSTKVANTS 0x00800
  #define CPC152LINE_MASK_CORR_ZERO       0x01000

  #define CPC152LINE_MASK_COMMON          0x01FFF

  #define CPC152LINE_MASK_ALARM_STARTS    0x08000
  #define CPC152LINE_MASK_DFT_ENABLED     0x10000

  typedef struct CPC152_DFT_ENABLED
  {
    DWORD dw_size;
    DWORD dft_enabled[CPC152_MAX_SLOTS][CPC152_MAX_GROUPS];
    #ifdef __cplusplus
    CPC152_DFT_ENABLED():dw_size(sizeof(CPC152_DFT_ENABLED))
    {memset(dft_enabled,-1,sizeof(dft_enabled));}
    #endif
  }CPC152_DFT_ENABLED;

  typedef CPC152_DFT_ENABLED* LPCPC152_DFT_ENABLED;

  //line_number = cp_number

  #define CMD_CPC152DEV_ADD_LINE MCMD(MT_MODEM_CPC152,1000)
  //p1 = LPCPC152_LINE_CONFIG

  #define CMD_CPC152DEV_GET_LINE MCMD(MT_MODEM_CPC152,1001)
  //p1 = LPCPC152_LINE_CONFIG

  #define CMD_CPC152DEV_UPD_LINE MCMD(MT_MODEM_CPC152,1002)
  //p1 = LPCPC152_LINE_CONFIG,p2 = old_line_number;

  #define CMD_CPC152DEV_DEL_LINE MCMD(MT_MODEM_CPC152,1003)
  //p1 = line_number

 #define MNF_NOTIFY_CPC152_ALARM_START_DEL 0x00040000
 #define MNF_NOTIFY_CPC152_ALARM_START_ADD 0x00020000


 struct  _CPC152_ALARM_START
 {
	DWORD dw_size;
	WORD  dev_num;
	WORD  grp_num;
	WORD  param_num;
	WORD  value;
	WORD  external;
 #ifdef __cplusplus
	_CPC152_ALARM_START(){dw_size = sizeof(*this); dev_num = grp_num = param_num = value = external = 0;}
	_CPC152_ALARM_START(WORD _dev_num,WORD _grp_num,WORD _param_num,WORD _value, WORD _external)
	 {dw_size = sizeof(*this); dev_num = _dev_num; grp_num = _grp_num;param_num = _param_num, value = _value; external = _external;}
	 _CPC152_ALARM_START(const _CPC152_ALARM_START& src){dw_size = sizeof(*this); *this = src;}
	 _CPC152_ALARM_START & operator = (const _CPC152_ALARM_START& src)
	 {dw_size = src.dw_size;dev_num = src.dev_num; grp_num = src.grp_num;param_num = src.param_num, value = src.value; external = src.external; return *this;}
	 bool operator < (const _CPC152_ALARM_START& obj)
	 {
	   if(dev_num == obj.dev_num)
	   {
		  if(grp_num == obj.grp_num)
		   return param_num<obj.param_num ? true : false;
		  else
		   return grp_num<obj.grp_num ? true : false;
	   }
	   return dev_num<obj.dev_num ? true : false;
	 }

	 bool operator == (const _CPC152_ALARM_START& obj)
	 {
	   return (dev_num == obj.dev_num && grp_num == obj.grp_num && param_num == obj.param_num) ? true : false;
	 }

 #endif
 };

 typedef _CPC152_ALARM_START   CPC152_ALARM_START;
 typedef _CPC152_ALARM_START * LPCPC152_ALARM_START;

 #define  CMD_CPC152_LINE_ALARM_START_ENUM  MCMD(MT_MODEM_CPC152,1004)
 //LOWORD(p1) line_number, HIWORD(p1) index, p2 = LPCPC152_ALARM_START
 #define  CMD_CPC152_LINE_ALARM_START_ADD   MCMD(MT_MODEM_CPC152,1005)
 //p1 line_number p2 = LPCPC152_ALARM_START
 #define  CMD_CPC152_LINE_ALARM_START_DEL   MCMD(MT_MODEM_CPC152,1006)
 //p1 line_number p2 = LPCPC152_ALARM_START

 #define  CMD_CPC152_LINE_GET_DFT_ENABLED   MCMD(MT_MODEM_CPC152,1007)
 //p1 line_number p2 = LPCPC152_DFT_ENABLED
 #define  CMD_CPC152_LINE_SET_DFT_ENABLED   MCMD(MT_MODEM_CPC152,1008)
 //p1 line_number p2 = LPCPC152_DFT_ENABLED


  #define CMD_CPC152DEV_WRITE_OSCILOGRAMM MCMD(MT_MODEM_CPC152,3003)
  //Запись осцилограммы
  //p1 line_number p2 time_lenght in ms

  typedef struct _CPC152_ANALOG_STATS
  {
    DWORD dw_size;
    int  enum_idx;
    int  otd_grp_num;
    int  dev_num;
    int  grp_num;
    WORD  param_count;
    DWORD overtimes;
    DWORD avg_scan_time;
    DWORD avg_scan_param_time;
  } CPC152_ANALOG_STATS, *LPCPC152_ANALOG_STATS;


  #define CMD_CPC152DEV_ENUM_ANALOGS MCMD(MT_MODEM_CPC152,4003)
  #define CMD_CPC152DEV_GET_ANALOG_STATS MCMD(MT_MODEM_CPC152,4004)
  //Перечисление получение статистики аналового модуля
  //p1 номер линии p2 LPCPC152_ANALOG_STATS


  #pragma pack (pop)

  #ifdef __cplusplus
  #include <complex>
   typedef std::complex<double> complex_t;
    template <typename T>
    struct filler
    {
      T v;
      T inc;
      filler(T _v,T _inc = (T)1):v(_v),inc(_inc){};
      filler(const filler & f){*this = f;}
      filler & operator =   (const filler & f){this->v = f.v; this->inc = f->inc;return *this;}
      T operator()() { T ret = v;v+=inc;return ret;}

    } ;

    struct complex_comparer_magn_more
    {
      complex_t * cvals;
      complex_comparer_magn_more(complex_t * _cvals){cvals = _cvals;}
      complex_comparer_magn_more(const complex_comparer_magn_more & src){cvals = src.cvals;}
      complex_comparer_magn_more & operator = (const complex_comparer_magn_more & src){cvals = src.cvals; return *this;}
      bool operator () (const int & v1,const int & v2)
      {
            int mag1 ;
            int mag2 ;
            mag1 = (1000.0*abs(cvals[v1]));
            mag2 = (1000.0*abs(cvals[v2]));
            bool ret = mag1 > mag2 ? true : false;
            return ret;
      }

    };

    template <typename T>
    struct fft_normalize:public std::unary_function<T,T>
    {
     double n_value;
     T operator () (T & value)
     {
       value/= n_value;
      return value;
     }
     fft_normalize(double nv):n_value(nv){};
     fft_normalize(const fft_normalize & src){*this = src;};
     fft_normalize & operator = (const fft_normalize & src){n_value = src.n_value; return *this;}


    };

  #endif


 #endif

#endif
