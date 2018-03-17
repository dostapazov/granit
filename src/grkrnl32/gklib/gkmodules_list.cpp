/*******************************************************************************
 *
 * Модуль    : gkmodules_list
 * Описание  : Функции добавления / удаления из списка
 * Автор     : Остапенко Д.В.
 * Версия    : 1
 * Создан    : 04.07.2002
 *
 ******************************************************************************/

 #pragma hdrstop
 #include <gkmodules.hpp>
 #include <algorithm>

 struct gkmodule_interface_less
 {
   bool operator()(  TGKModuleInterface * x1, TGKModuleInterface * x2)
    {
       int c_res = x1->compare(*x2);
       if(!c_res)
       return gkhandle_compare(((TGKModuleInterface &)*x1)(),((TGKModuleInterface&)*x2)()) <0 ? true:false;
       return c_res <0 ? true:false;
    }

 };

void                __fastcall  TGKModulesList::resort()
{
 lock();
 sort(modules.begin(),modules.end(),gkmodule_interface_less());
 unlock();
}

bool                __fastcall TGKModulesList::add   (TGKModuleInterface * i)
{
  bool ret(false);
  lock();
   std::vector<TGKModuleInterface*>::iterator insert_point = lower_bound(modules.begin(),modules.end(),i,gkmodule_interface_less());
   if( insert_point == modules.end() || i != *insert_point)
      {
       ret = true;
       modules.insert(insert_point,i);
      }
  unlock();
  return ret;

}

 int   __fastcall TGKModulesList::get_interface_index(GKHANDLE h)
 {
   int ret = -1;
   lock();
   TGKModuleInterface f(h,true);
   std::vector<TGKModuleInterface*>::iterator begin = modules.begin(),point = lower_bound(begin,modules.end(),&f,gkmodule_interface_less());
   if( point<modules.end() && is_same_gkhandle((**point)(),h))
      ret = distance(begin,point);
   unlock();
   return ret;
 }


