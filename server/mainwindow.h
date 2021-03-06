#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "build_number.h"
#include "SslServer.h"

#include <sys/time.h>
#include <time.h>
#include <cstdlib>
#include <stdlib.h>

#include <QMainWindow>
#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QVector>
#include <QVectorIterator>
#include <QDebug>
#include <QString>
#include <QSerialPort>
#include <QIODevice>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtNetwork/QAbstractSocket>
#include <QSslSocket>
#include <QSslError>
#include <QList>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QCloseEvent>


#define MAX_CLIENTS 2
#define DEF_PORT_NUMBER 6543
#define DEF_SPEED 9600
#define DEF_TIMEOUT 1000

#ifdef __WIN32
    const QString def_serial_port = "COM3";
#else
    const QString def_serial_port = "/dev/ttyUSB0";
#endif

//-----------------------------------------------------------------------------------
typedef struct {
    QString ip_adr;
    uint16_t peerPort;
    bool connected;
    QSslSocket *ssoc;
} s_cli;

//-----------------------------------------------------------------------------------

extern bool demos;
extern const QString DemoFileName;
extern const QString LogFileName;
extern const QString ipFileName;
extern uint16_t bPort;
extern QString sPort;
extern int sSpeed;

extern void LogSave(const char *func, QString st, bool pr);
extern void parse_param_start(char *param);

//-----------------------------------------------------------------------------------
namespace Ui {
    class MainWindow;
}
//-----------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    class TheError {
        public :
            int code;
            TheError(int);
    };

    explicit MainWindow(QWidget *parent = nullptr, uint16_t bp = DEF_PORT_NUMBER, QString sp = def_serial_port, int ss = DEF_SPEED);
    ~MainWindow();

    void cli_prn(s_cli *);
    int readserdmp();
    bool GetIpFile();
    QString sec_to_time(time_t &);
    void timerEvent(QTimerEvent *);

public slots:

    void slot_Release();
    void slot_About();
    void slotComCli();
    void slotDemoGetData();
    void newuser();
    void slotErrorClient(QAbstractSocket::SocketError SErr);
    void slotSslCliDone(QSslSocket *);
    void slotSendPack();
    // serial slot
    void sReadyRead();
    void sError(QSerialPort::SerialPortError serialPortError);
    void sTimeout();
    //tray
    void changeEvent(QEvent*);
    void closeEvent(QCloseEvent *);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayActionExecute();
    void setTrayIconActions();
    void showTrayIcon();

signals:

    void sigDemoGetData();
    void sigSslCliDone(QSslSocket *);
    void sigSendPack();
    void sslErrors(QList<QSslError> &);

private:

    Ui::MainWindow *ui;
    int tmr, MyError, cnt, tmr_msec;
    time_t startTime;
    QVector<QString> ip_adr;
    s_cli cli;
    QVector<s_cli> all_cli;
    QSerialPort *sobj;
    QFile *dmpFile;
    QByteArray buf;
    int max_period;

    QTimer serial_timer;
    QString serial_port;
    int serial_speed;
    uint16_t bind_port;
    int server_status;
    bool client;
    uint32_t total_pack, send_pack;
    QString CliUrl;

    QQueue<QByteArray> ku;

    QSslSocket *ssoc;
    SslServer *sslServer;

    QProcess *lastProc;

    //tray
    bool fst;
    int tmr_hide;
    QMenu *trayIconMenu;
    QAction *minA;
    QAction *maxA;
    QAction *quitA;
    QSystemTrayIcon *trayIcon;   

    const QString pic_path = "png/";
    const QString pic_main = pic_path + "srv_main.png";
    const QString pic_hide = pic_path + "eyeHide.png";
    const QString pic_show = pic_path + "eyeShow.png";
    const QString pic_close = pic_path + "close.png";

    const QString fCert = "key.pem";
    const QString fKey = "key.key";

};

#endif // MAINWINDOW_H
