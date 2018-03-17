#ifndef _BI_VECTOR_TEMPLATE
#define _BI_VECTOR_TEMPLATE

#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

template <typename Data,typename back_comparer= less<Data> ,typename direct_comparer = less<Data> >
class bi_vector
{
 public:
 direct_comparer       dc;
 back_comparer         bc;
 typedef vector<Data>  data_vector;
 typedef vector<DWORD> index_vector;
 protected:
 data_vector    data;
 index_vector   index;

 public:
 bi_vector(){};
 bool                  reserve(DWORD sz);
 void                  resize (DWORD sz);
 void                  resize (DWORD sz,Data & fill_value);

 data_vector::iterator  begin(){return data.begin();}
 data_vector::iterator  end  (){return data.end();}
 index_vector::iterator index_begin(){return index.begin();}
 index_vector::iterator index_end  (){return index.end();}

 Data &                 at      (DWORD offset){return data.at(offset);}
 Data &                 index_at(DWORD offset){return data.at(index.at(offset));}
 data_vector::iterator  sort_insert(Data & value,back_comparer bc);
 data_vector::iterator  lower_bound(data_vector::iterator start,data_vector::iterator finish,Data & value);
 data_vector::iterator  upper_bound(data_vector::iterator start,data_vector::iterator finish,Data & value);

};

 template <typename Data,typename back_comparer,typename direct_comparer = less<Data> >
 bool                  bi_vector<Data,back_comparer,direct_comparer>::reserve(DWORD sz)
 {
  data.reserve(sz);
  index.reserve(sz);
 }

template <typename Data,typename back_comparer,typename direct_comparer = less<Data> >
void                   bi_vector<Data,back_comparer,direct_comparer>::resize (DWORD sz)
{

}

template <typename Data,typename back_comparer,typename direct_comparer = less<Data> >
void                  bi_vector<Data,back_comparer,direct_comparer>::resize (DWORD sz,Data & fill_value)
{
}

template <typename Data,typename back_comparer,typename direct_comparer = less<Data> >
bi_vector<Data,back_comparer,direct_comparer>::data_vector::iterator
     bi_vector<Data,back_comparer,direct_comparer>::sort_insert(Data & value)
{
 data_vector::iterator __ptr = data.insert(lower_bound(begin(),end(),val),val);
 DWORD __offset = std::distance(data.begin(),__ptr);
 transfrom(index.begin(),index.end(),
}

template <typename Data,typename back_comparer,typename direct_comparer = less<Data> >
bi_vector<Data,back_comparer,direct_comparer>::data_vector::iterator
   bi_vector<Data,back_comparer,direct_comparer>::lower_bound(data_vector::iterator start,data_vector::iterator finish,Data & value)
{
 return std::lower_bound(start,finish,value,dc);
}

template <typename Data,typename back_comparer,typename direct_comparer = less<Data> >
bi_vector<Data,back_comparer,direct_comparer>::data_vector::iterator bi_vector<Data,back_comparer,direct_comparer>::upper_bound(data_vector::iterator start,data_vector::iterator finish,Data & value)
{
 return std::upper_bound(start,finish,value,dc);
}

#endif


