#pragma hdrstop
#include "cpc152dev.hpp"

 analog_ext_t::analog_ext_t (int _dev_num,int _grp_num,DWORD _dft_enabled)
 {
  bzero(this,sizeof(*this));
  dev_num = _dev_num;
  grp_num = _grp_num;
  this->dft_enabled = _dft_enabled;
  locker = new TFASTMutex;
  ext_result.op.dw_size = sizeof(ext_result.op);
 }
 analog_ext_t::~analog_ext_t()
 {
   free();
   if(locker)
      delete locker;
 }
 void __fastcall analog_ext_t::free()
 {
  alloc_params(0);
 }

 void __fastcall analog_ext_t::alloc_params(int _scan_count)
 {
  TLockHelper l(*locker);
  LPCPC152_ANALOG_PARAM * beg = params;
  LPCPC152_ANALOG_PARAM * end = beg+CPC152_MAX_PARAM_ANALOG;
  while(beg<end)
  {
	if(*beg)
	{
	 if(_scan_count>0)
		(*beg)->alloc(_scan_count);
	 else
	 {
	  delete *beg;
	  *beg = NULL;
	 }
	}
	else
	{
	 if(_scan_count)
	  *beg = new CPC152_ANALOG_PARAM(_scan_count);
	}
	++beg;
  }
 }



 void __fastcall analog_ext_t::init          (int _scan_count)
 {
        TLockHelper l(*locker);
	if(scan_count != _scan_count)
	{
           scan_capacity = _scan_count * SCANINPUT_MULTYPLIER;
           if(scan_count<_scan_count)
	     alloc_params(scan_capacity);
           recalc_done =  scan_to_recalc =  scan_counter = 0;
           scan_count  = _scan_count;
	}

  }


 void __fastcall analog_ext_t::add_param_scan(int param_num,__int16 val)
 {
   if(param_num< CPC152_MAX_PARAM_ANALOG)
   {
	LPCPC152_ANALOG_PARAM ap = params[param_num];
        if(!scan_counter)
           {
            ap->RMS = .0;
            ap->val_max = -65535;ap->val_min =  65535;
           }

        if(is_dft_enable(param_num))
        {
         double  & dval = *(ap->input+scan_counter);
         dval = val;
        }
        double dval = val;
        ap->RMS+=dval*dval;
        ap->val_max       = std::max(ap->val_max,dval);
        ap->val_min       = std::min(ap->val_min,dval);
        ap->val_effective = ap->val_effective+.0;
   }
 }


 double __fastcall cpc152dev_line::correct_analog_value(double otd_value)
 {
  double result = otd_value;
  int  must_kv = line_config.corr_must_kvants ,max_kv = line_config.corr_max_kvants;
  double tmp = must_kv - max_kv;
  if(int(tmp) && must_kv &&  max_kv)
  {
   //коррекция
   tmp/=double(max_kv);
   result += tmp*double(otd_value);

   if(line_config.corr_zero )
   {
    if(otd_value < 2) otd_value = 1.0;
    tmp = line_config.corr_zero;
    result -= tmp/otd_value;
   }
  }
  return result;
 }

 double __fastcall round_to(double x,int n)
 {
   double temp = pow(10.0,n);
   temp = floor(x*temp+0.5)/temp;
   return temp;
 }

 void  __fastcall cpc152dev_line::recalc_analog_params (lpanalog_ext_t aext,sotd_proto & sop,DWORD &ch_base,DWORD &ch_ext)
 {
        wchar_t rep_text[512];
	LPCPC152_ANALOG_PARAM * ptr_ap = aext->params;
        aext->scan_time_param = aext->avg_scan_time_param;
	for(int pn = 0,cpn = 0;pn < CPC152_MAX_PARAM_ANALOG;pn++)
	{
	  LPCPC152_ANALOG_PARAM ap = *ptr_ap;
          memcpy(ap->dft_input,ap->input,aext->scan_counter*sizeof(*ap->dft_input));
          ap->RMS = sqrt(ap->RMS/double(aext->scan_counter));
          ap->val_ampl      = (ap->val_max - ap->val_min)/2.0;
          ap->val_effective = ap->val_ampl/ M_SQRT2;

          double  otd_value = ap->RMS;
          int cv = line_config.calc_varint;
          switch(cv)
          {
           case 1:
             otd_value = ap->val_effective;
           break;
           case 2:
             otd_value = ap->val_ampl;
           break;
           case 3:
             otd_value = ap->val_max;
           break;
           case 4:
             otd_value = ap->val_min;
           break;
          }


          //for(int i = 0;i<1024;i++)
              //correct_analog_value(i);
          if(line_config.corr_enable && cv<2)
            otd_value = correct_analog_value(otd_value);

          short _otd_value = otd_value;
	  if(otd_proto_setvalue(&sop(),pn,&_otd_value,sizeof(otd_value),0,TRUE))
		 ch_base|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK;

          cpn = pn*3;
          if(aext->recalc_done)
          {
              float float_value = round_to(ap->freq,2);
              if(otd_proto_setvalue(&aext->ext_result(),cpn++,&float_value,sizeof(float_value),0,TRUE))
                     ch_ext|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK;

              float_value = ap->phase;
              if(otd_proto_setvalue(&aext->ext_result(),cpn++,&float_value,sizeof(float_value),0,TRUE))
                     ch_ext|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK;

              float_value = ap->offset;
              if(otd_proto_setvalue(&aext->ext_result(),cpn++,&float_value,sizeof(float_value),0,TRUE))
                     ch_ext|=OTD_PROTO_PART_DATA|OTD_PROTO_PART_PERSONAL_CHMASK;
          }
	  ++ptr_ap;
	}

        aext->scan_to_recalc = aext->scan_counter;
        aext->scan_counter   = 0;
        if(aext->recalc_done)
           aext->recalc_done = 0;
  }

