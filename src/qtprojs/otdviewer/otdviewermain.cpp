#include "otdviewermain.h"
#include <QMessageBox>
#include "ui_otdviewermain.h"
#include <time.h>
#include <sys/time.h>


#ifndef MAX_PATH
#define MAX_PATH 512
#endif



int   str2addr(const char * str,LPBYTE addr,DWORD addr_sz,int delim)
{
 int ret = 0;
 if(str && addr && addr_sz)
 {
   memset(addr,-1,addr_sz);
     while(str && *str && addr_sz-- )
     {
       *addr = (BYTE)atoi(str);
       str = strchr(str,delim);
       if(str)str++;
       addr++;ret++;
     }
 }
 return ret;
}

int   str2addr(const wchar_t * str,LPBYTE addr,DWORD addr_sz,int delim)
{
 int ret = 0;
 if(str && addr && addr_sz)
 {
   memset(addr,-1,addr_sz);
     while(str && *str && addr_sz-- )
     {
       #ifdef Q_OS_WIN32
        *addr  = _wtoi(str);
        #else
         *addr = (BYTE)boost::lexical_cast<int>(str);
        #endif

       str = wcschr(str,delim);
       if(str)str++;
       addr++;ret++;
     }
 }
 return ret;
}

OtdViewerMain::OtdViewerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OtdViewerMain)
{
    ui->setupUi(this);
    model             = new QStringListModel(this);

    ui->ArchList->setModel(model);
    ui->ArchList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    cpc152_arch_model = new QStringListModel(this);

    ui->cpc152_alarm_list->setModel(cpc152_arch_model);
    //ui->cpc152_alarm_list->setSelectionMode(QAbstractItemView::ExtendedSelection);

    current_fa = FA_DEVICE_RAW_CPC152;

    connect(&socket ,SIGNAL(connected())    ,this, SLOT(on_socket_connected()));
    connect(&socket ,SIGNAL(disconnected()) ,this, SLOT(on_socket_disconnected()));
    connect(&socket ,SIGNAL(readyRead())    ,this, SLOT(on_socket_read()));
    connect(&socket ,SIGNAL(bytesWritten(qint64)) ,this, SLOT(on_socket_write(qint64)));
    socket_connected = false;
    timer_id = 0;

    status_text = new QLabel(this);
    statusBar()->addWidget(status_text,1);
    status_text->setFrameShape(QFrame::NoFrame);

    char text[1024];
    snprintf(text,sizeof(text)
             ,"BYTE  - %d "
              "WORD  - %d "
              "DWORD - %d "
              "QWORD - %d "
              "unsigned int - %d "
              "wchar_t - %d "
              "otd_alarm_param - %d "

             ,(int)sizeof(BYTE)
             ,(int)sizeof(WORD)
             ,(int)sizeof(DWORD)
             ,(int)sizeof(QWORD)
             ,(int)sizeof(unsigned int)
             ,(int)sizeof(wchar_t)
             ,(int)sizeof(otd_alarm_param)

             );
    this->print_string(text);


}

OtdViewerMain::~OtdViewerMain()
{
    statusBar()->removeWidget(status_text);
    delete status_text;
    disconnect(&socket,SIGNAL(disconnected()),this,SLOT(on_socket_disconnected()));
    disconnect(&socket,SIGNAL(connected()),this,SLOT(on_socket_connected()));
    disconnect(&socket,SIGNAL(readyRead()),this,SLOT(on_socket_read()));
    delete ui;
}

void OtdViewerMain::on_act_exit_triggered()
{
  if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,"Завершение работы","Выйти из программы?",QMessageBox::Yes|QMessageBox::No).exec())
    close();
}

void OtdViewerMain::on_act_about_triggered()
{
    QMessageBox::about(this,"О программе","Программа просмотра OTD-Данных");
}

void OtdViewerMain::on_bConnect_clicked()
{
  if(!socket_connected)
  socket.connectToHost(ui->otd_server_addr->text(),ui->otd_server_port->text().toInt());
  else
   socket.disconnectFromHost();


}

void OtdViewerMain::on_socket_connected()
{
    socket_connected = true;
    ui->bConnect->setText("Отключить");
    pkt_num   = 0;
    total_bytes = total_kadrs = total_wr_bytes = 0;
    QVariant v = false;
    socket.setSocketOption(QAbstractSocket::LowDelayOption,v);
    no_tx_timer = no_rx_timer = 0;
    timer_id = startTimer(1000,Qt::VeryCoarseTimer);

}

