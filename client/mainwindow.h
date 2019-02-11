#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "build_number.h"
#include "global.h"

#include <inttypes.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <QApplication>
#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QEvent>
#include <QDateTime>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QPushButton>
#include <QAbstractButton>
#include <QFrame>
#include <QMouseEvent>
#include <QMovie>
#include <QFile>
#include <QtTest/QTest>

#include "itDialog.h"
#include "calc.h"

//--------------------------------------------------------------------------------
typedef struct {
    QString port;
    uint8_t type;
    uint8_t status;
    QString two;
    QStringList trunks;
} s_one;

typedef struct {
    QString port;
    QStringList trunks;
} s_tkgp;

#pragma pack(push,1)
typedef struct {
    uint32_t port;
    uint8_t status;
} s_stat;

typedef struct {
    uint32_t port1;
    uint32_t port2;
    uint8_t status;
} s_conn;
#pragma pack(pop)

enum port_type {
    SLT_TYPE = 0,
    KEY_TYPE = 1,
    TRK_TYPE = 2,
    GRP_TYPE = 3,
    MOD_TYPE = 4,
    CON_TYPE = 5,
    BAD_TYPE = 6
};
enum port_status {
    IDLE_STATUS = 0,
    BUSY_STATUS,
    LOCK_STATUS,
    NONE_STATUS
};

//--------------------------------------------------------------------------------

extern QString ServerIPAddress;
extern QString CfgFil;
extern QString TrkFil;
extern QString myPwd;
extern const QString _myPwd;

extern void parse_param_start(char *param);
extern void LogSave(const char *func, QString st, bool pr);

//--------------------------------------------------------------------------------
namespace Ui {
    class MainWindow;
}
//--------------------------------------------------------------------------------
class TheWin : public QTextEdit
{
    Q_OBJECT

public:

    explicit TheWin(QWidget *parent = nullptr, QRect *rc = nullptr,  s_one *ops = nullptr);
    ~TheWin();

    void _update(s_one *);
    void show_win(bool);
    QString get_port();
    uint8_t get_type();
    uint8_t get_status();
    QString get_two();
    s_one get_all();
    void set_two(QString);

private:

    QTextEdit *obj;
    QRect rect;
    s_one one;
};
//--------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    class TheError {
        public :
            int code;
            TheError(int);
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timerEvent(QTimerEvent *);
    bool eventFilter(QObject *, QEvent *);

    void SplitAdrPort(QString *);
    void InitScreen();
    void updateBar();
    bool GetConfigFile();
    QString sec_to_time(time_t &);

public slots:

    void slot_Release();
    void slot_About();
//    void mouseMoveEvent(QMouseEvent *);
    void slot_page(int);
    void slotShowHideAdminMenu(bool);
    void slotAdminOn();
    void slotAdminOff();
    void slotSetPwd(QString);
    void slotMakeCalc();

    void readData();
    void connectTcp();
    void disconnectTcp();
    void SokError(QAbstractSocket::SocketError SocketError);
    void slotNewCon();
    void slotPackParser(QByteArray &);
    void clearAllPort();

signals:
    void sigShowHideAdminMenu(bool);
    void sigNewCon();
    void sigPackParser(QByteArray &);

private:
    Ui::MainWindow *ui;
    int srv_port, MyError, tmr, tmr_wait, try_cnt, cntWait;
    QString srv_adr;
    QTcpSocket *_pSocket;
    QByteArray txdata, rxdata;
    bool beginSrv, errFirst, startWait;
    QString sbar;
    time_t time_start;

    QMovie *mv;

    int _page, _last, max_bad_len;
    int total_ports, AllBusyCount, MaxBusyCount;
    QMap<QString, TheWin *> map_adr;
    QVector<TheWin *> indexWin;
    bool conTrue, AdminFlag;

    QIcon *ico_black, *ico_green;

    pwdDialog *Dlg;

    CalcWin *Calc;

    QList<s_one> *alls = nullptr;
    QList<s_tkgp> *groups = nullptr;

};

#endif // MAINWINDOW_H
