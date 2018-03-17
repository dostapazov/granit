#ifndef OTDVIEWERMAIN_H
#define OTDVIEWERMAIN_H

#include <QMainWindow>
#include <QStringListModel>
#include <qtcpsocket.h>
#include <qstatusbar.h>
#include <qlabel.h>


#ifdef Q_OS_WIN
 #include <otd.h>
 #include <modem_proto.h>
 #include <otd_arch_proto.h>
 #include <cpc152proto.h>
#endif

#ifdef Q_OS_LINUX
 #include <boost/lexical_cast.hpp>
 #include <gklib/otd.h>
 #include <gklib/modem_proto.h>
 #include <gklib/otd_arch_proto.h>
 #include <gklib/cpc152proto.h>
#endif






namespace Ui {
class OtdViewerMain;
}

class OtdViewerMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit OtdViewerMain(QWidget *parent = 0);
    ~OtdViewerMain();

private slots:
    void on_act_exit_triggered();
    void on_act_about_triggered();
    void on_bConnect_clicked();
    void on_socket_read();
    void on_socket_connected();
    void on_socket_disconnected();
    void on_cbDiag_clicked();

    qint32 get_part_mask();

    void on_bCallData_clicked();

    void on_bOrderData_clicked();

    void on_bUnorderData_clicked();

    void on_bClearMon_clicked();
    void on_OtdViewerMain_iconSizeChanged(const QSize &iconSize);
    void on_bGetArchList_clicked();
    void on_bGetArchive_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_bEraseArchive_clicked();
    void on_bGetAndErase_clicked();
    void on_b_paused_clicked();
    void on_socket_write(qint64 wrb);

    void on_btnSendAlarmParam_clicked();

    void on_bCpc152AlarmAdd_clicked();

    void on_bCpc152AlarmErase_clicked();

    void on_bCpc152AlarmClear_clicked();

    void on_bCpc152GetAlarmList_clicked();

    void on_bCpc152GetAlarm_clicked();

private:
    Ui::OtdViewerMain *ui;
    bool       socket_connected;
    quint32    part_mask;
    quint16    pkt_num;
    QTcpSocket socket;
    QLabel     * status_text;
    modem_proto_buffer mpb;
    int timer_id;
    int no_tx_timer;
    int no_rx_timer;
    unsigned int total_kadrs;
    unsigned int total_bytes;
    unsigned int total_wr_bytes;

    QStringList        arch_list;
    QStringListModel * model;
    QStringListModel * cpc152_arch_model;

    int current_fa;

    void send_kpa      ();
    void send_query    (bool call,bool order);
    void print_string  (const char * str);
    void print_otd_data(const char * prefix,otd_data & od) ;
    void handle_mproto     (LPMPROTO_HEADER mph);
    void monitor_otd       (const char *prefix, LPBYTE data, DWORD data_size);
    void handle_otd_archive(LPBYTE data, DWORD data_size);
    void handle_archive_command(LPOTD_ARCH_HEADER lah);
    void handle_archive_list   (LPOTD_ARCH_LIST al, bool end);
    void handle_archive_get    (LPOTD_ARCH_LIST al, bool end, bool erase);
    void handle_archive_erase  (LPOTD_ARCH_LIST al, bool end );
    int  get_fa_str            (DWORD fa,DWORD pp,char * buf,DWORD buf_len);
    int  get_tu_str            (otd_tutr & tutr,char * out);
    void update_status_bar();
    void timerEvent(QTimerEvent *event);
    void handle_cpc152_raw(LPMPROTO_HEADER mph);
    void cpc152_handle_alarms_list(lpcpc152proto_hdr phdr,bool is_end);
    void cpc152_time_sync (LPMPROTO_HEADER mph,lpcpc152proto_hdr phdr);
    void cpc152_scan_data (LPMPROTO_HEADER mph, lpcpc152proto_hdr phdr, bool is_end);
    void do_send          (LPMPROTO_HEADER mph);
    void cpc152_send_alarm_condition(bool erase = false,bool erase_all = false);
    void cpc152_new_alarm (lpcpc152proto_hdr phdr);

};

int         str2addr(char * str,LPBYTE addr,DWORD addr_sz,int delim);
int         str2addr(wchar_t * str,LPBYTE addr,DWORD addr_sz,int delim);


#endif // OTDVIEWERMAIN_H
