#include "grnet232.hpp"
#include <crcunit.h>


 TGrNetClient::TGrNetClient(TGrNetServer * _owner,WORD id,TEventSimpleDataQueue * _queue):
 TGrNetBase(0,0,_queue),
 owner(_owner),
 ClientId(id),
 ClientState(stNotConnect),
 receipt_timer (false),
 tx_queue_event(false,false),
 server_socket_number(0)
 {
  SetFlags(GRNET_FLAGS_ENABLE_WATCHDOG,true);

 }

 TGrNetClient::TGrNetClient(const char * ServerName,DWORD ServerSocket,const char * name,WORD sn):
 TGrNetBase(sn,name,0),
 owner(0),
 ClientId(WORD(-1)),
 ClientState(stNotConnect),
 receipt_timer(false),
 tx_queue_event(false,false),
 server_socket_number(ServerSocket)
 {
  SetServerName(ServerName);
  SetFlags(GRNET_FLAGS_ENABLE_RECONNECT,true);
  SetFlags(GRNET_FLAGS_ENABLE_WATCHDOG,true );
 }

 void   __fastcall TGrNetClient::Terminate()
 {

  if(IsConnected())
  {
   lock();
   // Запрос на отключение
   tx_queue.DropData();
   SetClientState(stWaitReceipt,false);
   send_data(CmDisconnect,0,0,true);
   SetClientState(stWaitDisconnect,true);
   unlock();
  }
  //else
  TGrNetBase::Terminate();
 }

 void        __fastcall TGrNetClient::SetServerName(const char * s_name,int len)
 {
   lock();
   if (s_name)
   {
    if(len<1)
      len = lstrlenA(s_name);
    lstrcpynA(server_name,s_name,GRNET_NAMES_LEN);
    if(lstrlenA(server_name)==0)
      SetName(0);
   }
   else
   {
    char buffer[MAX_PATH];
    DWORD sz = sizeof(buffer);
    GetUserName(buffer,&sz);
    SetName(buffer,sz);
   }
   unlock();
 }

 DWORD       __fastcall TGrNetClient::GetServerName(char *out,DWORD sz)
 {
   DWORD ret(0);
   if(out)
   {
   lock();
   ret = lstrlenA(server_name);
   if(ret<sz)
    CopyMemory(out,server_name,ret+1);
   unlock();
   }
   return ret;
 }

 bool         __fastcall TGrNetClient::BeforeExecute  ()
 {
  bool ret(false);
  lock();
  //SetThreadPriority(tpHighest);
  ret = TGrNetBase::BeforeExecute();
  if(ret)
  {
   out_net_buffer = (TGRNetBuffer*)output_buffer;
   LastRxKadrNumber =  LastTxKadrNumber = 0;
   receipt_timer.StopTimer();
   synchro_set+=receipt_timer();
   tx_queue_event.SetEvent(false);
   tx_queue.DropData();
   synchro_set+=tx_queue_event();
   if(!owner)
      BeginConnection();
    else
	{
	  AcceptConnection(*out_net_buffer);  
	}
  }
  unlock();
  return ret;

 }

 void     __fastcall TGrNetClient::BeforeTerminate()
 {
   tx_queue.DropData();
   tx_queue_event.SetEvent(false);
   receipt_timer.StopTimer();
   out_net_buffer = 0;
   SetClientState(stNotConnect,true);
   TGrNetBase::BeforeTerminate();
 }

  void         __fastcall TGrNetClient::OnSecundTimer()
  {
    if(IsConnected())
    {
     TGrNetBase::OnSecundTimer();
     if(IsFree() && IsFlagsSet(GRNET_FLAGS_ENABLE_WATCHDOG) && (--WatchDogTimer<=0)  )
      {
       send_data(CmCheckConnect,0,0,true);
       WatchDogTimer = WatchDogTime; 
      }
    }
    else
    {
     if(!owner && IsFlagsSet(GRNET_FLAGS_ENABLE_RECONNECT) && !this->IsWaitConnect())
	 {
	     lock();
         {
		 BeforeTerminate();
		 BeforeExecute();
		 //BeginConnection();
         }
		unlock();
                
	 }
    }
  }



 void         __fastcall TGrNetClient::OnReceiptTimer ()
 {
     //Обработчик таймера ожидания квитанции
	 //resend
       SetClientState(stWaitReceipt,false);
	   if(out_net_buffer->msg.RetryNum++<WORD(RetryCount))
	   {
	     out_net_buffer->CalcCheckSum();
		 int slen = Send(host_addr,output_buffer,out_net_buffer->GetTotalLen());
         if((ClientState&stWaitDisconnect ) && ( out_net_buffer->msg.RetryNum > 3 ))
		 {
              SetClientState(stNotConnect|dcLostConnection,true);
              _dbgprn(("Потеря связи с клиентом"));
              Terminate();
 
		 }
		 else
		 {
		 if(out_net_buffer->msg.NeedReceipt || !slen)
			 SetClientState(stWaitReceipt,true);
		 }
	   }
	   else
	   {
	     //Потеря связи
           SetClientState(stNotConnect|(IsWaitConnect() ? dcServerNotFound : dcLostConnection),true);

	   }
 }

 void         __fastcall TGrNetClient::HandleReceive(TIpxAddress & from,LPVOID data,int bytes)
 {
   //Обработка принятого кадра
   TGRNetBuffer & net_buffer = *((TGRNetBuffer*)data);
   if(net_buffer.IsValid())
   {

    if(this->IsConnected())
    {
      if(from == host_addr)
      {
        WatchDogTimer = WatchDogTime;
        if(net_buffer.msg.IsReceipt())
          HandleReceipt(net_buffer);
          else
          HandleRx(net_buffer);
      }
    }
    else
    {
     if(owner == 0 && this->IsWaitConnect() &&
        net_buffer.msg.IsReceipt() &&
        net_buffer.msg.command==CmConnect
        )
     {
      //Ожидаем соединения
      TBeginConnection bc;
      net_buffer.GetData(&bc,sizeof(bc));
      if(SameName(bc.ClientName))
      {
        SetClientState(stWaitReceipt|stWaitConnect,false);
        if(bc.ErrorConnection == 0)
         {
          
          ClientId = bc.ID;
          host_addr = from;
          LastRxKadrNumber=0;
          WatchDogTimer = WatchDogTime;
          WorkTime = 0;
		  SetClientState(stNotConnect,false);
         }
         else
         {
          SetClientState(stNotConnect|dcErrorConnection,true);
         }
      }

     }
    }

   }
 }

 bool         __fastcall TGrNetClient::OnWaitEvent(DWORD WaikupNumber)
 {
   bool ret (true);
   lock();
   switch(WaikupNumber)
   {
    case  3:{
             //Таймер ожидания квитанции
             OnReceiptTimer();
            }
            break;
    case  4:
            {
             //Событие на передачу
             tx_queue_event.SetEvent(false);
             if(IsFree())
               ContinueSend() ;
            }
            break;
    default:
     ret = TGrNetBase::OnWaitEvent(WaikupNumber);
     break;
   }
   unlock();
   return ret;
 }

 void         __fastcall TGrNetClient::BeginConnection()
 {
  //Начало соединения
  lock();
  TBeginConnection bc={0};
  GetServerName(bc.ServerName,sizeof(bc.ServerName));
  CharUpperBuff(bc.ServerName,sizeof(bc.ServerName));
  GetName(bc.ClientName,sizeof(bc.ClientName));
  bc.ClientLevel =1;
  bc.ID = this->ClientId = WORD(-1);
  host_addr.SetBroadcast(server_socket_number,false);
  this->send_data(CmConnect,&bc,sizeof(bc),true);
  SetClientState(stWaitConnect,true);
  //this->MakeClientInfo(0);
  unlock();
 }


 inline bool __fastcall IsNewKadr(WORD old_num,WORD new_num)
 {
  return (old_num < new_num || (old_num == 0xFFFF && new_num<old_num)) ? true:false;
 }

 void         __fastcall TGrNetClient::HandleRx     (TGRNetBuffer & buff)
 {
   //Квитируем если требуется квитанция или повторная передача
   if(buff.msg.NeedReceipt || buff.msg.RetryNum)
     ReceiptMessage(buff);
   if(IsNewKadr(WORD(LastRxKadrNumber),buff.msg.KadrNum))
   {
    LastRxKadrNumber = buff.msg.KadrNum;
    HandleRxCommand( buff);
   }
 }

 void         __fastcall TGrNetClient::ReceiptMessage(TGRNetBuffer & buff)
 {
   DWORD SaveLen = buff.GetDataLen();
   buff.msg.ReceiptMessage();
   buff.Len = 0;
   buff.CalcCheckSum();
   this->Send(host_addr,&buff,buff.GetTotalLen());
   buff.Len = SaveLen;
 }

 void         __fastcall TGrNetClient::HandleRxCommand(TGRNetBuffer & buff)
 {
  switch(buff.msg.command)
  {
   case CmCheckConnect:break;
   case CmDisconnect  :
        SetClientState(stNotConnect|dcServerOff,true);
        break;
   default:
        this->PutDataToQueue(rx_queue,ClientId,buff.msg.command,
                                               buff.msg.KadrNum,
                                               buff.msg.RetryNum,
                                               buff.DATA,
                                               buff.GetDataLen());
        break;
  }
 }


 void         __fastcall TGrNetClient::HandleReceipt(TGRNetBuffer & buff)
 {
  if(buff.msg.KadrNum == WORD(LastTxKadrNumber))
     SetClientState(stWaitReceipt,false);
  if(buff.msg.command == CmDisconnect)
  {
   SetClientState(stNotConnect|dcSelfInitiate,true);
   Terminate();
  }
  else
  {
  if(IsFree())
  {
   ContinueSend();
  }
  }
 }

 void         __fastcall TGrNetClient::ContinueSend()
 {
  //Продолжение передачи
  //
  DWORD Command,NeedReceipt,len(0),Remind(0);
  LPVOID data =out_net_buffer->DATA;
  if(tx_queue.QueueCount())
  {
  do{
  if(int(GetDataFromQueue(&tx_queue,0,&Command,0,&NeedReceipt,data,
                      max_frame_size-(out_net_buffer->GetTotalLen()-out_net_buffer->Len),&len,&Remind))>=0)
					  send_data(Command,data,len,NeedReceipt? true:false);

    }while(IsFree() && Remind);
    if(Remind == 0)
     this->PutDataToQueue(rx_queue,ClientId,CmOutputQueueEmpty,0,0,0,0);
    } 
 }


 bool         __fastcall TGrNetClient::send_data(DWORD Command,void * data,int sz,bool nr,bool real)
 {
  //Передача данных
  if(sz<= sizeof(out_net_buffer->DATA))
  {
    lock();
    out_net_buffer->msg = *& TNetMessage(Command,ClientId,nr,++LastTxKadrNumber,0);
    out_net_buffer->SetData(data,sz);
    out_net_buffer->CalcCheckSum();
    int slen = out_net_buffer->GetTotalLen();
    if(real)
       slen=Send(host_addr,output_buffer,slen);
    if(nr || slen == 0)
     {
      SetClientState(stWaitReceipt,true);
      //Постановка на повторную передачу в случае ожидания квитанции или в случае
      // неудачи при отправке
     } 
    unlock();
    return 0;
  }
  return false;
 }

 void     __fastcall TGrNetClient::SetClientState(DWORD StateMask, bool set)
 {
   //this->lock();
   ClientState = set ? (ClientState|StateMask):(ClientState&(~StateMask));
   if(StateMask& stWaitReceipt)
   {
     if(set)
      receipt_timer.StartPeriodTimer(WaitRcptTime,true);
      else
      receipt_timer.StopTimer();
   }

   if(StateMask & stNotConnect)
   {
    if(set)
    {
     //разрыв связи
     receipt_timer.StopTimer();
     ClientState&=0xFF000000;
     ClientState|=(StateMask&0xFF);
	 //host_addr.SetBroadcast(server_socket_number);
    if(owner || !IsFlagsSet(GRNET_FLAGS_ENABLE_RECONNECT))
	{
	 Terminate();	
	}
    }
    else
    ClientState&=0xFFFFFF00;
	WorkTime = 0;
    MakeClientInfo();
   // TODO Выдать команду остановлен ?
	//this->unlock();

   }
   
 }

 bool   __fastcall TGrNetClient::SendTo(DWORD ClientId,DWORD Command, LPVOID data, DWORD length, bool NeedReceipt,bool First)
 {
   bool ret(false);
   lock();
   if(IsConnected())
   {
    ret = true;
    if(IsFree())
    {
     this->send_data(Command,data,length,NeedReceipt);
    }
    else
    {
     PutDataToQueue(&tx_queue,ClientId,Command,0,NeedReceipt,data,length);
     tx_queue_event.SetEvent(true);
    }
   }
   unlock();
   return ret;
 }

 void     __fastcall TGrNetClient::MakeClientInfo(LPCLIENT_INFO info)
 {
  CLIENT_INFO _info;
  if(!info)
     info = &_info;
   ZeroMemory(info,sizeof(*info));
   lock();
   GetName(info->ClientName,sizeof(info->ClientName));
   GetServerName(info->ServerName,sizeof(info->ServerName));
   info->ClientId = ClientId;
   memcpy(&info->Address,&host_addr,sizeof(info->Address));
   info->ConnectInfo = (ClientState&0xFF);
   if(!this->IsConnected() && !info->ConnectInfo)
       info->ConnectInfo |= 0x8000;
   info->WorkTime = WorkTime; 
   unlock();
   if((info == &_info) && rx_queue)
    this->PutDataToQueue(rx_queue,ClientId,CmInformation,0,0,info,sizeof(*info));
 }

 void     __fastcall TGrNetClient::EnumClients(GRNET_EnumClientsProc proc,LPVOID UserData)
 {
  if(proc)
  {
   CLIENT_INFO info;
   this->MakeClientInfo(&info);
   proc(UserData,&info);
  }
 }


 void         __fastcall TGrNetClient::AcceptConnection(TGRNetBuffer & buff)
 {
  //Подтверждение соединения
  lock();
  if(owner)
  {
   TBeginConnection bc={0};
   lstrcpynA(bc.ClientName,name,sizeof(bc.ClientName));
   lstrcpynA(bc.ServerName,server_name,sizeof(bc.ServerName));
   bc.ID = ClientId;
   bc.ClientLevel = 1;
   bc.ErrorConnection = 0;
   buff.SetData(&bc,sizeof(bc));
   buff.msg = *&TNetMessage(CmConnect,ClientId,1,0,0);
   buff.msg.ReceiptMessage();
   buff.CalcCheckSum();
   LastTxKadrNumber = LastRxKadrNumber = 0;
   Send(host_addr,&buff,buff.GetTotalLen());
    if(!IsConnected())
	{
       ClientState = 0;
	   MakeClientInfo();
	}
  }
  unlock();
 }

