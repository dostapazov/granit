
#include "grnet232.hpp"

  #define ID2IDX(x) ((x&0xFF))
  #define IDX2ID(x) ((x|0x100))

  TGrNetServer::TGrNetServer(const char * name,WORD Socket):
  TGrNetBase(Socket,name),
  clients_pool(0),
  MaxClients(DEFAULT_CLIENTS_COUNT)
  {
   if(name==0 || lstrlenA(name)==0)
    {
     char buff[MAX_PATH];
     DWORD sz = sizeof(buff);
     GetComputerName(buff,&sz);
     SetName(buff); 
    }
  }


  bool   __fastcall        TGrNetServer::CreateClients()
  {
   if(clients_pool)
   {
    TGrNetClient * client ;
    for(DWORD i = 0;i<MaxClients;i++)
    {
     client = new TGrNetClient(this,IDX2ID(i),rx_queue);
     client->WaitRcptTime = WaitRcptTime+i;
     if(!client)
        return false;
      clients_pool[i] = client;
    }
    return true;
   }
   return false;
  }

  void   __fastcall        TGrNetServer::DeleteClients()
  {
    TGrNetClient * client ;
    for(DWORD i = 0;i<MaxClients;i++)
    {
     client = clients_pool[i];
     if(client)
     {
      client->SetClientState(stNotConnect|dcLostConnection,true);
	  client->TerminateAndWait(client->GetTerminateTime(),true);
      delete client;
     }
     clients_pool[i] = 0;
    }
  }

  void        __fastcall TGrNetServer::DisconnectClients()
  {

    TGrNetClient * client ;
    for(DWORD i = 0;i<MaxClients;i++)
    {
     client = clients_pool[i];
     if(client)
     {
         client->SetClientState(stNotConnect|dcLostConnection,true);
		 client->Terminate();
     }
     
    }
  }

  bool   __fastcall       TGrNetServer::BeforeExecute ()
  {
   bool ret(true);
   lock();
   if(clients_pool == 0)
   {
    clients_pool = new TGrNetClient*[MaxClients];
    ret = CreateClients();
   }
   if(ret)
      ret = TGrNetBase::BeforeExecute();
   unlock();
   return ret;
  }

  void   __fastcall       TGrNetServer::BeforeTerminate()
  {
   DeleteClients();
   TGrNetBase::BeforeTerminate(); 
   delete [] clients_pool;
   clients_pool = 0;
  }

  void   __fastcall       TGrNetServer::HandleReceive(TIpxAddress & from,LPVOID data,int bytes)
  {
   TGRNetBuffer * buff = (TGRNetBuffer*)data;
   if(buff->IsValid())
   {
     if(buff->msg.command == CmConnect)
     {
      TBeginConnection bc;
      buff->GetData(&bc,sizeof(bc));
      if(SameName(bc.ServerName)) //Если имя запрашиваемого сервера совпадает ->
         RegisterClient(from,bc,*buff); //регистрируем клиента
      
     }
   }
  }

  void   __fastcall TGrNetServer::RegisterClient(TIpxAddress & from,TBeginConnection & bc,TGRNetBuffer & buff)
  {
   TGrNetClient * free_client = 0,*client = 0;
   lock();
   for(DWORD i = 0;i<MaxClients&& !free_client;i++)
   {
    client = clients_pool[i];
    if(!client->IsConnected() && client->GetState()!=tsRunning)
      {
       if(free_client == 0)
          free_client = client;
       client = 0;
      }
      else
      {
       if(client->SameName(bc.ClientName) && from == client->host_addr && client->GetState()==tsRunning)
         {free_client = 0;break;}
      }
   }

   if(client)
   {
    // Клиент уже зарегестрирован
    //client->lock();
    #ifdef _DEBUG
    _dbgprn(("Kлиент уже зарегестрирован гнездо %X ",DWORD(htons(client->host_addr.sa_socket))));
    #endif
    //client->AcceptConnection(buff);
    //client->ClientState = stNotConnect;
    //client->unlock();
    //client->TerminateAndWait(100);//Перезапуск
    //client->Start();
   }
   else
   {
    if(free_client)
    {
      free_client->host_addr = from;
      free_client->SetName(bc.ClientName);
      free_client->SetServerName(name);
      if(!free_client->Start())
      {
       //TODO ошибка при запуске

      }
    }
    else
    {
     //Нет места для
     bc.ErrorConnection = CONNECT_SPACE_ERROR;
     buff.SetData(&bc,sizeof(bc));
     buff.msg.ReceiptMessage();
     buff.CalcCheckSum();
     this->Send(from,&buff,buff.GetTotalLen());
    }
   }
   unlock();
  }

  void   __fastcall       TGrNetServer::Terminate()
  {
   TGrNetBase::Terminate();
  }


  DWORD  __fastcall       TGrNetServer::GetTerminateTime()
  {
   DWORD ret = TGrNetBase::GetTerminateTime();;
   lock();
   for(DWORD i = 0;i<MaxClients && clients_pool;i++)
   {
    ret+=clients_pool[i]->GetTerminateTime();
   }
   unlock();
   return ret;
  }


  bool        __fastcall TGrNetServer::SendTo(DWORD ClientId,DWORD Command, LPVOID data, DWORD length, bool NeedReceipt,bool First)
  {
   bool ret(false);
   TGrNetClient * client;
   lock();
   if(ClientId ==DWORD(-1))
   {
     for(DWORD i =0;i<MaxClients;i++)
         {
          client =clients_pool[i];
          if(client)
          client->SendTo(ClientId,Command,data,length,NeedReceipt,First);
         }
     ret = true;

   }
   else
   {
    DWORD idx = ID2IDX(ClientId);
    if(idx<MaxClients)
        client = clients_pool[idx];
        else
        client = 0;
      if(client)
      ret = client->SendTo(ClientId,Command,data,length,NeedReceipt,First);
   }
   unlock();
   return ret;
  }

  void        __fastcall TGrNetServer::EnumClients(GRNET_EnumClientsProc proc,LPVOID UserData)
  {
   lock();
   TGrNetClient * client;
   CLIENT_INFO    info;
   for(DWORD i=0;i<MaxClients && proc && clients_pool;i++)
   {
    client = clients_pool[i];
    if(client && client->IsConnected())
     {
      client->MakeClientInfo(&info);
      if(0==proc(UserData,&info))
         break;
     }
   }
   unlock();
  }

  DWORD       __fastcall TGrNetServer::GetWaitReceiptTimeout(DWORD id)
  {
   DWORD ret(0);
   if(id == DWORD(-1))
   {
    ret = TGrNetBase::GetWaitReceiptTimeout(id);
   }
   else
   {
   id = ID2IDX(id);
   if(id<MaxClients)
    {
     lock();
     TGrNetClient * client = clients_pool[id];
     if(client)
     ret = client->GetWaitReceiptTimeout(IDX2ID(id));

     unlock();
    }
    }
    return ret;
  }

  bool        __fastcall TGrNetServer::SetWaitReceiptTimeout(DWORD ClID,DWORD to)
  {
   lock();
   if(ClID == DWORD(-1))
      {
       TGrNetBase::SetWaitReceiptTimeout(ClID,to);
       to = this->WaitRcptTime;
      }

   TGrNetClient * client;
   for(DWORD i = 0;i<MaxClients && clients_pool;i++)
    {
       if(i == ID2IDX(ClID) || ClID == DWORD(-1))
        {
         client = clients_pool[i];
         if(client)
          client->SetWaitReceiptTimeout(IDX2ID(i),to+i);
        }
    }
   unlock();
   return true;
  }

  DWORD       __fastcall TGrNetServer::GetRetryCount(DWORD id)
  {
   DWORD ret(0);
   if(id == DWORD(-1))
   {
    ret = TGrNetBase::GetRetryCount(id);
   }
   else
   {
   id = ID2IDX(id);
   if(id<MaxClients)
    {
     lock();
     TGrNetClient * client = clients_pool[id];
     if(client)
     ret = client->GetRetryCount(IDX2ID(id));

     unlock();
    }
    }
    return ret;
    }

  bool        __fastcall TGrNetServer::SetRetryCount(DWORD ClID,DWORD rc)
  {
   lock();
   if(ClID == DWORD(-1))
      {
       TGrNetBase::SetRetryCount(ClID,rc);
       rc = this->RetryCount;
      }

   TGrNetClient * client;
   for(DWORD i = 0;i<MaxClients && clients_pool;i++)
    {
       if(i == ID2IDX(ClID) || ClID == DWORD(-1))
        {
         client = clients_pool[i];
         if(client)
          client->SetRetryCount(IDX2ID(i),rc);
        }
    }
   unlock();
   return true;
  }