void    __fastcall cpc152dev_line::__exec_dft(LPCPC152_ANALOG_PARAM ap,int scan_to_recalc,double phase_correct,bool make_dft)
{
  double freq =.0,phase =.0 ,offset =.0;
  if(make_dft)
   {
    wchar_t rep_text[512];

  __try{

           try{

               dft.do_dft(ap->dft_input,scan_to_recalc,&freq,&phase,&offset);
                ap->phase-=phase_correct;
                ap->exception_rised =0 ;

              }
              catch(...)
              {
               if(!ap->exception_rised&CPC152_CPP_EXCEPT)
                  {
                    snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d  c++ исключение при вычислении фазы и частоты",line_config.cp_number);
                    this->do_report(REPORT_ERROR_TYPE,rep_text);
                  }
                  ap->exception_rised|CPC152_CPP_EXCEPT;
              }
       }
      __except(EXCEPTION_CONTINUE_SEARCH)
       {
               if(!ap->exception_rised&CPC152_SEH_EXCEPT)
                  {
                    snwprintf(rep_text,KERTL_ARRAY_COUNT(rep_text),L"КП %d SEH исключение при вычислении фазы и частоты",line_config.cp_number);
                    this->do_report(REPORT_ERROR_TYPE,rep_text);
                  }
                  ap->exception_rised|CPC152_SEH_EXCEPT;
       }
   }
     ap->freq = freq; ap->phase  = phase;ap->offset = offset;
}


double __fastcall analog_ext_t::get_phase_correction(double freq)
{
  double ret = .0;
  if(this->enable_phase_correct && this->scan_time_param)
  {
   ret = (freq*M_PI*(double)2.0);
   ret/=1000000;
   ret*=double(scan_time_param);
  }
  return ret;
}

int   cpc152dev_line::analog_params_calc_thread_proc()
{
  HANDLE thread_ =   analog_params_calc_thread->native_handle();
  DWORD  icp_key = 0;
  LPOVERLAPPED    ovrlp;
  DWORD sz = 0;
  while(icp.GetStatus(icp_key,ovrlp,INFINITE,&sz))
  {
    lpanalog_ext_t aext = (lpanalog_ext_t)icp_key;
    if(aext)
    {
      TLockHelper l(aext->get_locker());
      if(aext->scan_to_recalc)
      {
           double phase_correct = aext->get_phase_correction(dft.get_freq());
           double curr_phase_correct = 0;
           LPCPC152_ANALOG_PARAM * ptr_ap = aext->params;
           LPCPC152_ANALOG_PARAM * end_ap = ptr_ap + aext->param_count;
           DWORD param_num = 0;
              while(ptr_ap<end_ap)
                {
                   __exec_dft (*ptr_ap,aext->scan_to_recalc,curr_phase_correct,aext->is_dft_enable(param_num));
                    curr_phase_correct+=phase_correct;
                   ++ptr_ap;
                   ++param_num;
                }
               ++aext->recalc_done ;
               aext->scan_to_recalc = 0;

      }
    }
    else
      return 1;
  }
  return 0;
}

