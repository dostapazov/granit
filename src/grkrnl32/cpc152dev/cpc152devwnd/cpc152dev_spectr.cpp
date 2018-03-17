//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "cpc152dev_spectr.h"
#include <complex>
#include "cpc152dev.h"
#include "compare_dft.h"
#include <algorithm>
#include <functional>
#include <numeric>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "gkmodule_form"
#pragma resource "*.dfm"
TCpc152SignalSpectr *Cpc152SignalSpectr;
HINSTANCE TCpc152SignalSpectr::dspl_lib = NULL;
HANDLE    TCpc152SignalSpectr::dspl_obj = NULL;

//---------------------------------------------------------------------------
__fastcall TCpc152SignalSpectr::TCpc152SignalSpectr(TComponent* Owner)
	: TGKModuleForm(Owner)
{
   Fcp_number = -1;
   for(int i = 0;i<CPC152_MAX_PARAM_ANALOG;i++)
   {
    ParamNum->Items->Add(i);
   }
   ParamNum->ItemIndex = 0;
   dft_input  = NULL;
   dft_result = NULL;
   dft_plan   = NULL;
   dft_window = NULL;
   if(!dspl_lib)
       {
        dspl_lib = (HINSTANCE)dspl_load();
        dspl_obj_create(&dspl_obj);
       }

}
//---------------------------------------------------------------------------

void     __fastcall TCpc152SignalSpectr::after_set_gkhandle()
{
    TGKModuleForm::after_set_gkhandle();
    set_notify(mod_iface(),MNF_NOTIFY_CPC152_SCAN_DATA );
}

void     __fastcall TCpc152SignalSpectr::on_module_state(GKHANDLE mod,DWORD state)
{

}

void     __fastcall TCpc152SignalSpectr::on_module_config_change(GKHANDLE mod,LPMODULE_CONFIG_DATA mcd)
{

}


LRESULT  __fastcall TCpc152SignalSpectr::on_gkhandle_notify (GKHANDLE from,LPNOTIFY_CODE nc,DWORD mask)
{
  if(mask == MNF_NOTIFY_CPC152_SCAN_DATA)
  {
     if((int)nc->notify_code == cp_number  )
      {
		 SendMessage(Handle,WM_USER_HANDLE_NOTIFY,0,(LPARAM)nc->notify_data);
      }
  }
  return GKH_RET_SUCCESS;
}



void __fastcall TCpc152SignalSpectr::FormClose(TObject *Sender, TCloseAction &Action)

{
  reset_notify(mod_iface(),-1);
  Action = caFree;
  this->alloc_dft_frame(0);

}
//---------------------------------------------------------------------------

void     __fastcall TCpc152SignalSpectr::handle_notify          (lpcpc152scan_data sd )
{
  if(!(sd->dev_num&CPC152SCAN_DATA_DISCRETE))
  {
    sotd_param_addr cur_addr (0,0);
    cur_addr.addr.cp = get_dev_num();
    cur_addr.addr.sb = get_grp_num();
    cur_addr.param_number = get_param_num();

    sotd_param_addr addr (0,0);
    addr.addr.cp = sd->dev_num&CPC152SCAN_DATA_DEVNUM_MASK;
    addr.addr.sb = sd->group_num;
    short * vptr     = (short*)sd->data;
    int tm_len = sd->tm_end - sd->tm_beg;

    for(int i = 0;i<CPC152_MAX_PARAM_ANALOG;i++)
    {
        addr.param_number = i;
        if(series_map.count(addr))
        {

          channel_data & cd   = series_map[addr];
          if(!Pause->Checked || cd.scan_count<cd.scan_size)
          {

           if(cd.put(double(*vptr)))
              {
               UnicodeString text;
               int resample = get_resample();
               cd.update_series(resample);
               if( cur_addr == addr)
               {
                text.printf(L"%.2f - %.2f",cd.RMS,cd.c_abs);
                RMS->Caption = text;
                text.printf(L"Time %d avg %d",tm_len,tm_len/32);
                TmLen->Caption =
                text;
                if(AutoDFT->Checked )
                    {
                     do_dft(cd);
                     text.printf(L"%.2f - %.2f",cd.RMS,cd.c_abs/sqrt(2.0));
                     RMS->Caption = text;
                    }
               }
               //if(!Pause->Checked)
               //{
               // cd.scan_count = 0;
               // cd.put(double(*vptr));
               //}
              }
          }
        }
      ++vptr;
    }

  }
}


