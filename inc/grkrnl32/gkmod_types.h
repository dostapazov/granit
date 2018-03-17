#ifndef _GKMOD_TYPES_INC_
#define _GKMOD_TYPES_INC_

//��������������� ���� �������
#define MT_UNKNOWN      WORD(-1) //������ ������������ ����

#define MT_KERNEL             1        //������ ���� ������-�
#define MT_REPORTER           0x0002   //������ ��� ��������� �� �������


#define MT_TLMDB              0x0010   //������ ���������������� ��
#define MT_MEDIUMDB           0x0020   //���� ������ �������� ������
#define MT_SECURITY           0x0040   //������ ������ �������������
#define MT_MEDIUMDB_HISTORY   0x0080   //������ �������� �������

#define MT_MODEMMER           0x0200   //������ ���������
#define MT_IS_MODEM(type)     (MT_MODEMMER==(type&0xFF00))

#define MT_SCALE              0x0800   //�����

#define MT_TLMVIEWER          0x1000   //������������
#define MT_TLM_WORK_PLACE     0x2001   //��� ����������

#define MT_IO                 0x1000   //������ ����� ������
#define MT_WINDOW             0x4000   //������ ����
#define MT_USER               0x8000

#endif