void OtdViewerMain::on_socket_disconnected()
{
 socket_connected = false;
    ui->bConnect->setText("Подключить");
    arch_list.clear();
    model->setStringList(arch_list);
    cpc152_arch_model->setStringList(arch_list);
    killTimer(timer_id);
    timer_id = 0;
}

void OtdViewerMain::on_socket_read()
{
 qint64 bytes_avail = socket.bytesAvailable();
 if(bytes_avail)
 {
  no_rx_timer = 0;
 do{
    if(bytes_avail)
    {

     mpb.grow( bytes_avail );
     socket.read((char*)mpb.end(),mpb.get_free_size());
     mpb.reserve(bytes_avail);
     this->total_bytes+=bytes_avail;
     bytes_avail = socket.bytesAvailable();
    }
   }while(bytes_avail);

  if(mpb.sync_internal(MP_INTERNAL_DEFAULT))
      print_string("Рассинхронизация");

  while(mpb.get_mproto_len())
  {
   ++total_kadrs;
   handle_mproto((LPMPROTO_HEADER)mpb.begin());
   mpb.move_mproto();
   if(mpb.sync_internal(MP_INTERNAL_DEFAULT))
       print_string("Рассинхронизация");
  }
  update_status_bar();
  this->no_rx_timer = 0;
 }
}

qint32 OtdViewerMain::get_part_mask()
{
 quint32 ret = 0;
 if(ui->cbDiag->isChecked())
     ret |= OTD_PROTO_PART_DIAG;
 if(ui->cbData->isChecked())
     ret |= OTD_PROTO_PART_DATA;
 if(ui->cbNames->isChecked())
     ret |= OTD_PROTO_PART_NAME;
 if(ui->cbPartData->isChecked())
     ret |= OTD_PROTO_FLAG_PARTIAL_DATA;
 if(ui->cbPersonalDiag->isChecked())
     ret |= OTD_PROTO_PART_PERSONAL_DIAG;
 if(ui->cbChangesMask->isChecked())
     ret |= OTD_PROTO_PART_PERSONAL_CHMASK;
 if(ui->cbTimestamp->isChecked())
     ret |= OTD_PROTO_PART_TIMESTAMP;

 return ret;
}

void OtdViewerMain::on_cbDiag_clicked()
{
}


void OtdViewerMain::on_bCallData_clicked()
{
    send_query(true,false);
}

void OtdViewerMain::on_bOrderData_clicked()
{
  send_query(true,true);
}

void OtdViewerMain::on_bUnorderData_clicked()
{
    send_query(false,false);
}

void OtdViewerMain::send_query(bool call,bool order)
{
  char buff[1024];
  LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
  ZeroMemory(mph,sizeof(*mph));
  otd_proto op;
  ZeroMemory(&op,sizeof(op));
  op.dw_size = sizeof(op);

  quint32 parts =  (!call && !order) ? 0 : get_part_mask() | OTD_PROTO_PART_KPK;
  otd_addr addr;
  str2addr(ui->otd_addr->text().toStdString().data(),(LPBYTE)&addr,sizeof(addr),(int)'.');
  addr.fa &=OTD_FA_ALL;
  if(call)
      addr.fa |=OTD_FA_CALL;
  if(order)
      addr.fa |= OTD_FA_ORDER;
  otd_proto_format_query((LPBYTE)&mph->data,sizeof(buff)-sizeof(*mph),&addr,0,parts,&op);
  otd_proto_protect(&op,op.proto_size,OTD_DEF_POLINOM);

  mproto_init(mph,++pkt_num,FA_OTD,op.proto_size);
  mproto_protect(mph);
  socket.write((char*)mph,mproto_size(mph));
  socket.waitForBytesWritten(1000);
  socket.flush();
  on_socket_read();

}


void OtdViewerMain::print_string(const char * str)
{
  ui->monitor->insertPlainText(QString(str));
  ui->monitor->insertPlainText(QString("\n"));
  ui->monitor->ensureCursorVisible();

}

void OtdViewerMain::handle_mproto(LPMPROTO_HEADER mph )
{

  if(mph->fa == this->current_fa && !ui->b_paused->isChecked())
  {
     char buf[4096];
     #ifdef __MINGW32__
     const char * fmt_str = "Принят кадр № %lu размер данных %lu кс %02X FA=%d";
     #else
     const char * fmt_str = "Принят кадр № %u размер данных %u кс %02X FA=%d";
     #endif
      sprintf(buf,fmt_str,(DWORD)mph->pkt_num,(DWORD)mph->data_size,(DWORD)mph->header_cs,(int)mph->fa);
      print_string(buf);
      if(mph->fa == FA_OTD  )
         {
          monitor_otd("OTD data",(LPBYTE)&mph->data,mph->data_size);
         }
      if(mph->fa == FA_OTD_ALARM_ARCHIVE)
      {
       handle_otd_archive((LPBYTE)&mph->data,mph->data_size);
      }
      if(mph->fa == FA_DEVICE_RAW_CPC152)
      {
       handle_cpc152_raw(mph);
      }
   }
}



