#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "build_number.h"

#include <sys/time.h>
#include <time.h>
#include <cstdlib>

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



#define MAX_CLIENTS 2
#define DEF_PORT_NUMBER 6543

#ifdef WIN32
    const QString def_serial_port = "COM1";
#else
    const QString def_serial_port = "/dev/ttyUSB0";
#endif

//-----------------------------------------------------------------------------------
typedef struct {
    QString ip_adr;
    bool connected;
    QTcpSocket *soc;
    uint16_t peerPort;
} s_cli;

//-----------------------------------------------------------------------------------

extern bool demos;
extern const QString DemoFileName;
extern const QString LogFileName;
extern const QString ipFileName;

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

    explicit MainWindow(QWidget *parent = nullptr, uint16_t bp = DEF_PORT_NUMBER, QString sp = def_serial_port);
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
    void slotCliDone(QTcpSocket *, int);
    void slotSendPack();

signals:

    void sigDemoGetData();
    void sigCliDone(QTcpSocket *, int);
    void sigSendPack();


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

    QString serial_port;
    uint16_t bind_port;
    int server_status;
    bool client;
    uint32_t total_pack, send_pack;
    QTcpServer *tcpServer;
    //QMap <int, s_cli> SClients;
    QString CliUrl;

};

#endif // MAINWINDOW_H
