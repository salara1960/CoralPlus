﻿#include "mainwindow.h"
#include "ui_mainwindow.h"


//const QString ver = "1.0 rc1";//07.02.2019
//const QString ver = "1.1 rc1";//08.02.2019
//const QString ver = "1.2";//08.02.2019
//const QString ver = "1.3";//10.02.2019
const QString ver = "1.4";//11.02.2019 - with queue for packets

bool demos = false;

const int def_max_period = 8;

const QString DemoFileName = "demo.dmp";
const QString LogFileName = "srv_logs.txt";
const QString ipFileName = "coral.ip";

const s_cli def_cli = {"", false, nullptr, 0};

const static char *all_param_name[] = {"mode=", "bind=", "serial="};
uint16_t bPort = DEF_PORT_NUMBER;
QString sPort = def_serial_port;
int sSpeed = DEF_SPEED;

//--------------------------------------------------------------------------------
void LogSave(const char *func, QString st, bool pr)
{

    if (st.length() > 0) {
        QFile fil(LogFileName);
        QIODevice::OpenMode md = QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text;
        if (fil.open(md )) {
            QString fw;
            if (pr) {
                struct timeval tvl;
                gettimeofday(&tvl, nullptr);
                time_t it_ct = tvl.tv_sec;//QDateTime::currentDateTime().toTime_t();
                struct tm *ctimka = localtime(&it_ct);
                fw.sprintf("%02d.%02d %02d:%02d:%02d.%03d | ",
                            ctimka->tm_mday,
                            ctimka->tm_mon+1,
                            //ctimka->tm_year+1900,
                            ctimka->tm_hour,
                            ctimka->tm_min,
                            ctimka->tm_sec,
                            static_cast<int>(tvl.tv_usec/1000));
            }
            if (func) {
                fw.append("[");
                fw.append(func);
                fw.append("] ");
            }

            fw.append(st);
            if (fw.at(fw.length() - 1) != '\n') fw.append("\n");

            fil.write(fw.toLocal8Bit(), fw.length());
            fil.close();
        }
    }

}
//-----------------------------------------------------------------------
void parse_param_start(char *param)
{
char *uk = nullptr, *uki = nullptr;

    uk = strstr(param, all_param_name[0]);//mode
    if (uk) {
        uk += strlen(all_param_name[0]);
        if (!strcmp(uk, "demo")) demos = true;
    } else {
        uk = strstr(param, all_param_name[1]);//tcp_bind_port
        if (uk) {
            uk += strlen(all_param_name[1]);
            bPort = static_cast<uint16_t>(atoi(uk));
        } else {
            uk = strstr(param, all_param_name[2]);//serial_port:speed
            if (uk) {
                uk += strlen(all_param_name[2]);
                uki = strchr(uk, ':');
                if (uki) {
                    sSpeed = atoi(uki + 1);
                    *uki = '\0';
                    if (sSpeed < DEF_SPEED) sSpeed = DEF_SPEED;
                }
                if (strlen(uk) > 0) {
                    sPort.clear();
                    sPort.append(uk);
                }
            }
        }
    }

}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
MainWindow::TheError::TheError(int err) { code = err; }// Конструктор класса ошибок
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int MainWindow::readserdmp()
{
int ret = 0, i = 0;
quint8 eop = 0xee;//238;

    buf.clear();

    if (!demos || !dmpFile) return ret;

    if (!dmpFile->isOpen()) dmpFile->open(QIODevice::ReadOnly);
    if (!dmpFile->isOpen()) return ret;

    while (true) {
        i++;
        if (dmpFile->atEnd()) {
            dmpFile->close();
            dmpFile->open(QIODevice::ReadOnly);
        }
        buf += dmpFile->read(1);
        if ( buf.contains(static_cast<char>(eop))) break;
                                              else if (i > 22) break;
    }

    return  buf.length();
}
//-----------------------------------------------------------------------
bool MainWindow::GetIpFile()
{
qint64 dl = 0;
const size_t lens = 256;
QString qstx;

    ip_adr.clear();

    char *stx = static_cast<char *>(calloc(1, lens));
    if (stx) {
        QFile fil(ipFileName);
        if (fil.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!fil.atEnd()) {
                memset(stx, 0, lens);
                dl = fil.readLine(stx, lens - 1);
                if (dl > 0) {
                    if (strchr(stx,'\n')) *(stx + dl - 1) = 0;
                    if (stx[0] != '@') {
                        if (stx[0] != ';') {
                            if (ip_adr.length() < MAX_CLIENTS) {
                                qstx.clear();
                                qstx.append(stx);
                                ip_adr << qstx;
                            } else break;
                        }
                    } else break;
                }
            }
            fil.close();
        }
    }

    return (ip_adr.length() ? true : false);
}
//-----------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, uint16_t bp, QString sp, int ss) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    startTime = time(nullptr);

    this->setWindowIcon(QIcon("png/main.png"));
    this->setFixedSize(this->size());
    if (demos)
        this->setWindowTitle("CoralPlus server ver." + ver + " | demo mode");
    else
        this->setWindowTitle("CoralPlus server ver." + ver);

    ui->cli1->hide(); ui->cli2->hide();

    serial_speed = ss;
    serial_port = sp;
    bind_port = bp;
    ku.clear();

    tcpServer = nullptr;
    server_status = 0;

    dmpFile = nullptr;
    sobj = nullptr;
    MyError = 0;
    cnt = 0;
    ip_adr.clear();
    all_cli.clear();
    buf.clear();
    max_period = def_max_period;

    QString temp = "Start server with :\n\tbind_port " + QString::number(bind_port, 10) + "\n"
                    + "\tserial_port " + serial_port + " "
                    + QString::number(serial_speed, 10) + " 8N1";
    LogSave(__func__, temp, true);

    temp.clear();
    if (GetIpFile()) {
        temp.append("Client's address : ");
        int i = 0;
        while (i < ip_adr.length()) {
            cli = def_cli;
            cli.ip_adr = ip_adr[i];
            if (i > 0) temp.append(" , ");
            temp.append("'" + ip_adr[i] + "'");
            all_cli << cli;
            i++;
        }
        ui->status->setText(temp);
    } else {
        temp = "No present client's address in " + ipFileName;
        MyError |= 1;//ip_adr read from 'coral.ip' error
    }
    ui->status->setText(temp);
    LogSave(__func__, temp, true);

    if (MyError) throw TheError(MyError);

    connect(ui->actionRelease, SIGNAL(triggered(bool)), this, SLOT(slot_Release()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(slot_About()));
    connect(ui->actionCOM_and_Client, SIGNAL(triggered(bool)), this, SLOT(slotComCli()));

    tmr = startTimer(1000);
    if (tmr <= 0) {
        MyError |= 2;//timer error
        throw TheError(MyError);
    }
    tmr_msec = startTimer(250);
    if (tmr_msec <= 0) {
        MyError |= 2;//timer error
        throw TheError(MyError);
    }


    if (demos) {//demo mode
        dmpFile = new QFile(DemoFileName);
        if (dmpFile) {
            connect(this, SIGNAL(sigDemoGetData()), this, SLOT(slotDemoGetData()));
            dmpFile->open(QIODevice::ReadOnly);
        }
    } else {//working mode
        sobj = new QSerialPort();
        if (sobj) {
            sobj->setPortName(serial_port);
            sobj->setBaudRate(serial_speed);
            sobj->setDataBits(QSerialPort::Data8);
            sobj->setFlowControl(QSerialPort::NoFlowControl);
            sobj->setParity(QSerialPort::NoParity);
            sobj->setStopBits(QSerialPort::OneStop);
            sobj->setReadBufferSize(18);//18
            sobj->open(QIODevice::ReadOnly);
            sobj->flush();
            sobj->clear(QSerialPort::Input);
            QObject::connect(sobj, &QSerialPort::readyRead, this, &MainWindow::sReadyRead);
            QObject::connect(sobj, &QSerialPort::errorOccurred, this, &MainWindow::sError);
            QObject::connect(&serial_timer, &QTimer::timeout, this, &MainWindow::sTimeout);
            serial_timer.start(DEF_TIMEOUT);
        } else {
            MyError |= 0x10;//make serial object error
            throw TheError(MyError);
        }
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer) {
        MyError |= 4;//create server object error - no memory
        throw TheError(MyError);
    }
    temp = "TCP Server start, listen port " + QString::number(bind_port, 10);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newuser()));
    if (!tcpServer->listen(QHostAddress("0.0.0.0"), bind_port) && !server_status) {
        temp.clear();
        temp.append("Unable to start the server : " + tcpServer->errorString() + "\n");
        MyError |= 8;//Unable to start tcp server
        throw TheError(MyError);
    } else {
        server_status = 1;
        total_pack = 0;
    }
    ui->status->setText(temp);
    LogSave(__func__, temp, true);

    connect(this, SIGNAL(sigSendPack()), this, SLOT(slotSendPack()));

    ui->cat->setText("Wait new packet ...");

}
//-----------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    if (dmpFile) {
        if (dmpFile->isOpen()) dmpFile->close();
        delete dmpFile;
    }
    if (sobj) {
        serial_timer.stop();
        if (sobj->isOpen()) {
            sobj->flush();
            sobj->close();
        }
        delete sobj;
    }

    int i = 0;
    while (i < all_cli.length()) {
        cli = all_cli.at(i);
        if (cli.soc) cli.soc->close();
        i++;
    }

    if (tcpServer) delete tcpServer;

    if (tmr_msec) killTimer(tmr_msec);
    if (tmr) killTimer(tmr);

    LogSave(__func__, "Stop server", true);

    delete ui;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::sReadyRead()
{
int i = 0;
bool yes = false;
quint8 eop = 238;

    while (true) {
        i++;
        buf += sobj->read(1);
        if ( buf.contains( static_cast<char>(eop) ) ) {
            yes = true;
            break;
        } else if (i > 11) break;
    }

    if (!serial_timer.isActive()) serial_timer.start(DEF_TIMEOUT);

    if (yes) {
        //
        ku.enqueue(buf);
        //
        QString tmp = buf.toHex().toUpper();
        buf.clear();
        total_pack++;
        ui->packet->setText(tmp);
        ui->cat->setText("Getting packet # " + QString::number(total_pack, 10) + " done");
        emit sigSendPack();
    }

}
//------------------------------------------------------------------------------
void MainWindow::sTimeout()
{
    if (!serial_timer.isActive()) serial_timer.start(DEF_TIMEOUT);
}
//------------------------------------------------------------------------------
void MainWindow::sError(QSerialPort::SerialPortError code_sError)
{
    if (code_sError == QSerialPort::ReadError) {
        QString stx = tr("Error reading data from '%1' -> '%2'").arg(sobj->portName()).arg(sobj->errorString());
        ui->status->setText(stx);
        LogSave(__func__, stx, true);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MainWindow::cli_prn(s_cli *sc)
{
void *uk = sc->soc;
char stx[128] = {0};
QString tmp = "ip_adr:" + sc->ip_adr;

    sprintf(stx," connected: %u socket:%p peerPort:%u", sc->connected, uk, sc->peerPort);
    tmp.append(stx);

    LogSave(__func__, tmp, true);

}
//------------------------------------------------------------------------------
void MainWindow::slotSendPack()
{
int i = 0;
bool yes = false;
QByteArray bf;

    if (!ku.isEmpty()) bf = ku.dequeue(); else return;

    while (i < all_cli.length()) {
        cli = all_cli.at(i++);
        if ((cli.connected) && (cli.soc)) {
            cli.soc->write(bf);
            yes = true;
        }
    }
    if (yes) ui->cat->setText("Sending packet # " + QString::number(total_pack, 10) + " done");

}
//------------------------------------------------------------------------------
void MainWindow::slotDemoGetData()
{
    ui->cat->setText("Getting new packet ...");
    if (readserdmp()) {
        //
        ku.enqueue(buf);
        //
        QString tmp = buf.toHex().toUpper();
        buf.clear();
        total_pack++;
        ui->packet->setText(tmp);
//        LogSave(__func__, "Packet : " + tmp, true);
        ui->cat->setText("Getting packet # " + QString::number(total_pack, 10) + " done");
        emit sigSendPack();
    }
}
//-----------------------------------------------------------------------
void MainWindow::newuser()
{
    if (server_status) {
        QTcpSocket *cliSocket = tcpServer->nextPendingConnection();
        int fd = static_cast<int>(cliSocket->socketDescriptor());
        CliUrl = cliSocket->peerAddress().toString();// + ":" + QString::number(cliSocket->peerPort(), 10);
        int i = 0, find = 0;
        while (i < all_cli.length()) {
            cli = def_cli;
            cli = all_cli.at(i);
            if (CliUrl == cli.ip_adr) {
                if (!cli.connected) {
                    cli.peerPort = cliSocket->peerPort();
                    cli.connected = true;
                    cli.soc = cliSocket;
                    all_cli.replace(i, cli);
                    find = 1;
                    break;
                } else {
                    find = 2;
                    break;
                }
            }
            i++;
        }
        QString stx;
        switch (find) {
            case 0 :
                cliSocket->close();
                stx = "Unknown client " + CliUrl;
                break;
            case 1:
                if (!i) {
                    ui->cli1->show();
                    ui->ip1->setText(cli.ip_adr + ":" + QString::number(cli.peerPort));
                    ui->ip1->setAlignment(Qt::AlignCenter);
                } else {
                    ui->cli2->show();
                    ui->ip2->setText(cli.ip_adr + ":" + QString::number(cli.peerPort));
                    ui->ip2->setAlignment(Qt::AlignCenter);
                }
                stx.append("New client '" + CliUrl + "' online, socket " + QString::number(fd, 10));
                connect(this, SIGNAL(sigCliDone(QTcpSocket *, int)), this, SLOT(slotCliDone(QTcpSocket *, int)));
                connect(cli.soc, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotErrorClient(QAbstractSocket::SocketError)));
                break;
            default:
                cliSocket->close();
                stx = "Client from this ip " + CliUrl + "already connected ";
                break;
        }
        ui->status->setText(stx);
        LogSave(__func__, stx, true);
    }
}
//------------------------------------------------------------------------------
void MainWindow::slotCliDone(QTcpSocket *csoc, int prn)
{
int fd = static_cast<int>(csoc->socketDescriptor());

    int i = 0, find = 0;
    while (i < all_cli.length()) {
        cli = all_cli.at(i);
        if (cli.soc == csoc) {
            cli.soc = nullptr;
            cli.peerPort = 0;
            cli.connected = false;
            all_cli.replace(i, cli);
            find = 1;
            break;
        }
        i++;
    }

    if (csoc->isOpen() && find) {
        if (prn) {
            QString stx = "Close client, socket " + QString::number(fd, 10) + ".\n";
            ui->status->setText(stx);
            LogSave(__func__, stx, true);
        }
        disconnect(csoc);
        csoc->close();
        if (!i) {
            ui->cli1->hide();
            ui->ip1->setText("Client 1 closed");
            ui->ip1->setAlignment(Qt::AlignCenter);
        } else {
            ui->cli2->hide();
            ui->ip2->setText("Client 2 closed");
            ui->ip2->setAlignment(Qt::AlignCenter);
        }
    }


}
//-----------------------------------------------------------------------
void MainWindow::slotErrorClient(QAbstractSocket::SocketError SErr)
{
QTcpSocket *cliSocket = reinterpret_cast<QTcpSocket *>(sender());
QString qs = "Socket ERROR (" + QString::number(static_cast<int>(SErr), 10) + ") : " + cliSocket->errorString();

    QString stx = "Close client, socket " + QString::number(cliSocket->socketDescriptor(), 10) + ". " + qs + "\n";
    ui->status->setText(stx);
    LogSave(__func__, stx, true);

    emit sigCliDone(cliSocket, 0);
}
//------------------------------------------------------------------------------
QString MainWindow::sec_to_time(time_t &sec)
{
QString ret;
long day = 0, min = 0, hour = 0, seconda = 0;

    day = sec / (60 * 60 * 24);
    sec = sec % (60 * 60 * 24);

    hour = sec / (60 * 60);
    sec = sec % (60 * 60);

    min = sec / (60);
    sec = sec % 60;

    seconda = sec;

    ret.sprintf("%lu.%02lu:%02lu:%02lu", day, hour, min, seconda);

    return ret;
}
//-----------------------------------------------------------------------
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (tmr_msec == event->timerId()) {
        if (demos) {
            cnt++;
            if (cnt == max_period) {
                cnt = 0;
                max_period = rand() % def_max_period + 1;
                emit sigDemoGetData();
            }
        }
    } else if (tmr == event->timerId()) {
        time_t ttm = time(nullptr);
        struct tm *tim = localtime(&ttm);
        QString stk;
        stk.sprintf("%02d.%02d.%02d  %02d:%02d:%02d  -  ",
                tim->tm_mday, tim->tm_mon + 1, tim->tm_year + 1900 - 2000,
                tim->tm_hour, tim->tm_min, tim->tm_sec);
        ttm -= startTime;
        ui->date->setText(stk + sec_to_time(ttm));
        ui->date->setAlignment(Qt::AlignCenter);
    }
}
//-----------------------------------------------------------------------
void MainWindow::slot_Release()
{
    QMessageBox::information(this,
                             "Info",
                             "\nCoral Plus server\nVersion " + ver
                             + "\nBind tpc port " + QString::number(bind_port, 10)
                             + "\nSerial port '" + serial_port + " " + QString::number(serial_speed, 10) + " 8N1"
                             + "'\nBuild #" + BUILD
                             + "\nQt framework version " + QT_VERSION_STR);
}
//-----------------------------------------------------------------------
void MainWindow::slot_About()
{
    QMessageBox::information(this, "Help", "\nSorry, no help present. (:\n");
}
//-----------------------------------------------------------------------------------
void MainWindow::slotComCli()
{
    QString temp("Client's address : ");
    int i = 0;
    while (i < ip_adr.length()) {
        if (i > 0) temp.append(" , ");
        temp.append("'" + ip_adr[i++] + "'");
    }
    temp.append("\n");
    QMessageBox::information(this, "Info", "\n\nBind tcp port : " + QString::number(bind_port, 10)
                             + "\n\nData Traffic serial port : '" + serial_port
                             + " " + QString::number(serial_speed, 10) + " 8N1'\n\n" + temp);
}
//-----------------------------------------------------------------------------------