void OtdViewerMain::on_bClearMon_clicked()
{
 ui->monitor->clear();
}

void OtdViewerMain::print_otd_data(const char * prefix,otd_data & od)
{
    WORD begin = (WORD)od.numbers.loN;
    WORD end   = (WORD)od.numbers.hiN;
    char str[2048];
    int len;
    sprintf(str,"%s с %03d по %03d",prefix ? prefix:"",(int)begin,(int)end);
    print_string(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     DWORD value = 0;
     otd_get_value(&od,begin,&value,sizeof(value));
     if(od.otd_type == OTD_DISCRETE)
      len += sprintf(str+len,"%1d",value ? TRUE:FALSE);
      else
      {
       if(od.otd_type == OTD_FLOAT)
       {
        float *fv = ((float*)&value);
        len += sprintf(str+len,"%.2f ",*fv );
       }
       else
       len += sprintf(str+len,"%04X ",
                     #ifdef __MINGW32__
                      (unsigned int)
                     #endif
                      value );
      }
    }
    print_string(str);

}


void  filetime2timeval(QWORD ft,timeval &tv)
{
  ft/=10;
  ft-=11644473600000000LL;
  tv.tv_usec = ft%1000000;
  tv.tv_sec  = ft/1000000;
}


QWORD timeval2qword(timeval & tv)
{
    QWORD ret = 11644473600;
    ret+=tv.tv_sec;
    ret*= 10000000;
    ret+= tv.tv_usec*10;
    //ret-= ret%10000;
    return ret;
}

QWORD get_current_time()
{

    /*
    chrono::system_clock::time_point t = chrono::system_clock::now();
    chrono::nanoseconds  ns = t.time_since_epoch();
    return ns.count();
    */
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return timeval2qword(tv);

}


void OtdViewerMain::monitor_otd(const char * prefix,LPBYTE data, DWORD data_size)
{
 if(prefix )print_string(prefix);
 otd_proto op;
 op.dw_size = sizeof(op);
 DWORD len;
 if(otd_proto_parser(data,data_size,&op,&len))
 {
  if(!otd_proto_check(&op,0,OTD_DEF_POLINOM))
        print_string("!!!Ошибка контрольной суммы!!!");

 char str[MAX_PATH<<1];
 char str_templ1[128];
 //char str_templ2[128];
 strcpy(str_templ1,"ОТД адрес %03u.%03u.%03u.%03u длина %d");
 len = sprintf(str,str_templ1,(DWORD)op.addr->pu,(DWORD)op.addr->cp,(DWORD)(op.addr->fa),(DWORD)op.addr->sb,(int) op.proto_size);
 if(OTD_FA_ISQUERY(op.addr->fa))
 {
  char fa_str[64];
  get_fa_str(op.addr->fa,*op.ver_parts,fa_str,sizeof(fa_str));
  sprintf(str +len ,"%s маска составляющих %08X",fa_str,
          #ifdef __MINGW32__
          (unsigned int)
          #else
          (DWORD)
          #endif
          ((*op.ver_parts)&OTD_PROTO_PARTSMASK));
  print_string(str);
 }
 else
 {
 if((*op.ver_parts)&OTD_PROTO_FLAG_QUERYRESPOND)
    len+= sprintf(str+len," ** Ответ на вызов данных **");
    print_string(str);

 if(op.time_stamp)
  {
      char date_time[MAX_PATH];

      timeval tv;
      QWORD * ptr = (QWORD*)op.time_stamp;
      QWORD   ft_val = *ptr;
      filetime2timeval(ft_val,tv);
      time_t   t = tv.tv_sec;
      struct tm * tm;
      tm = localtime(&t);
      len = strftime(date_time,sizeof(date_time),"%Y-%m-%d %H:%M:%S",tm);
      sprintf(date_time+len,".%06d",(int)tv.tv_usec);
      print_string(date_time);
  }

 if(op.diag)
    {
     sprintf(str ,"Диагностика %08X ",
        #ifdef __MINGW32__
        (unsigned int)
        #endif
        *op.diag );
     print_string(str);
    }


 if(op.name)
   {
    wchar_t w_name[MAX_PATH];
    otd_text_getwstr(op.name,w_name,sizeof(w_name)/sizeof(wchar_t));
    QString s = QString::fromWCharArray(w_name);
    sprintf(str,"Имя - %s",s.toStdString().data());
    print_string(str);
   }

 if(op.data)
 {
   /*Печать данных */
   //TODO Дописать вариант для ТУ /ТР
    DWORD begin = (DWORD)op.data->numbers.loN;
    DWORD end   = (DWORD)op.data->numbers.hiN;
   if(OTD_FA_ISCTRL(op.addr->fa))
   {
    //ТУ/ТР
    sprintf(str,"%s с %03d по %03d",(op.addr->fa&OTD_FA_ALL)? "ТР" : "ТУ", (int)begin,(int)end);
    print_string(str);
    len = 0;
    for(;begin<=end;begin++)
    {
     otd_tutr value;
     otd_get_value(op.data,begin,&value,sizeof(value));
     len+=get_tu_str(value,str+len);
    }
   }
   else
   {
    print_otd_data("Данные",*op.data);
   }
  }

 if(op.personal_diag)
 {
  print_otd_data("Персональная диагностика",*op.personal_diag);
 }

 if(op.personal_chmask)
 { 
    print_otd_data("Персональная маска изменений",*op.personal_chmask);
 }

  len = 32;
  memset(str,'-',len);
  str[len] = 0;
  print_string(str);
 }
 }
}


