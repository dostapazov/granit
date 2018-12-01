/********************************************************************************
** Form generated from reading UI file 'otdviewermain.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OTDVIEWERMAIN_H
#define UI_OTDVIEWERMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OtdViewerMain
{
public:
    QAction *act_about;
    QAction *act_exit;
    QWidget *centralWidget;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_6;
    QPlainTextEdit *monitor;
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_4;
    QLabel *label_2;
    QLineEdit *otd_server_port;
    QLabel *label;
    QLineEdit *otd_server_addr;
    QPushButton *bConnect;
    QPushButton *bClearMon;
    QPushButton *b_paused;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_3;
    QGridLayout *gridLayout;
    QCheckBox *cbDiag;
    QCheckBox *cbData;
    QCheckBox *cbTimestamp;
    QCheckBox *cbPersonalDiag;
    QGridLayout *gridLayout_2;
    QCheckBox *cbChangesMask;
    QCheckBox *cbNames;
    QCheckBox *cbPartData;
    QGridLayout *gridLayout_3;
    QPushButton *bCallData;
    QPushButton *bOrderData;
    QPushButton *bUnorderData;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLineEdit *otd_addr;
    QSpacerItem *horizontalSpacer;
    QWidget *tab_2;
    QListView *ArchList;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    QPushButton *bGetArchList;
    QPushButton *bGetArchive;
    QPushButton *bGetAndErase;
    QPushButton *bEraseArchive;
    QWidget *tab_3;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_8;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *acClear;
    QRadioButton *acLess;
    QRadioButton *acMore;
    QLabel *label_4;
    QLineEdit *apAddrParam;
    QLabel *label_5;
    QLineEdit *apValue;
    QPushButton *btnSendAlarmParam;
    QWidget *tab_4;
    QListView *cpc152_alarm_list;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout;
    QPushButton *bCpc152GetAlarmList;
    QPushButton *bCpc152GetAlarm;
    QCheckBox *cbEraseAlarmFile;
    QCheckBox *cbOnlyArchShow;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QWidget *layoutWidget4;
    QVBoxLayout *verticalLayout_5;
    QLineEdit *dev_num;
    QLineEdit *grp_num;
    QLineEdit *param_num;
    QWidget *layoutWidget5;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *RB_cpc152_off;
    QRadioButton *RB_cpc152_on;
    QSplitter *splitter;
    QPushButton *bCpc152AlarmAdd;
    QPushButton *bCpc152AlarmErase;
    QPushButton *bCpc152AlarmClear;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *OtdViewerMain)
    {
        if (OtdViewerMain->objectName().isEmpty())
            OtdViewerMain->setObjectName(QStringLiteral("OtdViewerMain"));
        OtdViewerMain->resize(1006, 726);
        QIcon icon;
        icon.addFile(QStringLiteral("Connections.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        OtdViewerMain->setWindowIcon(icon);
        act_about = new QAction(OtdViewerMain);
        act_about->setObjectName(QStringLiteral("act_about"));
        act_exit = new QAction(OtdViewerMain);
        act_exit->setObjectName(QStringLiteral("act_exit"));
        centralWidget = new QWidget(OtdViewerMain);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_7 = new QGridLayout(centralWidget);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setSpacing(6);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));

        gridLayout_7->addLayout(gridLayout_6, 2, 0, 1, 1);

        monitor = new QPlainTextEdit(centralWidget);
        monitor->setObjectName(QStringLiteral("monitor"));
        monitor->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(monitor->sizePolicy().hasHeightForWidth());
        monitor->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        font.setPointSize(10);
        font.setKerning(true);
        monitor->setFont(font);
        monitor->viewport()->setProperty("cursor", QVariant(QCursor(Qt::IBeamCursor)));
        monitor->setFocusPolicy(Qt::NoFocus);
        monitor->setAcceptDrops(false);
        monitor->setLayoutDirection(Qt::LeftToRight);
        monitor->setAutoFillBackground(false);
        monitor->setInputMethodHints(Qt::ImhNone);
        monitor->setFrameShape(QFrame::WinPanel);
        monitor->setFrameShadow(QFrame::Sunken);
        monitor->setLineWrapMode(QPlainTextEdit::NoWrap);
        monitor->setReadOnly(true);
        monitor->setMaximumBlockCount(2048);
        monitor->setBackgroundVisible(true);

        gridLayout_7->addWidget(monitor, 1, 1, 1, 1);

        gridLayout_5 = new QGridLayout();
        gridLayout_5->setSpacing(6);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        gridLayout_4 = new QGridLayout(groupBox);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_4->addWidget(label_2, 1, 0, 1, 1);

        otd_server_port = new QLineEdit(groupBox);
        otd_server_port->setObjectName(QStringLiteral("otd_server_port"));

        gridLayout_4->addWidget(otd_server_port, 1, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setFrameShape(QFrame::NoFrame);
        label->setFrameShadow(QFrame::Sunken);
        label->setTextFormat(Qt::PlainText);

        gridLayout_4->addWidget(label, 0, 0, 1, 1);

        otd_server_addr = new QLineEdit(groupBox);
        otd_server_addr->setObjectName(QStringLiteral("otd_server_addr"));

        gridLayout_4->addWidget(otd_server_addr, 0, 1, 1, 1);

        bConnect = new QPushButton(groupBox);
        bConnect->setObjectName(QStringLiteral("bConnect"));
        bConnect->setIcon(icon);
        bConnect->setIconSize(QSize(16, 16));

        gridLayout_4->addWidget(bConnect, 2, 0, 1, 1);

        bClearMon = new QPushButton(groupBox);
        bClearMon->setObjectName(QStringLiteral("bClearMon"));

        gridLayout_4->addWidget(bClearMon, 3, 0, 1, 1);

        b_paused = new QPushButton(groupBox);
        b_paused->setObjectName(QStringLiteral("b_paused"));
        b_paused->setCheckable(true);
        b_paused->setChecked(false);

        gridLayout_4->addWidget(b_paused, 2, 1, 1, 1);


        gridLayout_5->addWidget(groupBox, 0, 0, 1, 1);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        horizontalLayout_3 = new QHBoxLayout(tab);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cbDiag = new QCheckBox(tab);
        cbDiag->setObjectName(QStringLiteral("cbDiag"));
        cbDiag->setChecked(true);

        gridLayout->addWidget(cbDiag, 0, 0, 1, 1);

        cbData = new QCheckBox(tab);
        cbData->setObjectName(QStringLiteral("cbData"));
        cbData->setChecked(true);

        gridLayout->addWidget(cbData, 1, 0, 1, 1);

        cbTimestamp = new QCheckBox(tab);
        cbTimestamp->setObjectName(QStringLiteral("cbTimestamp"));
        cbTimestamp->setChecked(true);

        gridLayout->addWidget(cbTimestamp, 2, 0, 1, 1);

        cbPersonalDiag = new QCheckBox(tab);
        cbPersonalDiag->setObjectName(QStringLiteral("cbPersonalDiag"));
        cbPersonalDiag->setChecked(true);

        gridLayout->addWidget(cbPersonalDiag, 3, 0, 1, 1);


        horizontalLayout_3->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        cbChangesMask = new QCheckBox(tab);
        cbChangesMask->setObjectName(QStringLiteral("cbChangesMask"));
        cbChangesMask->setChecked(true);

        gridLayout_2->addWidget(cbChangesMask, 0, 0, 1, 1);

        cbNames = new QCheckBox(tab);
        cbNames->setObjectName(QStringLiteral("cbNames"));
        cbNames->setChecked(true);

        gridLayout_2->addWidget(cbNames, 1, 0, 1, 1);

        cbPartData = new QCheckBox(tab);
        cbPartData->setObjectName(QStringLiteral("cbPartData"));

        gridLayout_2->addWidget(cbPartData, 2, 0, 1, 1);


        horizontalLayout_3->addLayout(gridLayout_2);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        bCallData = new QPushButton(tab);
        bCallData->setObjectName(QStringLiteral("bCallData"));
        bCallData->setAutoRepeat(true);
        bCallData->setAutoRepeatDelay(250);
        bCallData->setAutoRepeatInterval(0);
        bCallData->setFlat(false);

        gridLayout_3->addWidget(bCallData, 0, 0, 1, 1);

        bOrderData = new QPushButton(tab);
        bOrderData->setObjectName(QStringLiteral("bOrderData"));

        gridLayout_3->addWidget(bOrderData, 1, 0, 1, 1);

        bUnorderData = new QPushButton(tab);
        bUnorderData->setObjectName(QStringLiteral("bUnorderData"));

        gridLayout_3->addWidget(bUnorderData, 2, 0, 1, 1);


        horizontalLayout_3->addLayout(gridLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        otd_addr = new QLineEdit(tab);
        otd_addr->setObjectName(QStringLiteral("otd_addr"));
        sizePolicy1.setHeightForWidth(otd_addr->sizePolicy().hasHeightForWidth());
        otd_addr->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(otd_addr);


        horizontalLayout_3->addLayout(horizontalLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        ArchList = new QListView(tab_2);
        ArchList->setObjectName(QStringLiteral("ArchList"));
        ArchList->setGeometry(QRect(141, 10, 481, 111));
        ArchList->setResizeMode(QListView::Adjust);
        ArchList->setLayoutMode(QListView::Batched);
        layoutWidget = new QWidget(tab_2);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(2, 3, 138, 124));
        verticalLayout_3 = new QVBoxLayout(layoutWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        bGetArchList = new QPushButton(layoutWidget);
        bGetArchList->setObjectName(QStringLiteral("bGetArchList"));

        verticalLayout_3->addWidget(bGetArchList);

        bGetArchive = new QPushButton(layoutWidget);
        bGetArchive->setObjectName(QStringLiteral("bGetArchive"));

        verticalLayout_3->addWidget(bGetArchive);

        bGetAndErase = new QPushButton(layoutWidget);
        bGetAndErase->setObjectName(QStringLiteral("bGetAndErase"));

        verticalLayout_3->addWidget(bGetAndErase);

        bEraseArchive = new QPushButton(layoutWidget);
        bEraseArchive->setObjectName(QStringLiteral("bEraseArchive"));

        verticalLayout_3->addWidget(bEraseArchive);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        layoutWidget1 = new QWidget(tab_3);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 0, 335, 117));
        gridLayout_8 = new QGridLayout(layoutWidget1);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_8->setContentsMargins(0, 0, 0, 0);
        groupBox_2 = new QGroupBox(layoutWidget1);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setFlat(true);
        groupBox_2->setCheckable(false);
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        acClear = new QRadioButton(groupBox_2);
        acClear->setObjectName(QStringLiteral("acClear"));

        verticalLayout_2->addWidget(acClear);

        acLess = new QRadioButton(groupBox_2);
        acLess->setObjectName(QStringLiteral("acLess"));

        verticalLayout_2->addWidget(acLess);

        acMore = new QRadioButton(groupBox_2);
        acMore->setObjectName(QStringLiteral("acMore"));
        acMore->setChecked(true);

        verticalLayout_2->addWidget(acMore);


        horizontalLayout_2->addLayout(verticalLayout_2);


        gridLayout_8->addWidget(groupBox_2, 0, 0, 3, 1);

        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_8->addWidget(label_4, 0, 1, 1, 1);

        apAddrParam = new QLineEdit(layoutWidget1);
        apAddrParam->setObjectName(QStringLiteral("apAddrParam"));

        gridLayout_8->addWidget(apAddrParam, 0, 2, 1, 1);

        label_5 = new QLabel(layoutWidget1);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_8->addWidget(label_5, 1, 1, 1, 1);

        apValue = new QLineEdit(layoutWidget1);
        apValue->setObjectName(QStringLiteral("apValue"));

        gridLayout_8->addWidget(apValue, 1, 2, 1, 1);

        btnSendAlarmParam = new QPushButton(layoutWidget1);
        btnSendAlarmParam->setObjectName(QStringLiteral("btnSendAlarmParam"));

        gridLayout_8->addWidget(btnSendAlarmParam, 2, 1, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        cpc152_alarm_list = new QListView(tab_4);
        cpc152_alarm_list->setObjectName(QStringLiteral("cpc152_alarm_list"));
        cpc152_alarm_list->setGeometry(QRect(132, 0, 270, 111));
        layoutWidget2 = new QWidget(tab_4);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(0, 1, 131, 111));
        verticalLayout = new QVBoxLayout(layoutWidget2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        bCpc152GetAlarmList = new QPushButton(layoutWidget2);
        bCpc152GetAlarmList->setObjectName(QStringLiteral("bCpc152GetAlarmList"));

        verticalLayout->addWidget(bCpc152GetAlarmList);

        bCpc152GetAlarm = new QPushButton(layoutWidget2);
        bCpc152GetAlarm->setObjectName(QStringLiteral("bCpc152GetAlarm"));

        verticalLayout->addWidget(bCpc152GetAlarm);

        cbEraseAlarmFile = new QCheckBox(layoutWidget2);
        cbEraseAlarmFile->setObjectName(QStringLiteral("cbEraseAlarmFile"));

        verticalLayout->addWidget(cbEraseAlarmFile);

        cbOnlyArchShow = new QCheckBox(layoutWidget2);
        cbOnlyArchShow->setObjectName(QStringLiteral("cbOnlyArchShow"));

        verticalLayout->addWidget(cbOnlyArchShow);

        layoutWidget3 = new QWidget(tab_4);
        layoutWidget3->setObjectName(QStringLiteral("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(405, 3, 74, 80));
        verticalLayout_4 = new QVBoxLayout(layoutWidget3);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_6 = new QLabel(layoutWidget3);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout_4->addWidget(label_6);

        label_7 = new QLabel(layoutWidget3);
        label_7->setObjectName(QStringLiteral("label_7"));

        verticalLayout_4->addWidget(label_7);

        label_8 = new QLabel(layoutWidget3);
        label_8->setObjectName(QStringLiteral("label_8"));

        verticalLayout_4->addWidget(label_8);

        layoutWidget4 = new QWidget(tab_4);
        layoutWidget4->setObjectName(QStringLiteral("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(478, 3, 127, 80));
        verticalLayout_5 = new QVBoxLayout(layoutWidget4);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        dev_num = new QLineEdit(layoutWidget4);
        dev_num->setObjectName(QStringLiteral("dev_num"));

        verticalLayout_5->addWidget(dev_num);

        grp_num = new QLineEdit(layoutWidget4);
        grp_num->setObjectName(QStringLiteral("grp_num"));

        verticalLayout_5->addWidget(grp_num);

        param_num = new QLineEdit(layoutWidget4);
        param_num->setObjectName(QStringLiteral("param_num"));

        verticalLayout_5->addWidget(param_num);

        layoutWidget5 = new QWidget(tab_4);
        layoutWidget5->setObjectName(QStringLiteral("layoutWidget5"));
        layoutWidget5->setGeometry(QRect(406, 90, 190, 22));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget5);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        RB_cpc152_off = new QRadioButton(layoutWidget5);
        RB_cpc152_off->setObjectName(QStringLiteral("RB_cpc152_off"));

        horizontalLayout_4->addWidget(RB_cpc152_off);

        RB_cpc152_on = new QRadioButton(layoutWidget5);
        RB_cpc152_on->setObjectName(QStringLiteral("RB_cpc152_on"));

        horizontalLayout_4->addWidget(RB_cpc152_on);

        splitter = new QSplitter(tab_4);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(610, 1, 80, 66));
        splitter->setOrientation(Qt::Vertical);
        bCpc152AlarmAdd = new QPushButton(splitter);
        bCpc152AlarmAdd->setObjectName(QStringLiteral("bCpc152AlarmAdd"));
        splitter->addWidget(bCpc152AlarmAdd);
        bCpc152AlarmErase = new QPushButton(splitter);
        bCpc152AlarmErase->setObjectName(QStringLiteral("bCpc152AlarmErase"));
        splitter->addWidget(bCpc152AlarmErase);
        bCpc152AlarmClear = new QPushButton(splitter);
        bCpc152AlarmClear->setObjectName(QStringLiteral("bCpc152AlarmClear"));
        splitter->addWidget(bCpc152AlarmClear);
        tabWidget->addTab(tab_4, QString());

        gridLayout_5->addWidget(tabWidget, 0, 1, 1, 1);


        gridLayout_7->addLayout(gridLayout_5, 0, 0, 1, 3);

        OtdViewerMain->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(OtdViewerMain);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1006, 18));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        OtdViewerMain->setMenuBar(menuBar);
        mainToolBar = new QToolBar(OtdViewerMain);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        OtdViewerMain->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(OtdViewerMain);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        OtdViewerMain->setStatusBar(statusBar);
        QWidget::setTabOrder(cbPersonalDiag, cbChangesMask);
        QWidget::setTabOrder(cbChangesMask, otd_server_addr);
        QWidget::setTabOrder(otd_server_addr, otd_server_port);
        QWidget::setTabOrder(otd_server_port, cbDiag);
        QWidget::setTabOrder(cbDiag, cbData);
        QWidget::setTabOrder(cbData, cbTimestamp);
        QWidget::setTabOrder(cbTimestamp, cbNames);
        QWidget::setTabOrder(cbNames, cbPartData);
        QWidget::setTabOrder(cbPartData, bCallData);
        QWidget::setTabOrder(bCallData, bOrderData);
        QWidget::setTabOrder(bOrderData, bUnorderData);
        QWidget::setTabOrder(bUnorderData, otd_addr);

        menuBar->addAction(menu->menuAction());
        menu->addAction(act_about);
        menu->addSeparator();
        menu->addAction(act_exit);

        retranslateUi(OtdViewerMain);

        tabWidget->setCurrentIndex(0);
        bCallData->setDefault(true);


        QMetaObject::connectSlotsByName(OtdViewerMain);
    } // setupUi

    void retranslateUi(QMainWindow *OtdViewerMain)
    {
        OtdViewerMain->setWindowTitle(QApplication::translate("OtdViewerMain", "OtdViewerMain", nullptr));
        act_about->setText(QApplication::translate("OtdViewerMain", "\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265", nullptr));
        act_exit->setText(QApplication::translate("OtdViewerMain", "\320\227\320\260\320\262\320\265\321\200\321\210\320\270\321\202\321\214", nullptr));
#ifndef QT_NO_TOOLTIP
        act_exit->setToolTip(QApplication::translate("OtdViewerMain", "\320\227\320\260\320\262\320\265\321\200\321\210\320\270\321\202\321\214 \321\200\320\260\320\261\320\276\321\202\321\203", nullptr));
#endif // QT_NO_TOOLTIP
        groupBox->setTitle(QApplication::translate("OtdViewerMain", "\320\237\320\276\320\264\320\272\320\273\321\216\321\207\320\265\320\275\320\270\320\265", nullptr));
        label_2->setText(QApplication::translate("OtdViewerMain", "\320\277\320\276\321\200\321\202", nullptr));
        otd_server_port->setText(QApplication::translate("OtdViewerMain", "45777", nullptr));
        label->setText(QApplication::translate("OtdViewerMain", "\321\201\320\265\321\200\320\262\320\265\321\200", nullptr));
        otd_server_addr->setText(QApplication::translate("OtdViewerMain", "127.0.0.1", nullptr));
        bConnect->setText(QApplication::translate("OtdViewerMain", "\320\237\320\276\320\264\320\272\320\273\321\216\321\207\320\270\321\202\321\214", nullptr));
        bClearMon->setText(QApplication::translate("OtdViewerMain", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \320\274\320\276\320\275\320\270\321\202\320\276\321\200", nullptr));
        b_paused->setText(QApplication::translate("OtdViewerMain", "\320\237\320\260\321\203\320\267\320\260", nullptr));
        cbDiag->setText(QApplication::translate("OtdViewerMain", "\320\224\320\270\320\260\320\263\320\275\320\276\321\201\321\202\320\270\320\272\320\260", nullptr));
        cbData->setText(QApplication::translate("OtdViewerMain", "\320\224\320\260\320\275\320\275\321\213\320\265", nullptr));
        cbTimestamp->setText(QApplication::translate("OtdViewerMain", "\320\234\320\265\321\202\320\272\320\260 \320\262\321\200\320\265\320\274\320\265\320\275\320\270", nullptr));
        cbPersonalDiag->setText(QApplication::translate("OtdViewerMain", "\320\237\320\265\321\200\321\201\320\276\320\275\320\260\320\273\321\214\320\275\320\260\321\217 \320\264\320\270\320\260\320\263\320\275\320\276\321\201\321\202\320\270\320\272\320\260", nullptr));
        cbChangesMask->setText(QApplication::translate("OtdViewerMain", "\320\234\320\260\321\201\320\272\320\260 \320\270\320\267\320\274\320\265\320\275\320\265\320\275\320\270\320\271", nullptr));
        cbNames->setText(QApplication::translate("OtdViewerMain", "\320\230\320\274\320\265\320\275\320\260", nullptr));
        cbPartData->setText(QApplication::translate("OtdViewerMain", "\320\247\320\260\321\201\321\202\320\270\321\207\320\275\321\213\320\265 \320\264\320\260\320\275\320\275\321\213\320\265", nullptr));
        bCallData->setText(QApplication::translate("OtdViewerMain", "\320\222\321\213\320\267\320\276\320\262 \320\264\320\260\320\275\320\275\321\213\321\205", nullptr));
        bOrderData->setText(QApplication::translate("OtdViewerMain", "\320\237\320\276\320\264\320\277\320\270\321\201\320\272\320\260", nullptr));
        bUnorderData->setText(QApplication::translate("OtdViewerMain", "\320\236\321\202\320\272\320\260\320\267 \320\276\321\202 \320\277\320\276\320\264\320\277\320\270\321\201\320\272\320\270", nullptr));
        label_3->setText(QApplication::translate("OtdViewerMain", "\320\220\320\264\321\200\320\265\321\201", nullptr));
        otd_addr->setText(QApplication::translate("OtdViewerMain", "255.255.255.255", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("OtdViewerMain", "\320\236\320\242\320\224", nullptr));
        bGetArchList->setText(QApplication::translate("OtdViewerMain", "\320\241\320\277\320\270\321\201\320\276\320\272", nullptr));
        bGetArchive->setText(QApplication::translate("OtdViewerMain", "\320\237\320\276\320\273\321\203\321\207\320\270\321\202\321\214 \320\260\321\200\321\205\320\270\320\262", nullptr));
        bGetAndErase->setText(QApplication::translate("OtdViewerMain", "\320\237\320\276\320\273\321\203\321\207\320\270\321\202\321\214 \320\270 \321\203\320\264\320\260\320\273\320\270\321\202\321\214", nullptr));
        bEraseArchive->setText(QApplication::translate("OtdViewerMain", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 \320\260\321\200\321\205\320\270\320\262", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("OtdViewerMain", "OTD \320\260\321\200\321\205\320\270\320\262\321\213", nullptr));
        groupBox_2->setTitle(QApplication::translate("OtdViewerMain", "\320\232\320\276\320\274\320\260\320\275\320\264\320\260", nullptr));
        acClear->setText(QApplication::translate("OtdViewerMain", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", nullptr));
        acLess->setText(QApplication::translate("OtdViewerMain", "\320\234\320\265\320\275\321\214\321\210\320\265", nullptr));
        acMore->setText(QApplication::translate("OtdViewerMain", "\320\221\320\276\320\273\321\214\321\210\320\265", nullptr));
        label_4->setText(QApplication::translate("OtdViewerMain", "\320\220\320\264\321\200\320\265\321\201 \320\277\320\260\321\200\320\260\320\274\320\265\321\202\321\200\320\260", nullptr));
        apAddrParam->setText(QApplication::translate("OtdViewerMain", "1.1.0.0-1", nullptr));
        label_5->setText(QApplication::translate("OtdViewerMain", "\320\227\320\275\320\260\321\207\320\265\320\275\320\270\320\265", nullptr));
        btnSendAlarmParam->setText(QApplication::translate("OtdViewerMain", "\320\222\321\213\320\277\320\276\320\273\320\275\320\270\321\202\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("OtdViewerMain", "\320\220\320\262\320\260\321\200\320\270\320\271\320\275\321\213\320\265 \321\203\321\201\321\202\320\260\320\262\320\272\320\270", nullptr));
        bCpc152GetAlarmList->setText(QApplication::translate("OtdViewerMain", "\320\241\320\277\320\270\321\201\320\276\320\272", nullptr));
        bCpc152GetAlarm->setText(QApplication::translate("OtdViewerMain", "\320\237\320\276\320\273\321\203\321\207\320\270\321\202\321\214", nullptr));
        cbEraseAlarmFile->setText(QApplication::translate("OtdViewerMain", "\320\243\320\264\320\260\320\273\321\217\321\202\321\214 \320\260\321\200\320\270\321\205\320\262\321\213", nullptr));
        cbOnlyArchShow->setText(QApplication::translate("OtdViewerMain", "\320\242\320\276\320\273\321\214\320\272\320\276 \320\260\321\200\321\205\320\270\320\262\321\213", nullptr));
        label_6->setText(QApplication::translate("OtdViewerMain", "\320\243\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\276", nullptr));
        label_7->setText(QApplication::translate("OtdViewerMain", "\320\263\321\200\321\203\320\277\320\277\320\260", nullptr));
        label_8->setText(QApplication::translate("OtdViewerMain", "\320\277\320\260\321\200\320\260\320\274\320\265\321\202\321\200", nullptr));
        RB_cpc152_off->setText(QApplication::translate("OtdViewerMain", "\320\236\320\242\320\232\320\233", nullptr));
        RB_cpc152_on->setText(QApplication::translate("OtdViewerMain", "\320\222\320\232\320\233", nullptr));
        bCpc152AlarmAdd->setText(QApplication::translate("OtdViewerMain", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", nullptr));
        bCpc152AlarmErase->setText(QApplication::translate("OtdViewerMain", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214", nullptr));
        bCpc152AlarmClear->setText(QApplication::translate("OtdViewerMain", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("OtdViewerMain", "\320\241PC152", nullptr));
        menu->setTitle(QApplication::translate("OtdViewerMain", "\320\237\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OtdViewerMain: public Ui_OtdViewerMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OTDVIEWERMAIN_H