void __fastcall TCpc152SignalSpectr::SpeedButton1Click(TObject *Sender)
{
    sotd_param_addr addr (0,0);
    addr.addr.cp = get_dev_num();
    addr.addr.sb = get_grp_num();
    addr.param_number = get_param_num();
    if(!series_map.count(addr))
    {
      channel_data cd(new TLineSeries(SignalChart));
      UnicodeString s;
      s.printf(L"%d-%d-%d",int(addr.addr.cp),int(addr.addr.sb),int(addr.param_number));
      cd.series->Title = s;
      SignalChart->AddSeries(cd.series);
      series_map[addr] = cd;
      channel_data & obj = series_map[addr];
      obj.alloc_buffer(get_scan_count());
      series_map_t::iterator sptr     = series_map.begin();
      series_map_t::iterator sptr_end = series_map.end();
      while(sptr!=sptr_end)
      {
        sptr->second.scan_count = 0;
        ++sptr;
      }
    }

}
//---------------------------------------------------------------------------

void __fastcall TCpc152SignalSpectr::SpeedButton2Click(TObject *Sender)
{
    sotd_param_addr addr (0,0);
    addr.addr.cp = get_dev_num();
    addr.addr.sb = get_grp_num();
    addr.param_number = get_param_num();
    if(series_map.count(addr))
    {
      TLineSeries * s = series_map[addr].series;
      series_map.erase(addr);
      SignalChart->RemoveSeries(s);
      delete s;

    }
}
//---------------------------------------------------------------------------

void __fastcall TCpc152SignalSpectr::SpeedButton3Click(TObject *Sender)
{
      series_map_t::iterator sptr     = series_map.begin();
      series_map_t::iterator sptr_end = series_map.end();
      while(sptr!=sptr_end)
      {
        TLineSeries * s = sptr->second.series;
        sptr->second.alloc_buffer(0) ;
        SignalChart->RemoveSeries(s);
        delete s;
        ++sptr;
      }
     series_map.clear();
}
//---------------------------------------------------------------------------

void __fastcall TCpc152SignalSpectr::DftExpChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
   int dft_frame_size = pow(2,(double)NewValue);
   DftFrame->Text     = dft_frame_size;
   alloc_dft_frame(dft_frame_size);
}
//---------------------------------------------------------------------------
void     __fastcall TCpc152SignalSpectr::alloc_dft_frame        (int dft_frame_size)
{
  if(dft_plan  ) {fftw_destroy_plan(dft_plan); dft_plan   = NULL;}
  if(dft_input ) {fftw_free(dft_input );       dft_input  = NULL;}
  if(dft_result) {fftw_free(dft_result);       dft_result = NULL;}
  if(dft_window) {delete [] dft_window;        dft_window = NULL;}

  if(dft_frame_size)
  {
    dft_result = (complex_t*) fftw_alloc_complex(dft_frame_size);
    dft_input  = (double *  ) fftw_alloc_real(dft_frame_size);
    dft_plan   = fftw_plan_dft_r2c_1d(dft_frame_size,dft_input,(fftw_complex*)dft_result,FFTW_ESTIMATE);

  }

}

void __fastcall TCpc152SignalSpectr::prepare_window(int sz,int dft_sz)
{
  double * ptr = this->dft_input;
  for(int i = 0;i<sz;i++,ptr++)
  {
      switch(DftWndType->ItemIndex)
      {
       case 1:
       (*ptr) *= Gausse(i,dft_sz*2,0.5);
       break;
       case 2:
       (*ptr) *= Hamming(i,dft_sz);
       break;
       case 3:
       (*ptr) *= Hann(i,dft_sz);
       break;
       case 4:
       (*ptr) *= BlackmannHarris(i,dft_sz);
       break;

      }
  }
}