int  OtdViewerMain::get_fa_str(DWORD fa,DWORD pp,char * buf,DWORD buf_len)
{
 *buf = 0;
 if(fa&OTD_FA_CALL)
   {
    if(fa&OTD_FA_ORDER)
      return strlen(strncpy(buf,"вызов+подписка",buf_len));
    else
      return strlen(strncpy(buf,"вызов",buf_len));

   }
    if(fa&OTD_FA_ORDER)
      {
       return strlen(strncpy(buf,pp ? "подписка" : "отмена подписки",buf_len));
      }
  return strlen(strncpy(buf,"квитанция",buf_len));
}


int  OtdViewerMain::get_tu_str(otd_tutr & tutr,char * out)
{
 int ret = 0;
 switch(tutr.command&OTD_TUTR_CMDMASK)
 {
  case OTD_TUTR_CMDSELECT  : ret = sprintf(out,"Выбор ТУ-ТР");break;
  case OTD_TUTR_CMDENABLE  : ret = sprintf(out,"Разрешение ТУ-ТР");break;
  case OTD_TUTR_CMDDESELECT: ret = sprintf(out,"Отмена выбора ТУ-ТР");break;
  default :
   {
       switch(tutr.command&OTD_TUTR_OPMASK)
       {
       case OTD_TUOP_ON       :  ret = sprintf(out,"Команда ТУ-ТР ВКЛ /+");break;
       case OTD_TUOP_OFF      :  ret = sprintf(out,"Команда ТУ-ТР ОТКЛ/-");break;
       case OTD_TUTROP_CANCEL :  ret = sprintf(out,"Команда ТУ-ТР ОТМЕНА");break;
       }
   }
   break;
 }

 return ret;
}




void OtdViewerMain::on_OtdViewerMain_iconSizeChanged(const QSize &iconSize)
{
    iconSize.isValid() ;
}

void OtdViewerMain::on_bGetArchList_clicked()
{
  //Отправка запроса на перечисление   архивов

   arch_list.clear();
   model->setStringList(arch_list);
   if(this->socket_connected)
    {
     BYTE buf[1024];

     LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
     sotd_proto sop;
     otd_arch_proto_format((LPBYTE)mph->data,sizeof(buf),-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST,0,NULL,0,NULL,&sop.op);
     mproto_init   (mph,++pkt_num,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size);
     mproto_protect(mph);
     socket.write((char*)mph,mproto_size(mph));
     socket.waitForBytesWritten(1000);
     socket.flush();
    }
}

void OtdViewerMain::handle_otd_archive(LPBYTE data, DWORD data_size)
{
 sotd_proto sop(data,data_size);
 if(otd_proto_check(&sop.op,0,OTD_DEF_POLINOM))
   {
    if(sop.op.data)
      {
       monitor_otd("ARCHIVE data",data,data_size);
      }
      else
      {
       if(sop.op.extension)
          handle_archive_command((LPOTD_ARCH_HEADER)sop.op.extension->block_data);
      }

   }
   else
    print_string("Ошибка контрольной суммы!");

}

