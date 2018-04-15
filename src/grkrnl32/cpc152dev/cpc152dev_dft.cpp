#pragma hdrstop
#include "cpc152dev.hpp"

  void __fastcall dft_t::release()
  {
    TLockHelper l(locker);
    if(index) delete [] index;
    index     = NULL;
    index_min   = 0;
    index_count = 0;
    if(dft_plan)  fftw_destroy_plan(dft_plan);
       dft_plan = NULL;
    if(dft_input) fftw_free(dft_input);
       dft_input =  NULL;
    if(dft_result) fftw_free(dft_result);
       dft_result = NULL;
     this->dft_frame = 0;

  }

  void __fastcall dft_t::setup  (int _dft_frame,double _sample_freq,double _freq,double _gate,double _percent)
  {
    TLockHelper l(locker);
    release();

    sample_freq = _sample_freq;
    freq        = _freq;
    gate        = fabs(_gate);
    percent     = _percent;
    if(_dft_frame)
    {
     if(dft_frame != _dft_frame || !this->dft_input || !this->dft_plan || ! this->dft_result)
        {
         release();
         dft_frame = _dft_frame;
         dft_result =   (complex_t*)fftw_alloc_complex(dft_frame);
         dft_input  =   (double *  )fftw_alloc_real   (dft_frame);
         dft_plan   = fftw_plan_dft_r2c_1d(dft_frame,dft_input,(fftw_complex*)dft_result,FFTW_ESTIMATE);
        }
        dft_limit = (dft_frame>>1)-2;
        dF          = sample_freq/(double)dft_frame;
     if(gate > .05 && gate < freq)
       {
        index_min   = (freq-gate)/dF;
        index_count = (gate*2.0) /dF;
       }
       else
       {
         index_min   = 1;
         index_count = dft_limit;
       }
     index           =   new int[ index_count ];
     bzero(dft_result,dft_frame*sizeof(*dft_result));
     bzero(dft_input ,dft_frame*sizeof(*dft_input ));
     std::generate(index,index+index_count,filler<int>(index_min));
    }
    else
    release();
  }

 double dft_t::get_phase_quadrant(complex_t* cv)
 {

   if(cv->real())
	  {
		 if(cv->imag()<0)
		   return -M_PI;
		   else
		   return M_PI;
	   }
	return .0;

 }

  int __fastcall dft_t::do_dft (double * input,int input_size,double * res_freq,double * res_phase,double *offset)
  {

    int count = 0;
    if(input && input_size && input_size < dft_frame)
    {
      TLockHelper l(locker);
      memcpy(dft_input,input,input_size*sizeof(*dft_input));
      memset (dft_input+input_size,0,(dft_frame-input_size)*sizeof(*dft_input));
      fftw_execute(dft_plan);
      std::transform(dft_result,dft_result+dft_limit,dft_result,fft_normalize<complex_t>(input_size));
      complex_t cv;
      if(offset)
      {
       cv  = *dft_result;
       cv /= double(input_size) ;
       *offset = cv.real();
      }
      int * idx_ptr = index,*idx_end = index+index_count;
      std::sort(idx_ptr,idx_end,complex_comparer_magn_more(dft_result));
      double _freq  = .0;
      double _phase = .0;
      cv     = dft_result[*idx_ptr];
      double _magn;

      if(fabs(cv.real())>.1)
      {
           _freq  = (*idx_ptr)*dF;

           _magn   = abs(cv);
           _phase  = arg(cv);
           _phase += get_phase_quadrant(&cv);

          while(idx_ptr<idx_end )
          {
             ++idx_ptr;
             cv     = dft_result[*idx_ptr];
             double cur_mag = abs(cv);
             double cperc = 100.0 * cur_mag/_magn;
             if(cperc>this->percent)
             {
               _freq  += (*idx_ptr)*dF;
               _phase += arg(cv)+get_phase_quadrant(&cv);
               ++count;
             }
             else
              break;
          }
           if(count++)
             {
               _freq  /=double(count);
               _phase /=double(count);
             }
           double  pi = M_PI;
            while(fabs(_phase) > pi)
              {
               if(_phase > pi)
                _phase-=pi;
                if(_phase<-pi)
                _phase +=pi;
              }
      }
     if(res_freq ) *res_freq  = _freq;
     if(res_phase) *res_phase = _phase;
    }
   return count;
  }


