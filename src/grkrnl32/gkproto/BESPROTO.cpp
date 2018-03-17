

/*******************************************************************************
 *
 * Модуль    : besproto
 * Описание  : Реализация вспомогательных функций протокола BESIK
 * Автор     :Остапенко Д.В.
 * Версия    :1
 * Создан    :26.06.2002 8:51:19
 *
 ******************************************************************************/

 


#include <besproto.h>
#include <crcunit_wrapper.hpp>


#ifdef __BORLANDC__
#pragma argsused
#endif

#if _MSC_VER >=1200
#ifndef _DEBUG
#pragma comment(linker,"/opt:nowin98")
#endif
#endif


TCrcunitWrapper crc_unit;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
        if(fwdreason == DLL_PROCESS_ATTACH)
           {
            DisableThreadLibraryCalls(hinstDLL);
            crc_unit.load();
           }
        if(fwdreason == DLL_PROCESS_DETACH)
           crc_unit.unload();
        return 1;
}

extern "C" int _DllMainCRTStartup(int i1,int i2,int i3)
{
 return  DllMain((HINSTANCE)i1,(DWORD)i2,(LPVOID)i3);
}


//---------------------------------------------------------------------------

    DWORD WINAPI besproto_parser(LPVOID buffer,DWORD b_sz,LPBESPROTO_PARSED bp,DWORD bpsz)
    {
     //Проверка входных данных
     if(bp && bpsz== sizeof(BESPROTO_PARSED))
     {
       if(buffer && b_sz>= sizeof(BES_HDR)+sizeof(WORD)) //Минимальный кадр состоит из заголовка и КПК
       {
         ZeroMemory(bp,bpsz);
         LPBYTE ptr       = (LPBYTE)buffer;

         bp->bes_hdr      = (LPBES_HDR)ptr;
         bp->bes_hdr_size = sizeof(BES_HDR);
         bp->cb_sz        = sizeof(BES_HDR);

         ptr+=bp->bes_hdr_size;

         //Установка адресного блока
         DWORD ab = DWORD((bp->bes_hdr->b_descr&BDEF_ADDR_TYPE_MASK)>>5);
         switch(ab)
         {
          case 1: bp->bes_addr_1 = (LPBES_ADDR_1)ptr;
                  bp->bes_addr_size = sizeof(*bp->bes_addr_1);
                  break;
          case 2: bp->bes_addr_2 = (LPBES_ADDR_2)ptr;
                  bp->bes_addr_size = sizeof(*bp->bes_addr_2);
                  break;

         }
         bp->cb_sz+= bp->bes_addr_size;
         ptr+=bp->bes_addr_size;

         //Далее должен идти ДИФА и Индекс

         bp->hdr_kpk = LPWORD(ptr);
         bp->cb_sz  += sizeof(WORD);
         ptr+=sizeof(WORD);

         if(int(b_sz)-int(bp->cb_sz) > sizeof(WORD))
          {
            //Значит еще есть данные
            bp->data      = ptr;
            bp->data_size = b_sz-bp->cb_sz-sizeof(WORD);
            ptr          += bp->data_size;
            bp->cb_sz    += bp->data_size;
            bp->kpk       = LPWORD(ptr);
            bp->cb_sz    += sizeof(WORD);
          }
        //Окончательная проверка полученного размера
         if(b_sz>= bp->cb_sz)
            return bp->cb_sz;

       }
     }
     return 0;
    }

//******************************************************************************
//26.06.2002 11:36:16
//Формирование кадра в формате протокола
//
//
//******************************************************************************

DWORD WINAPI besproto_maker(LPVOID buffer,DWORD b_sz,LPBESPROTO_PARSED bp,DWORD bp_sz,WORD pol_del)
{
  if(buffer && b_sz && bp && bp_sz == sizeof(*bp))
  {
    //Размер выходного буффера достаточен ? 
    if(( bp->bes_hdr_size   +
         bp->bes_addr_size  +
         bp->bes_difa_size  +
         bp->bes_index_size +
         bp->data_size      +
         sizeof(WORD)       +
       ( bp->data_size ?    sizeof(WORD) : 0))<= b_sz
       )
       {

          LPBYTE ptr = LPBYTE(buffer);
          LPBES_HDR   dst_hdr =LPBES_HDR(ptr);
          memcpy(ptr,bp->bes_hdr,bp->bes_hdr_size);
          bp->cb_sz = bp->bes_hdr_size;
          ptr      += bp->bes_hdr_size;

          //Копирование адресного блока
          BYTE vab = 0;
          if(bp->bes_addr_1)
          {
           memcpy(ptr,bp->bes_addr_1,bp->bes_addr_size);
           vab = 1;
          }

          if(bp->bes_addr_2)
          {
           memcpy(ptr,bp->bes_addr_2,bp->bes_addr_size);
           vab = 2;
          }

          ptr       += bp->bes_addr_size;
          bp->cb_sz += bp->bes_addr_size;
          //Установка соответствующего Варианта адресного блока
          dst_hdr->b_descr&=~BDEF_ADDR_TYPE_MASK;
          dst_hdr->b_descr|=((vab<<5)&BDEF_ADDR_TYPE_MASK);

          // В будущем ДИФА и индекс

          //Защита заголовка
		  LPWORD wptr = LPWORD(ptr);
          if(pol_del && crc_unit.CalcKPK.is_valid())
             *wptr = (WORD)crc_unit.CalcKPK(buffer,WORD(bp->cb_sz),pol_del);
             else
             *wptr = 0;

             ptr      +=sizeof(WORD);
             bp->cb_sz+=sizeof(WORD);


          if(bp->data && bp->data_size)
          {
           memcpy(ptr,bp->data,bp->data_size);
           ptr      +=bp->data_size;
           bp->cb_sz+=bp->data_size;
		   wptr = LPWORD(ptr);
            if(pol_del && crc_unit.CalcKPK.is_valid())
               *wptr = crc_unit.CalcKPK(bp->data,WORD(bp->data_size),pol_del);
               else
               *wptr = 0;
             ptr      +=sizeof(WORD);
             bp->cb_sz+=sizeof(WORD);
          }

         return bp->cb_sz;
       }
  }
  return 0;
}

extern "C" __declspec(dllexport) HANDLE WINAPI gkmodule_entry()
{
	return NULL;
}