void OtdViewerMain::handle_archive_list(LPOTD_ARCH_LIST al,bool end)
{
 if(end)
     print_string("Получение списка архивов");

    for(int i = 0;i<(int)al->count;i++)
    {
     char txt[512];
     sprintf(txt,"%llu",al->archive[i]);
     arch_list.append(txt);
     print_string(txt);
    }
    if(end)
        model->setStringList(arch_list);

}

void OtdViewerMain::handle_archive_command(LPOTD_ARCH_HEADER lah)
{
   if(OTD_ARCH_CMD_IS_RESPOND(lah->cmd) || lah->cmd == OTD_ARCH_CMD_ALARM_ARCHIEVES_CREATED)
    {
     DWORD cmd = OTD_ARCH_CMD(lah->cmd);
     LPOTD_ARCH_LIST al = (LPOTD_ARCH_LIST)lah->data;
     switch(cmd)
     {
     case OTD_ARCH_CMD_ALARM_ARCHIEVES_CREATED:
        {
         print_string("Новые аварийный архив");
         handle_archive_list(al,true);
        }
      break;
      case OTD_ARCH_CMD_ALARM_ARCHIEVES_LIST:
      {
       handle_archive_list(al,OTD_ARCH_CMD_IS_ENDRESPOND(lah->cmd) ? true:false);

      }
      break;
      case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET:
      case OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE:
      {
       handle_archive_get(al,OTD_ARCH_CMD_IS_ENDRESPOND(lah->cmd),cmd == OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE ? true:false);
      }
      break;
      case OTD_ARCH_CMD_ALARM_ARCHIEVES_ERASE:
         handle_archive_erase(al,OTD_ARCH_CMD_IS_ENDRESPOND(lah->cmd));
      break;

     }
    }
}

void OtdViewerMain::handle_archive_get (LPOTD_ARCH_LIST al,bool end,bool erase)
{
 char buf[2048];
 sprintf(buf,"Прием архивов %s" ,end ? "завершен":"");
 print_string(buf);
 int len = 0;
 for(int i = 0;i<(int)al->count;i++)
 {
  if(end && (i&1))
      len+=sprintf(buf+len," Окончание %Lu ",al->archive[i]);
  len += sprintf(buf+len,"%Lu ",al->archive[i]);
  timeval tv;
  filetime2timeval(al->archive[i],tv);
  time_t   t = tv.tv_sec;
  struct tm * tm;
  tm = localtime(&t);
  len += strftime(buf+len,sizeof(buf)-len," %Y-%m-%d %H:%M:%S ",tm);
  sprintf(buf+len,".%06d",(int)tv.tv_usec);


  if(end && erase && !(i&1))
      arch_list.removeOne(buf);
  if(end && (i&1))
  {
   print_string(buf);
   len = 0;
  }
 }

  if(erase)
   model->setStringList(arch_list);
}

void OtdViewerMain::handle_archive_erase(LPOTD_ARCH_LIST al,bool end)
{
 char buf[2048];

 sprintf(buf,"Удаление архивов %s" ,end ? "завершено":"");
 print_string(buf);
 for(int i = 0;i<(int)al->count;i++)
 {
  sprintf(buf,"%Lu",al->archive[i]);
  print_string(buf);
  arch_list.removeOne(buf);
 }
 model->setStringList(arch_list);
}


void OtdViewerMain::on_bGetArchive_clicked()
{
 //Запрос архива
 std::vector<QWORD> arch_vect;
 QItemSelectionModel * sm = ui->ArchList->selectionModel();

 // Получение списка выделенных
 foreach (const QModelIndex &i, sm->selectedIndexes())
 {
          arch_vect.push_back(i.data().toLongLong());
 }

 DWORD cmd = OTD_ARCH_CMD_ALARM_ARCHIEVES_GET_AND_ERASE;
 if(QObject::sender() == ui->bGetArchive)
      cmd =  OTD_ARCH_CMD_ALARM_ARCHIEVES_GET;

 DWORD arch_size = arch_vect.size();
 if(arch_size)
 {
 sotd_proto sop;
 BYTE   __buff[4096];
 LPBYTE buf    = __buff;
 LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
 DWORD need_sz = sizeof(__buff)-sizeof(*mph);

 std::vector<QWORD>::iterator beg = arch_vect.begin();

 DWORD sz = otd_arch_proto_format_list((LPBYTE)mph->data,need_sz,-1,-1,cmd,arch_size,beg.base(),0,&need_sz,&sop.op);
 if(!sz)
 {
  buf = new BYTE[need_sz+sizeof(*mph)];
  sz = otd_arch_proto_format_list((LPBYTE)mph->data,need_sz,-1,-1,cmd,arch_size,beg.base(),0,&need_sz,&sop.op);
 }

 otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
 mproto_init   (mph,++pkt_num,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size);
 mproto_protect(mph);
 this->socket.write((const char*)mph,MPROTO_SIZE(mph));
 if(buf!=__buff)
     delete [] buf;
 }
 else
     print_string("select archive to get");
}