struct dft_normal
{
  double nval;
  dft_normal(double _nval):nval(_nval){};
  dft_normal(const dft_normal & src):nval(src.nval){};
  dft_normal & operator = (const dft_normal & src){nval = src.nval; return * this;};
  complex_t operator ()(complex_t & val){ val/=nval; return val;}


};

void     __fastcall TCpc152SignalSpectr::do_dft(channel_data & cd)
{
  int sz = this->DftFrame->Text.ToInt();
  if(!dft_input) this->alloc_dft_frame(sz);
  bzero(dft_input,sizeof(*dft_input)*sz);
  bzero(dft_result,sizeof(*dft_result)*sz);
  if(cd.rs_values)
  memcpy(dft_input,cd.rs_values,cd.rs_sz_n *sizeof(*dft_input));
  else
  memcpy(dft_input,cd.values,cd.scan_count *sizeof(*dft_input));
  prepare_window(cd.scan_size,sz);


  fftw_execute(dft_plan);
  complex_t * beg = dft_result;
  complex_t * end = beg+sz;

  std::transform(beg,end,beg,dft_normal(cd.scan_count));
  ++beg;
  end = beg+(sz/2-2);
  complex_t zv(0);
  cd.sum  = std::accumulate(beg,end,zv);
  cd.c_abs = std::abs(cd.sum);
  cd.c_arg = std::arg(cd.sum);


  complex_t * cptr = dft_result;
  cd.offs = cptr->real();
  Const->Text = cd.offs;
  SendMessage(Handle,WM_USER_DRAW_SPECTR,0,0);
  //++cptr;
  //int count = sz/2-1;
}

void __fastcall TCpc152SignalSpectr::WndProc(TMessage & msg)
{
  if(msg.Msg == WM_USER_DRAW_SPECTR)
     do_draw_spectr();
	 else
	 {
	  if(msg.Msg == WM_USER_HANDLE_NOTIFY)
  		  handle_notify((lpcpc152scan_data)msg.LParam);
       else
	  TGKModuleForm::WndProc(msg);
	 }
}

void __fastcall TCpc152SignalSpectr::do_draw_spectr()
{
  int sz = this->DftFrame->Text.ToInt();
  double dF = _wtof(SampleFreq->Text.c_str());
  int resample = this->get_resample();
  if(resample>1) dF*=(double)resample;
  dF/=double(sz);
  complex_t * cptr = dft_result;
  int idx_beg  = _wtof(FrqBeg->Text.c_str())/dF;
  int idx_end  = _wtof(FrqEnd->Text.c_str())/dF;
  if(idx_end<=idx_beg)  idx_end = idx_beg+10;


  std::vector<int> idxv;
  idxv.resize(idx_end-idx_beg);
  std::generate(idxv.begin(),idxv.end(),filler<int>(idx_beg));
  std::vector<int>::iterator i_beg =  idxv.begin(),i_end = idxv.end();
  std::sort(i_beg,i_end,complex_comparer_magn_more(dft_result));

  int freq_index = *i_beg;
  i_beg++;
  double frq = dF*(double)freq_index;
  complex_t cv;
  cv = dft_result[freq_index];
  if(cv.real()> .001)
  {
  double mmag  = abs(cv);
  double phase = (mmag>1.0) ?  arg(cv) : 0;
  int cnt = 1;
  while(i_beg < i_end  )
  {
	freq_index = *i_beg;
	cv = dft_result[freq_index];
	double mag = abs(cv);
	double percent = 100.0*mag/mmag;
	if(percent>99.9)
	{
	 frq = (frq+dF*(double)freq_index)/2.0;
     if(mmag>1.0)
	 phase = (phase + arg(cv))/2.0;
	 ++cnt;
	}
	else
    break;
	++i_beg;
  }

  UnicodeString s;
  PhaseSeries->Clear();
  PhaseSeries->AddXY(0,0);
  PhaseSeries->AddXY(sin(phase),cos(phase));
  double gr = phase*(180.0/M_PI);
  s.printf(L"Ph= %.2f grad ",gr);
  PhaseChart->Title->Text->Text = s;



  s.printf(L"F= %.2f HZ iter %d",frq,cnt);
  SpectrChart->Title->Text->Text = s;

  cptr+=idx_beg;
  frq   = dF*(double)idx_beg;

  SpectrSeries->Clear() ;
  for(int i = idx_beg;i<idx_end;i++)
  {
	SpectrSeries->AddXY(frq,abs(*cptr));
	++cptr;
	frq+=dF;
  }
 }
}


