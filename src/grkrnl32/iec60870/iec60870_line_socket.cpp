#pragma hdrstop
#include "iec60870_modem.hpp"

   const int wait_terminate = 0;
   const int wait_timer     = 1;
   const int wait_socket    = 2;

   int __fastcall Tiec60870line::socket_execute()
   {
     connect_socket();
     //wtimer->StartPeriodTimer(1000,false);
     DWORD wait_result = -1;

     do{
       wait_result =  sset.Wait(get_wait_ticks(),false);
       switch(wait_result)
       {
        case wait_timer   : on_waitable_timer ();break;
        case wait_socket  : on_socket_event   ();break;
        case WAIT_TIMEOUT : on_socket_timeout ();break;
       }
      }while(wait_result || !this->CheckTerminateRequest());
     connect       (false);
     destroy_socket();
     return 0;
   }

    bool  __fastcall Tiec60870line::create_socket     ()
    {
       destroy_socket();
       socket = new TIPSocket(true);
       if(socket)
       {
         socket->EnableReuseAddress(true);
         socket->EnableKeepAlive   (true);
         net_events = WSACreateEvent();
         if(net_events && net_events!=INVALID_HANDLE_VALUE)
           {
            WSAEventSelect(socket->GetSocket(),net_events,FD_READ|FD_CLOSE|FD_CONNECT);
            sset+=net_events;
            return true;
           }
       }
       destroy_socket();
       return false;
    }

    void  __fastcall Tiec60870line::destroy_socket    ()
    {
      if(net_events && net_events!=INVALID_HANDLE_VALUE)
        {
          sset-=net_events;
          WSACloseEvent(net_events);
          net_events = INVALID_HANDLE_VALUE;
        }
      if(this->socket)
        {
          socket->DestroySocket();
          delete socket;
          socket = NULL;
        }
    }

   void __fastcall Tiec60870line::connect_socket   ()
   {
    if(create_socket())
    {
        rxbuf.clear();
        set_state(MODEM_LINE_STATE_CONNECTING,true,true);
        {
         TLockHelper l(locker);
         char addr_text[1024];
         Unicode2Ansi(addr_text,sizeof(addr_text),line_config.host_addr);
         TIPAddress host_addr (addr_text, line_config.host_port);
         socket->Connect(host_addr(),sizeof(host_addr));
         countered_timer_start(5);
        }
    }
    else
     connect(false);
   }


   void __fastcall Tiec60870line::on_socket_timeout()
   {
        if(is_countered_timer_fired())
          {
           if(is_connected())
             {
             }
              else
              {
               connect(false);
               connect_socket();
              }
          }
   }

   void  __fastcall Tiec60870line::on_socket_event ()
   {
     WSANETWORKEVENTS ne={0};
     if(socket)
     {
       while(!WSAEnumNetworkEvents(socket->GetSocket(),net_events,&ne) && ne.lNetworkEvents)
         {
          if(ne.lNetworkEvents&FD_CONNECT)
             {
              on_socket_connect(ne.iErrorCode[FD_CONNECT_BIT]);
              ne.lNetworkEvents&=~FD_CONNECT;
             }

          if(ne.lNetworkEvents&FD_READ)
            {
             on_socket_recv(ne.iErrorCode[FD_READ_BIT]);
             ne.lNetworkEvents&=~FD_READ;
            }

          if(ne.lNetworkEvents&FD_WRITE)
          {
           DWORD send_error = ne.iErrorCode[FD_WRITE_BIT];
           ne.lNetworkEvents&=~FD_WRITE;
           if(!send_error )
           {

            //prepare next send
           }
           else
             this->on_socket_disconnect(send_error);
          }

          if(ne.lNetworkEvents&FD_CLOSE)
          {
            on_socket_disconnect(ne.iErrorCode[FD_CLOSE_BIT]);
            ne.lNetworkEvents&=~FD_CLOSE;
          }
         ne.lNetworkEvents = 0;
       }
     }
     else
     {
       socket = socket;
     }
    }


   void __fastcall Tiec60870line::on_socket_connect   (DWORD error)
   {
     //connect(true);
     if(!error)
     {
      rx_number.set_number(0);
      tx_number = rx_number;
      BYTE buffer[32];
      lpiec60870_proto_header_t phdr = iec60870_proto_startdt(buffer,sizeof(buffer),false);
      do_socket_send((LPBYTE)phdr,iec60870_proto_get_frame_length(phdr));
      countered_timer_start(3);
     }
     else
     {

     }
    //send startd act
  }

   void __fastcall Tiec60870line::on_socket_recv      (DWORD error)
   {
     BYTE  rcbuf[1024];
     DWORD len = socket->Recv(rcbuf,sizeof(rcbuf),0);
     while(len)
     {
       update_stats_rx(len,0,0);
       rxbuf.add_bytes(rcbuf,len);
       len = socket->Recv(rcbuf,sizeof(rcbuf),0);

     }
     handle_recv();
     this->countered_timer_start(3);
   }

   void __fastcall Tiec60870line::on_socket_disconnect(DWORD error)
   {
    connect(false);
    countered_timer_start(5);
   }

   bool __fastcall Tiec60870line::do_socket_send      (LPBYTE ptr,DWORD len)
   {
    DWORD ret = socket->Send(ptr,len,0);
    owner->notify(MNF_LINE_TX,get_number(),ptr,len);
    update_stats_tx(len,1,ret!=len);
    if(ret!=len)//Ошибка передачи
          {
           destroy_socket() ;
           return false;
          }
          else
          WTimer_start(WT_TX_TIMER);
    return true;
   }