void OtdViewerMain::on_tabWidget_currentChanged(int index)
{
    switch(index)
    {
     case 0: current_fa = FA_OTD;
                break;
     case 1: current_fa = FA_OTD_ALARM_ARCHIVE;
               break;
     case 3: current_fa = FA_DEVICE_RAW_CPC152;
        break;

    }
}


void OtdViewerMain::on_bEraseArchive_clicked()
{
    std::vector<QWORD> arch_vect;
    QItemSelectionModel * sm = ui->ArchList->selectionModel();

    // Получение списка выделенных
    foreach (const QModelIndex &i, sm->selectedIndexes())
    {
             arch_vect.push_back(i.data().toLongLong());
    }

    DWORD arch_size = arch_vect.size();
    if(arch_size)
    {
    sotd_proto sop;
    BYTE   __buff[4096];
    LPBYTE buf    = __buff;
    LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
    DWORD need_sz = sizeof(__buff)-sizeof(*mph);

    std::vector<QWORD>::iterator beg = arch_vect.begin();

    DWORD sz = otd_arch_proto_format_list((LPBYTE)mph->data,need_sz,-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_ERASE,arch_size,beg.base(),0,&need_sz,&sop.op);
    if(!sz)
    {
     buf = new BYTE[need_sz+sizeof(*mph)];
     sz = otd_arch_proto_format_list((LPBYTE)mph->data,need_sz,-1,-1,OTD_ARCH_CMD_ALARM_ARCHIEVES_ERASE,arch_size,beg.base(),0,&need_sz,&sop.op);
    }

    otd_proto_protect(&sop.op,0,OTD_DEF_POLINOM);
    mproto_init   (mph,++pkt_num,FA_OTD_ALARM_ARCHIVE,sop.op.proto_size);
    mproto_protect(mph);
    socket.write((const char*)mph,MPROTO_SIZE(mph));
    if(buf!=__buff)
        delete [] buf;
    }
    else
        print_string("select archive to erase");

}

void OtdViewerMain::on_bGetAndErase_clicked()
{
  on_bGetArchive_clicked();
}

void OtdViewerMain::update_status_bar()
{
    QString str = QString("Принято %1 кадров %2 байт | Отправлено %3 байт ").arg(total_kadrs).arg(total_bytes).arg(total_wr_bytes);

    status_text->setText(str);
}

void OtdViewerMain::on_b_paused_clicked()
{

}

void OtdViewerMain::send_kpa      ()
{
  MPROTO_HEADER mph;
  ZeroMemory(&mph,sizeof(mph));
  mph.internal =  MP_INTERNAL_DEFAULT;
  mph.pkt_num  = ++pkt_num;
  mproto_protect(&mph);
  socket.write((char*)&mph,mproto_size(&mph));
  socket.waitForBytesWritten(1000);
  socket.flush();
}

void OtdViewerMain::on_socket_write(qint64 wrb)
{
   total_wr_bytes += wrb;
   no_tx_timer = 0;
   update_status_bar();
}