void __fastcall TCpc152SignalSpectr::SpeedButton4Click(TObject *Sender)
{
    sotd_param_addr addr (0,0);
    addr.addr.cp = get_dev_num();
    addr.addr.sb = get_grp_num();
    addr.param_number = get_param_num();
    if(series_map.count(addr))
    {
      channel_data & cd = series_map[addr];
      do_dft(cd);
    }
}
//---------------------------------------------------------------------------
  double TCpc152SignalSpectr::Gausse(double n, double frameSize,double Q)
        {
            double a = (frameSize - 1)/2;
            double t = (n - a)/(Q*a);
            t*=t;
            t = exp(-t/2.0);

            return t ;
        }

 double TCpc152SignalSpectr::Hamming(double n, double frameSize)
    {
        return 0.54 - 0.46*cos((2.0*M_PI*n)/double(frameSize - 1));
    }

    double TCpc152SignalSpectr::Hann(double n, double frameSize)
    {
        return 0.5*(1 - cos((2.0*M_PI*n)/double(frameSize - 1)));
    }

    double TCpc152SignalSpectr::BlackmannHarris(double n, double frameSize)
    {
        frameSize-=1.0;
        double pi2n = M_PI*2.0*n;
        return 0.35875 - (0.48829*cos((pi2n)/(frameSize))) +
               (0.14128*cos((2*pi2n)/(frameSize ))) - (0.01168*cos((2*pi2n)/(frameSize )));
    }

void __fastcall TCpc152SignalSpectr::scan_countChange(TObject *Sender)
{
      series_map_t::iterator sptr     = series_map.begin();
      series_map_t::iterator sptr_end = series_map.end();
      int scan_size = abs(_wtoi(scan_count->Text.c_str()));
      while(scan_size > 10 && sptr!=sptr_end)
      {
        channel_data & cd = sptr->second;
        cd.alloc_buffer(scan_size);
        ++sptr;
      }
}
//---------------------------------------------------------------------------
/*
q:=points;

for i:=0 to points-1 do
  if a[i]*b[i]>0
   then dec(q);

Phase:=q/points*180;
*/
double  __fastcall test_phase(const channel_data & cd1,const channel_data & cd2)
{
   double ph1 = std::arg(cd1.sum);
   double ph2 = std::arg(cd2.sum);
   return 180.0*(ph1-ph2);

}


  void __fastcall channel_data::update_series(int resample )
  {
       double * ptr = values;
       series->Clear();
       tempRMS/=double(scan_count);
       RMS = sqrt(tempRMS);
       if(!resample)
       {
       for(int i = 0;i<scan_count;i++,ptr++)
          series->AddXY(i,*ptr);
        if(rs_values) delete [] rs_values;
           rs_values = NULL;
           rs_sz_n = resample_size = 0;
       }
       else
       {
         int N = scan_count*resample;
         if(N != this->resample_size)
         {
           if(this->rs_values) delete [] rs_values;
           this->resample_size = N;
           this->rs_values = new double[resample_size];

         }
         rs_sz_n = this->resample_size;
         dspl_resample_lagrange(values,scan_count,resample,1,0,&rs_values,&rs_sz_n);
         ptr = rs_values;
         for(int i = 0;i<rs_sz_n;i++,ptr++)
            series->AddXY(i,*ptr);

       }
  }

  bool __fastcall channel_data::put(double val)
  {
    if(!scan_count)
         tempRMS = .0;
    if(scan_count<scan_size)
      {
       values[scan_count++]= val;
       last_value = val;
       tempRMS+=val*val;
      }
      else
      {
       memcpy(prev_values,values,sizeof(*prev_values)*scan_count);
       scan_count = 0;
       return put(val);
      }
      bool ret = scan_count<scan_size ? false : true;
      if(ret)
         ret &=true;
      return ret;
  }



