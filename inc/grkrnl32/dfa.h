#ifndef _DFA_INC_H
#define _DFA_INC_H

#ifndef GKMODULES_INC_H
#include <gkmodules.h>
#endif

#define DFA_HANDLE_NAME L"DFA"

//������� ���

//����������� �����������
//p1 = LPDFA_HANDLER_DATA
//p2 = BOOL true - register, false - unregister
#define DFACMD_REGISTERHANDLER MCMD(MT_DFA,1)


typedef struct _DFA_HANDLER_DATA
{
 DWORD     dw_size;//������ ��������� ��� ��������
 DWORD     fa;     //����� ��
 GKHANDLE  module; //������ ����������
 DWORD     module_command;//������� ������������ ������ //��� ���������
 wchar_t  *handler_name;
 int       handler_phase;//������� ( ��������� ) ���������
}DFA_HANDLER_DATA,*LPDFA_HANDLER_DATA;

#define DFACMD_HANDLE MCMD    (MT_DFA,2)

#define DFACMD_HANDLE_AND_WAIT(MT_DFA,3)


typedef struct _DFA_HANDLE_DATA
{
 GKHANDLE      handler_module;//����������
 HANDLE        h_done_event;  //������� ���������� ���������
 LRESULT       ret_code;      //��� �������� ���������
 DWORD         data_size;     //������ ������ ��� ���������
 BYTE          data[1];       //������ ������
}DFA_HANDLE_DATA,*LPDFA_HANDLE_DATA;


#endif
 