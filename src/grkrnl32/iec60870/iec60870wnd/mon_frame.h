//---------------------------------------------------------------------------

#ifndef mon_frameH
#define mon_frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "gkmod_frame.h"
#include <Vcl.ExtCtrls.hpp>
#include <text_viewer.hpp>
#include <Vcl.Buttons.hpp>
#include <iec60870proto.h>


//---------------------------------------------------------------------------
class TIEC60870MonFrame : public TGKModuleFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TSpeedButton *bPause;
	TSpeedButton *bClear;
	TSpeedButton *bDetails;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TLabel *TypeLabel;
	TComboBox *TypeFilter;
	TSpeedButton *tbEnableTypeS;
	TSpeedButton *tbEnableTypeU;
	TSpeedButton *tbEnableTypeI;
	TSpeedButton *bClipbrd;
	void __fastcall Panel2Resize(TObject *Sender);
	void __fastcall bClearClick(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall tbEnableTypeIClick(TObject *Sender);
	void __fastcall bClipbrdClick(TObject *Sender);
	void __fastcall bDetailsClick(TObject *Sender);
private:	// User declarations
       typedef KeWin::TTextViewer<char> TMonitor;
       TMonitor  mon;
       int print_radix;
       int Fobj_addr_size;
       virtual void __fastcall BeforeDestruction  ();
               void __fastcall create_mon_instance();

      void __fastcall do_print_bytes(LPBYTE ptr,int len,int radix);
      void __fastcall print_common(DWORD line,LPBYTE ptr,int len,bool tx);
      void __fastcall print_common_end();
      void __fastcall print_proto_details(DWORD line,LPBYTE ptr,int len,bool tx);
      void __fastcall print_type_U       (DWORD line,lpiec60870_proto_header_t phdr,bool tx);
      void __fastcall print_type_I       (DWORD line,lpiec60870_proto_header_t phdr,bool tx);
	  void __fastcall print_type_S       (DWORD line,lpiec60870_proto_header_t phdr,bool tx);
      void __fastcall print_remote_control(DWORD asdu_type,DWORD obj_count,LPBYTE ptr,int obj_addr_size);

public:		// User declarations
	__fastcall TIEC60870MonFrame(TComponent* Owner);
   void __fastcall do_monotir(DWORD line,LPBYTE ptr,int len,bool tx);
   __property int  obj_addr_size = {read = Fobj_addr_size,write = Fobj_addr_size};
static AnsiString __fastcall get_transmit_cause_string(DWORD tc);
static AnsiString __fastcall get_asdu_type_name       (DWORD asdu_type);
};
//---------------------------------------------------------------------------
extern PACKAGE TIEC60870MonFrame *IEC60870MonFrame;
//---------------------------------------------------------------------------
      struct time2str
      {
       AnsiString operator () (lpiec60870_cp24time2a tm);
       AnsiString operator () (lpiec60870_cp56time2a tm);
      };

      struct quality2str
      {
       AnsiString operator () (lpiec60870_quality     q);
       AnsiString operator () (lpiec60870_quality_mes q);
      };

      template <typename obj_t>
	  struct meashure_short2str
	  {
		AnsiString operator ()(obj_t * obj)
		{
		 AnsiString str;
		 str.printf(" %04X ",obj->get_value());
		 return str;
		}
	  };

	  template <typename obj_t>
      struct meashure2str
      {
        AnsiString operator ()(obj_t * obj)
        {
         AnsiString str;
         str.printf(" %8.2f ",obj->get_value());
         return str;
        }
      };

      template <typename obj_t>
      struct sp_discrete2str
      {
        AnsiString operator ()(obj_t * obj)
        {
         AnsiString str;
         str.printf(" %s ",obj->get_value() ? "SW- ON ":"SW-OFF ");
         return str;
        }
      };

      template <typename obj_t>
      struct st_discrete2str
      {
        AnsiString operator ()(obj_t * obj)
        {
         AnsiString str;
         iec60870_vti  value;
         value.value = obj->get_value();
         BYTE mask = 0x40;
         for(int i = 0;i<7;i++)
            {
             str.cat_printf("%c",(value.value&mask) ? '1':'0');
             mask>>=1;
            }
            str.cat_printf("%s state",(value.transient) ? "transient":" non transient");
         return str;
        }
      };


      template <typename obj_t>
      struct dp_discrete2str
      {
        AnsiString operator ()(obj_t * obj)
        {
         AnsiString str;
                  char * fmt = "INDETERMINATE-%d ";
         switch(obj->get_value())
         {
          case dp_off : fmt = "      SW-OFF    " ;break;
          case dp_on  : fmt = "      SW- ON    " ;break;
         }
         str.printf(fmt,  obj->get_value()); 
         return str;
        }

      };



#endif