void OtdViewerMain::timerEvent(QTimerEvent *event)
{
  if(event->timerId() == timer_id)
  {
    if(no_rx_timer++>10)
     {
       //Не получено ни одного кадра в течении 5 сек
        //on_bConnect_clicked();
       no_rx_timer = 0;
     }

  if(no_tx_timer++>3)
  {
     send_kpa();
  }
 }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

void OtdViewerMain::on_btnSendAlarmParam_clicked()
{
  //Отправка команды установки аварийной уставки
   BYTE buf[1024];
   LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buf;
   otd_proto op;
   ZeroMemory(&op,sizeof(op));
   DWORD need_sz;
   otd_addr addr;
   std::string addr_str = ui->apAddrParam->text().toStdString();

   str2addr(addr_str.c_str(),(LPBYTE)&addr,sizeof(addr),(int)'.');

   DWORD sz = otd_proto_format_alarm(mph->data,sizeof(buf)-sizeof(*mph),&need_sz,&addr,1,&op);
   lpotd_alarm_param ap = (lpotd_alarm_param)op.extension->block_data;
   ap->param_addr.addr.addr = addr.addr;

   int param_offset = addr_str.find('-');
   if(param_offset>0)
     ap->param_addr.param_number = atoi(addr_str.data()+1+param_offset);
   else
   ap->param_addr.param_number = -1;
   if(ui->acClear->isChecked())
       ap->cmd = OTD_ALARM_CMD_CLEAR;
   if(ui->acLess->isChecked())
       ap->cmd = OTD_ALARM_CMD_LESS|OTD_ALARM_CMD_SET;
   if(ui->acMore->isChecked())
       ap->cmd = OTD_ALARM_CMD_MORE|OTD_ALARM_CMD_SET;
   ap->value = atoi(ui->apValue->text().toStdString().c_str());
   mproto_init(mph,++pkt_num,FA_OTD,op.proto_size);
   mproto_protect(mph);
   socket.write((char*)mph,mproto_size(mph));
   socket.waitForBytesWritten(1000);
   socket.flush();


}

void OtdViewerMain::do_send(LPMPROTO_HEADER mph)
{
    mph->pkt_num = ++this->pkt_num;
    mproto_protect(mph);
    socket.write((char*)mph,mproto_size(mph));
    socket.waitForBytesWritten(1000);
    socket.flush();
    this->on_socket_read();

}

void OtdViewerMain::handle_cpc152_raw(LPMPROTO_HEADER mph)
{
  lpcpc152proto_hdr phdr = (lpcpc152proto_hdr) mph->data;
  bool is_respond   = (phdr->fa&CPC152PROTO_FA_FLAG_RESPOND) ? true : false;
  bool is_end       = (phdr->fa&CPC152PROTO_FA_FLAG_END    ) ? true : false;
  bool is_erase     = (phdr->fa&CPC152PROTO_FA_FLAG_ERASE  ) ? true : false;
  switch(phdr->fa&CPC152PROTO_FA_MASK)
  {
    case CPC152PROTO_FA_SCAN_DATA:
      cpc152_scan_data(mph,phdr,is_end);
      break;
    case CPC152PROTO_FA_SYNCTIME:
      cpc152_time_sync(mph,phdr);
    break;
    case CPC152PROTO_FA_GET_ALARMS_LIST:
      if(is_respond)
         cpc152_handle_alarms_list(phdr,is_end);
      break;
   case CPC152PROTO_FA_GET_ALARM_FILE:
      if(is_respond)
         cpc152_scan_data(mph,phdr,is_end);
      break;
   case CPC152PROTO_FA_NEW_ALARM:
      cpc152_new_alarm(phdr);
      break;
  }

}

void OtdViewerMain::cpc152_scan_data (LPMPROTO_HEADER mph,lpcpc152proto_hdr phdr,bool is_end)
{
  char text[1024];
  LPBYTE ptr = phdr->data;
  int cur_len = 0;
  int tot_len = 0;

  bool is_archive = (phdr->fa&CPC152PROTO_FA_MASK) == CPC152PROTO_FA_GET_ALARM_FILE ? true:false;
  if(is_archive)
  {
    lpcpc152alarm_file_content fc = (lpcpc152alarm_file_content)ptr;
    ptr = fc->data;
    sprintf(text,"Аварийный архив № %hu длиной %d количество сканов %d arch_id - %Ld %s"
                       ,mph->pkt_num
                       ,mph->data_size
                       ,phdr->count
                       ,fc->alarm_id
                       ,is_end ? "Завершение":"");

  }
  else
  {
    if(ui->cbOnlyArchShow->isChecked()) return;
     sprintf(text,"Данные сканирования № %hu длиной %d количество сканов %d",mph->pkt_num,mph->data_size,phdr->count);
  }
  print_string(text);

  for(int i = 0;i<phdr->count;i++)
  {
   lpcpc152scan_data sd = (lpcpc152scan_data)ptr;
   if(sd->dev_num & CPC152SCAN_DATA_DISCRETE )
    {
     sprintf(text,"dev_num %d grp_num %d data %02X %02X %02X mask %02X %02X %02X"
             ,unsigned(sd->dev_num&CPC152SCAN_DATA_DEVNUM_MASK)
             ,unsigned(sd->group_num)
             ,unsigned(sd->data[0])
             ,unsigned(sd->data[1])
             ,unsigned(sd->data[2])
             ,unsigned(sd->data[3])
             ,unsigned(sd->data[4])
             ,unsigned(sd->data[5])
             );
     print_string(text);
    }
   cur_len = (sd->dlen+sizeof(*sd)-sizeof(sd->data[0]));
   cur_len+=sizeof(WORD);//kpk
   tot_len +=cur_len;
   ptr+= cur_len;
  }

  sprintf(text,"mph data_size %d total_len %d",(int)mph->data_size,tot_len);
  print_string(text);

}

void OtdViewerMain::cpc152_time_sync(LPMPROTO_HEADER mph,lpcpc152proto_hdr phdr)
{
    print_string("Запрос на синхронизацию времени");
    phdr->fa |= CPC152PROTO_FA_FLAG_RESPOND|CPC152PROTO_FA_FLAG_END;
    LPQWORD pqw =  (LPQWORD)phdr->data;
    pqw [1] = get_current_time();
    do_send(mph);
}

#pragma GCC diagnostic pop

void OtdViewerMain::cpc152_send_alarm_condition(bool erase,bool erase_all)
{
    //setup alarm condition
     char buff[512];
     LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
     lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
     lptalarm_condition  ac = (lptalarm_condition )phdr->data;
     phdr->count   = 1;
     phdr->fa      = CPC152PROTO_FA_ADD_CONDITION;

     ac->dev_num   = ui->dev_num->text().toInt();
     ac->grp_num   = ui->grp_num->text().toInt();
     ac->param_num = ui->param_num->text().toInt();

     if(ui->RB_cpc152_on->isChecked())
     {
      ac->more = true;
      ac->alarm_value = 0;
     }
     if(ui->RB_cpc152_off->isChecked())
     {
      ac->more = false;
      ac->alarm_value = 1;
     }

     if(erase)
     {
       phdr->fa|=CPC152PROTO_FA_FLAG_ERASE;
       if(erase_all)
       {
           ac->dev_num = ac->grp_num = ac->param_num = USHRT_MAX;
           ac->more    = 1;
       }

     }

     mproto_init   (mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(sizeof(*ac)));
     do_send(mph);

}

void OtdViewerMain::on_bCpc152AlarmAdd_clicked()
{
  cpc152_send_alarm_condition();
}

void OtdViewerMain::on_bCpc152AlarmErase_clicked()
{
   cpc152_send_alarm_condition(true);
}

void OtdViewerMain::on_bCpc152AlarmClear_clicked()
{
    cpc152_send_alarm_condition(true,true);
}

void OtdViewerMain::on_bCpc152GetAlarmList_clicked()
{
    //Request for alarm list
    arch_list.clear();
    cpc152_arch_model->setStringList(arch_list);

    char buff[512];
    LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
    lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
    phdr->count = 0;
    phdr->fa    = CPC152PROTO_FA_GET_ALARMS_LIST;
    mproto_init   (mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(0));
    do_send(mph);
}

void OtdViewerMain::cpc152_handle_alarms_list(lpcpc152proto_hdr phdr,bool is_end)
{
 if(phdr->count)
 {
   LPQWORD qptr = (LPQWORD) phdr->data;
   LPQWORD qptr_end = qptr+phdr->count;
   while(qptr<qptr_end)
   {
       char txt[512];
       sprintf(txt,"%llu",*qptr);
       arch_list.append(txt);
       ++qptr;
   }

   if(is_end)
       cpc152_arch_model->setStringList(arch_list);
  }
}

void OtdViewerMain::cpc152_new_alarm (lpcpc152proto_hdr phdr)
{
   LPQWORD qptr = (LPQWORD)phdr->data;
   char txt[512];
   sprintf(txt,"%llu",*qptr);
   arch_list.append(QString(txt));
   cpc152_arch_model->setStringList(arch_list);
}

void OtdViewerMain::on_bCpc152GetAlarm_clicked()
{
    //Request to get alarm file
    QItemSelectionModel * sm = ui->cpc152_alarm_list->selectionModel();
    if(sm && sm->hasSelection())
    {
      QModelIndex idx  = sm->currentIndex();

      char buff[512];
      LPMPROTO_HEADER mph = (LPMPROTO_HEADER)buff;
      lpcpc152proto_hdr phdr = (lpcpc152proto_hdr)mph->data;
      LPQWORD arch_id = (LPQWORD)phdr->data;
      *arch_id        =  idx.data().toLongLong();
      phdr->count = 1;
      phdr->fa    = CPC152PROTO_FA_GET_ALARM_FILE|CPC152PROTO_FA_FLAG_END;
      if(ui->cbEraseAlarmFile->isChecked())
          phdr->fa |= CPC152PROTO_FA_FLAG_ERASE;
      mproto_init   (mph,0,FA_DEVICE_RAW_CPC152,cpc152proto_hdr_calc_size(sizeof(arch_id)));
      do_send(mph);
    }
}
