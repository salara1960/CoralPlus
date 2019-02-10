#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------

//const QString ver = "1.1 ";//30.01.2019
//const QString ver = "1.2 ";//31.01.2019
//const QString ver = "1.3 ";//31.01.2019
//const QString ver = "1.4 rc2";//01.02.2019 //insert calculator class
//const QString ver = "1.4 rc3";//02.02.2019 //add global.h file
//const QString ver = "1.4 rc4";//03.02.2019 //add configuration's file (coral.npl) parser
//const QString ver = "1.4 rc5";//03.02.2019 //add configuration's file (coral.trk) parser
//const QString ver = "1.5 rc1";//04.02.2019 //add network client
//const QString ver = "1.6 rc1";//05.02.2019 // use qmap class for adress of TheWin class object
//const QString ver = "1.6 rc2";//05.02.2019 // minor changes
//const QString ver = "1.7 rc1";//06.02.2019 // minor changes in reconnect to server
const QString ver = "1.8";//06.02.2019 // working release


QString ServerIPAddress = "127.0.0.1:6543";
QString CfgFil = "coral.npl";
QString TrkFil = "coral.trk";
QString myPwd;
const QString _myPwd = "zero";
const QString def_srv_adr = "127.0.0.1";
const int def_srv_port = 6543;
const int wTime = 10;

const static char *all_param_name[] = {"serverip=", "cfgfile=", "admin="};

const uint8_t KolType = 7;
const static char *NamePortType[] = {"SLT:", "KEY:", "TRK:", "GRP:", "MOD:", "CON:", "BAD:"};
const QString Name2PortType[] = {"SltSet", "KeySet", "Trunk", "TkGp", "Modem", "Conf", "Bad"};

//const uint8_t KolStatus = 4;
const QString PortStatus[] = {"", "Busy", "Lock", ""};
const QString Name2PortStatus[] = {"Idle", "Busy", "Lock", ""};

const int wPort = 74;
const int hPort = 46;
const int Lis = 0;//1;
const int PortsPerLine = 12;
const int LinePerPage = 12;
const int PortPerPage = PortsPerLine * LinePerPage; //144

const s_one def_port = {"2000", SLT_TYPE, IDLE_STATUS, "", {}};

const QString LogFileName = "cli_logs.txt";
//-----------------------------------------------------------------------
void parse_param_start(char *param)
{
char *uk = nullptr;

    uk = strstr(param, all_param_name[0]);
    if (uk) {
        uk += strlen(all_param_name[0]);
        ServerIPAddress.clear();
        ServerIPAddress.append(uk);
    } else {
        uk = strstr(param, all_param_name[1]);
        if (uk) {
            uk += strlen(all_param_name[1]);
            CfgFil.clear();
            CfgFil.append(uk);
        } else {
            uk = strstr(param, all_param_name[2]);
            if (uk) {
                uk += strlen(all_param_name[2]);
                myPwd.clear();
                myPwd.append(uk);
            }
        }
    }

}
//--------------------------------------------------------------------------------
void LogSave(const char *func, QString st, bool pr)
{

    if (st.length() > 0) {
        QFile fil(LogFileName);
        QIODevice::OpenMode md = QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text;
        if (fil.open(md )) {
            QString fw;
            if (pr) {
                time_t it_ct = QDateTime::currentDateTime().toTime_t();
                struct tm *ctimka = localtime(&it_ct);
                fw.sprintf("%02d.%02d %02d:%02d:%02d | ",
                            ctimka->tm_mday,
                            ctimka->tm_mon+1,
                            //ctimka->tm_year+1900,
                            ctimka->tm_hour,
                            ctimka->tm_min,
                            ctimka->tm_sec);
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
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
MainWindow::TheError::TheError(int err) { code = err; }// Конструктор класса ошибок
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
TheWin::TheWin(QWidget *parent, QRect *rc,  s_one *ops)
{
    rect = *rc;
    one = *ops;
    obj = new QTextEdit(parent);
    if (obj) {
        obj->setGeometry(rect);
        QFont font = parent->font();
        obj->setStyleSheet(QString::fromUtf8("background-color: rgb(200, 200, 200);\ncolor: rgb(0, 0, 0);"));

        if (MAX_DIGIT_PORT > 6)
            font.setPixelSize(13);
        else
            font.setPixelSize(14);

        obj->setFont(font);
        obj->setContextMenuPolicy(Qt::NoContextMenu);
        obj->setFrameShape(QFrame::Box);
        obj->setFrameShadow(QFrame::Raised);
        obj->setLineWidth(2);
        obj->setMidLineWidth(0);
        obj->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        obj->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        obj->setReadOnly(true);
        if (one.status != NONE_STATUS)
            obj->setHtml(one.port + "<br>" + PortStatus[one.status]);
        else
            obj->setHtml(one.port + "<br>" + one.two);

        QString tTip = " " + Name2PortType[one.type] + " ";
        if (one.trunks.length() > 0) {
            tTip.append(":");
            QStringList::iterator it(one.trunks.begin());
            while (it != one.trunks.end()) tTip.append("\n  " + *it++);
        }
        obj->setToolTip(tTip);
        obj->setAlignment(Qt::AlignCenter);
        if (one.type == BAD_TYPE) obj->hide();
    }
}
//--------------------------------------------------------------------------------
TheWin::~TheWin()
{
    if (obj) delete obj;
}
//--------------------------------------------------------------------------------
void TheWin::_update(s_one *ops)
{
    one = *ops;
    this->obj->clear();

    if (!one.two.length())
        obj->setHtml(one.port + "<br>" + PortStatus[one.status]);
    else
        obj->setHtml(one.port + "<br>" + one.two);

    this->obj->setAlignment(Qt::AlignCenter);
}
//--------------------------------------------------------------------------------
void TheWin::show_win(bool f)
{
    if (f) obj->show(); else obj->hide();
}
//--------------------------------------------------------------------------------
QString TheWin::get_port()
{
    return one.port;
}
//--------------------------------------------------------------------------------
uint8_t TheWin::get_type()
{
    return one.type;
}
//--------------------------------------------------------------------------------
uint8_t TheWin::get_status()
{
    return one.status;
}
//--------------------------------------------------------------------------------
QString TheWin::get_two()
{
    return one.two;
}
//--------------------------------------------------------------------------------
s_one TheWin::get_all()
{
    return one;
}
//--------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon("png/main.png"));
    this->setFixedSize(this->size());
    this->setWindowTitle("CoralPlus client ver." + ver);

    ico_black = new QIcon("png/BlackTag.png");
    ico_green = new QIcon("png/GreenTag.png");

    QFont font = this->font();
#ifdef WIN32
    //font.fromString(QString::fromUtf8("font: 12pt Sans Serif;"));
    font.setPixelSize(14);//15
    ui->l_busy->setFont(font);
    ui->l_all_ports->setFont(font);
    ui->l_date_time->setFont(font);
    ui->l_data->setFont(font);
    font.setPixelSize(16);//18
    ui->menu->setFont(font);
    ui->menuBar->setFont(font);
    ui->menu_2->setFont(font);
    ui->menu_3->setFont(font);
    ui->menu_4->setFont(font);
#else
    /*
    //font.fromString(QString::fromUtf8("font: 12pt Sans Serif;"));
    font.setPixelSize(16);//15
    ui->l_busy->setFont(font);
    ui->l_all_ports->setFont(font);
    ui->l_date_time->setFont(font);
    ui->l_data->setFont(font);
    //font.setPixelSize(16);//18
    //ui->menu->setFont(font);
    //ui->menuBar->setFont(font);
    //ui->menu_2->setFont(font);
    //ui->menu_3->setFont(font);
    //ui->menu_4->setFont(font);
    */
#endif

    ui->l_all_ports->setToolTip("MaxPort "+QString::number(max_adr, 10));
    _pSocket = nullptr;
    mv = nullptr;
    Dlg = nullptr;
    Calc = nullptr;
    beginSrv = conTrue = false;
    tmr_wait = total_ports = 0;
    try_cnt = cntWait = 0;
    errFirst = startWait = false;
    AdminFlag = false;
    max_bad_len = 4;
    indexWin.clear();
    AllBusyCount = MaxBusyCount = 0;

    SplitAdrPort(&ServerIPAddress);
    if (myPwd == _myPwd) AdminFlag = true;
    sbar = "Server : " + ServerIPAddress + " ConfigFile : " + CfgFil + " User : " + myPwd;
    statusBar()->showMessage(sbar);
    qApp->installEventFilter(this);

    time_start = QDateTime::currentDateTime().toTime_t();

    connect(ui->actionRelease, SIGNAL(triggered(bool)), this, SLOT(slot_Release()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(slot_About()));

    connect(this, SIGNAL(sigShowHideAdminMenu(bool)), this, SLOT(slotShowHideAdminMenu(bool)));
    emit sigShowHideAdminMenu(!AdminFlag);


    connect(ui->actionEnable_menu, SIGNAL(triggered(bool)), this, SLOT(slotAdminOn()));
    connect(ui->actionDisable_menu, SIGNAL(triggered(bool)), this, SLOT(slotAdminOff()));
    connect(ui->actionCalc, SIGNAL(triggered(bool)), this, SLOT(slotMakeCalc()));

    //---------------------------------------------------------------------------
    alls = new QList<s_one>;
    groups = new QList<s_tkgp>;

    GetConfigFile();

    InitScreen();
    connect(ui->win_area, SIGNAL(currentChanged(int) ), this, SLOT(slot_page(int)));

    delete alls; alls = nullptr;
    delete groups; groups = nullptr;
    //---------------------------------------------------------------------------

    ui->run->hide();
    mv = new QMovie("png/move.gif");
    ui->run->setMovie(mv);
    mv->start();

    tmr = startTimer(1000);
    if (tmr <= 0) {
        MyError |= 2;//timer error
        throw TheError(MyError);
    }

    _pSocket = new QTcpSocket(this);
    if (!_pSocket) {
        MyError |= 0x20;//create socket error - no memory
        throw TheError(MyError);
    }
    connect(_pSocket, SIGNAL(connected()), this, SLOT(connectTcp()));
    connect(_pSocket, SIGNAL(disconnected()), this, SLOT(disconnectTcp()));
    connect(_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(SokError(QAbstractSocket::SocketError)));
    connect(_pSocket, SIGNAL(readyRead()), this, SLOT(readData()));

    QObject::connect(this, &MainWindow::sigNewCon, this, &MainWindow::slotNewCon);
    QObject::connect(this, &MainWindow::sigPackParser, this, &MainWindow::slotPackParser);

    LogSave(nullptr, "Start program", true);

    emit sigNewCon();

}
//-----------------------------------------------------------------------
MainWindow::~MainWindow()
{
    if (tmr_wait > 0) killTimer(tmr_wait);
    if (tmr > 0) killTimer(tmr);

    if (_pSocket) {
        if (_pSocket->isOpen()) _pSocket->close();
        delete _pSocket;
    }

    if (mv) {
        if (mv->Running) mv->stop();
        delete mv;
    }

    if (ico_green) delete ico_green;
    if (ico_black) delete ico_black;
    if (Calc) delete Calc;

    for (int i = 0; i < indexWin.length(); i++) {
        if (indexWin[i] != nullptr) delete indexWin[i];
    }

    LogSave(nullptr, "Stop program\n", true);

    delete ui;
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
//--------------------------------------------------------------------------------
void MainWindow::clearAllPort()
{
    s_one one;
    TheWin *pad = nullptr;
    QVector<TheWin *>::iterator it = indexWin.begin();
    while (it != indexWin.end()) {
        pad = *it++;
        if (pad) {
            one = pad->get_all();
            one.two.clear();
            one.status = IDLE_STATUS;
            pad->_update(&one);
        }
    }
    AllBusyCount = MaxBusyCount = 0;

}
//--------------------------------------------------------------------------------
void MainWindow::SokError(QAbstractSocket::SocketError SocketError)
{
QString qs;

    qs.sprintf("[%d] Socket ERROR (%d) : server ", try_cnt, SocketError);
    qs.append(srv_adr);
    qs.append(" <- ");
    if (_pSocket) qs.append(_pSocket->errorString());
    disconnectTcp();

}
//--------------------------------------------------------------------------------
void MainWindow::connectTcp()
{
    ui->run->show();
    ui->l_data->clear();
    ui->l_data->setAlignment(Qt::AlignLeft);

    sbar.sprintf("#%d Connected to server ", try_cnt);
    if (_pSocket) sbar += _pSocket->peerName() + ":" + QString::number(_pSocket->peerPort());
    statusBar()->showMessage(sbar);

    LogSave(__func__, sbar, true);

    conTrue = true;
    cntWait = 0;

    if (tmr_wait > 0) killTimer(tmr_wait);
}
//--------------------------------------------------------------------------------
void MainWindow::disconnectTcp()
{
    if (conTrue) clearAllPort();
    conTrue = false;

    if (_pSocket) {
        //_pSocket->abort();
        _pSocket->close();
    }

    sbar = "Disconnect from server " + ServerIPAddress + ". Try #"+QString::number(try_cnt, 10)+" connect after " + QString::number(wTime, 10) + " sec";
    statusBar()->showMessage(sbar);

    ui->l_data->clear();
    if (ui->run->isVisible()) ui->run->hide();//hide running cat

    LogSave(__func__, sbar, true);

    if (!tmr_wait) {
        cntWait = 0;
        tmr_wait = startTimer(wTime * 1000);//10000 msec
        if (tmr_wait <= 0) {
            MyError |= 2;//timer error
            throw TheError(MyError);
        }
        LogSave(__func__, "Start tmr_wait timer (wait " + QString::number(wTime, 10) + " sec.)", true);
    }
}
//-----------------------------------------------------------------------
void MainWindow::slotNewCon()
{
    try_cnt++;
    cntWait = 0;
    if (tmr_wait > 0) killTimer(tmr_wait);

    sbar = "Try #" + QString::number(try_cnt, 10) +" connect to Server " + ServerIPAddress + " ...";
    statusBar()->showMessage(sbar);

    if (_pSocket) {
        if (_pSocket->isOpen()) _pSocket->close();
        _pSocket->connectToHost(srv_adr, static_cast<uint16_t>(srv_port));
        rxdata.clear();
        txdata.clear();
    }

    LogSave(__func__, sbar, true);
}
//-----------------------------------------------------------------------
void MainWindow::readData()
{
quint8 eop = 0xee;//238;

    if (_pSocket) {
        rxdata.clear();
        while (true) {
            rxdata += _pSocket->read(1);
            if (rxdata.contains(eop)) break;
        }
        emit sigPackParser(rxdata);
    }
}
//-----------------------------------------------------------------------
uint32_t my_htonl(uint32_t in)
{
uint32_t res = in;
uint8_t *uk;
uint8_t b1, b2;

    res = (res >> 16) | (res << 16);
    uk = (uint8_t *)&res;
    b1 = *uk; b2 = *(uk+1);
    *uk = b2; *(uk+1) = b1;
    b1 = *(uk+2); b2 = *(uk+3);
    *(uk+2) = b2; *(uk+3) = b1;

    return res;
}
//-----------------------------------------------------------------------
void MainWindow::slotPackParser(QByteArray &pack)
{
QByteArray pk(pack);
uint8_t *uk = nullptr;
uint32_t dword = 0;
uint8_t mas[4] = {0};
char stx[64] = {0};
char sport1[16] = {0};
char sport2[16] = {0};
s_one one, two;
TheWin *pad1, *pad2;
uint8_t byte = static_cast<uint8_t>(pk.at(0));
QString txt = "";//, tp = "";
uint8_t istat;
bool bad_port = false;
int imax = AllBusyCount;


    uk = reinterpret_cast<uint8_t *>(pk.data() + 1);

    switch (byte) {
        case 0x70:
        {
            s_stat *stat = reinterpret_cast<s_stat *>(uk);
            if (stat->port == 0xFFFFFFFF) {
                bad_port = true;
                for (int i = 0; i < max_bad_len; i++) sprintf(sport1+strlen(sport1), "F");
            } else {
                dword = my_htonl(stat->port);
                memcpy(mas, &dword, sizeof(uint32_t));
                for (int i=0; i<4; i++) {
                    if ((mas[i] >> 4) == 0x0F) mas[i] &= 0x0F;
                    if ((mas[i] & 0x0F) == 0x0F) mas[i] &= 0xF0;
                }
                memcpy(&dword, mas, sizeof(uint32_t));
                dword = my_htonl(dword);
                sprintf(sport1, "%X", dword);
            }
            istat = stat->status & 3;
            QString prt(sport1);
            pad1 = map_adr.value(prt);
            if (pad1) {
                one = pad1->get_all();
                if (prt == one.port) {
                    one.two.clear();
                    one.status = istat;
                    pad1->_update(&one);
                }
            }
            sprintf(stx+strlen(stx), "%s %02X", sport1, stat->status);
            txt.append(stx); txt.append(" | " + pk.toHex().toUpper());
            if (!bad_port) {
                if (istat == BUSY_STATUS) {
                    AllBusyCount++;
                    if (AllBusyCount > total_ports) AllBusyCount = total_ports;
                    if (MaxBusyCount < AllBusyCount) MaxBusyCount = AllBusyCount;
                } else if (istat == IDLE_STATUS) {
                    if (AllBusyCount > 0) AllBusyCount--;
                }
            }
        }
        break;
        case 0x75:
        {
            s_conn *conn = reinterpret_cast<s_conn *>(uk);
            if (conn->port1 == 0xFFFFFFFF) {
                for (int i = 0; i < max_bad_len; i++) sprintf(sport1+strlen(sport1), "F");
            } else {
                dword = my_htonl(conn->port1);
                memcpy(mas, &dword, sizeof(uint32_t));
                for (int i=0; i<4; i++) {
                    if ((mas[i] >> 4) == 0x0F) mas[i] &= 0x0F;
                    if ((mas[i] & 0x0F) == 0x0F) mas[i] &= 0xF0;
                }
                memcpy(&dword, mas, sizeof(uint32_t));
                dword = my_htonl(dword);
                sprintf(sport1, "%X", dword);
            }
            //
            if (conn->port1 == 0xFFFFFFFF) {
                for (int i = 0; i < max_bad_len; i++) sprintf(sport2+strlen(sport2), "F");
            } else {
                dword = my_htonl(conn->port2);
                memcpy(mas, &dword, sizeof(uint32_t));
                for (int i=0; i<4; i++) {
                    if ((mas[i] >> 4) == 0x0F) mas[i] &= 0x0F;
                    if ((mas[i] & 0x0F) == 0x0F) mas[i] &= 0xF0;
                }
                memcpy(&dword, mas, sizeof(uint32_t));
                dword = my_htonl(dword);
                sprintf(sport2, "%X", dword);
            }

            QString pt1(sport1);
            QString pt2(sport2);
            pad1 = map_adr.value(pt1);
            pad2 = map_adr.value(pt2);
            if ((pad1) && (pad2)) {
                one = pad1->get_all();
                two = pad2->get_all();
                if (pt1 == one.port) {
                    one.two = pt2;
                    one.status = conn->status & 3;
                    pad1->_update(&one);
                    if (pt2 == two.port) {
                        two.two = pt1;
                        two.status = conn->status & 3;
                        pad2->_update(&two);
                    }
                }
            }
            sprintf(stx, "%s-%s %02X", sport1, sport2, conn->status);
            txt.append(stx); txt.append(" | " + pk.toHex().toUpper());
        }
        break;
        case 0x79:
            txt = pk.toHex().toUpper();
        break;
        case 0x7B:
            txt = pk.toHex().toUpper();
        break;
        default:
            txt = pk.toHex().toUpper();
            break;
    }

    ui->l_data->setText(txt);
    if (imax != AllBusyCount)
        ui->l_busy->setText(QString::number(AllBusyCount, 10) + "/" + QString::number(MaxBusyCount, 10));
}
//-----------------------------------------------------------------------
bool MainWindow::GetConfigFile()
{
bool ret = false;
int i, j, istart, istop, ind = 0;
uint8_t itype, it;
char sstart[MAX_DIGIT_PORT<<1] = {0};
char sstop[MAX_DIGIT_PORT<<1] = {0};
char *uks = nullptr, *uk = nullptr;
const char sep = '-';
s_one def_one = {"", BAD_TYPE, NONE_STATUS, "", {}};
s_one one;
qint64 dl;
s_tkgp def_tkgp = {"", {}};
s_tkgp tkgp = def_tkgp;
QString st;
int id = -1, igrp = 0;


    if (!alls) return ret;

    char *stx = static_cast<char *>(calloc(1, 256));
    if (!stx) return ret;

    //------------------------   get tkgp_trunks   ---------------------------
    if (groups) {
        QFile tfil(TrkFil);
        if (tfil.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!tfil.atEnd()) {
                memset(stx, 0, 256);
                dl = tfil.readLine(stx, 255);
                if ((dl > 0) && (dl < 256)) {
                    if (strchr(stx,'\n')) *(stx + dl - 1) = 0;
                    if (stx[0] != '@') {
                        if (stx[0] == '#') {
                            if (tkgp.port.length() > 0) {
                                *groups << tkgp;
                                igrp++;
                            }
                            tkgp = def_tkgp;
                            tkgp.port.append(&stx[1]);
                        } else {
                            st.clear();
                            st.append(stx);
                            tkgp.trunks << st;
                        }
                    } else break;
                }
            }
            if (tkgp.port.length() > 0) {
                *groups << tkgp;
                igrp++;
            }
            tfil.close();
        }
    }

    //------------------------   get npl   -----------------------------------
    QFile fil(CfgFil);
    if (fil.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!fil.atEnd()) {
            itype = BAD_TYPE;
            memset(stx, 0, 256);
            dl = fil.readLine(stx, 255);
            if ((dl > 0) && (dl < 256)) {
                if (strchr(stx,'\n')) *(stx + dl - 1) = 0;
                if (stx[0] == '@') break;
                else if (stx[0] != ';') {
                    uks = nullptr;
                    for (it = 0; it < KolType; it++) {
                        uk = strstr(stx, NamePortType[it]);
                        if (uk) {
                            uks = uk + strlen(NamePortType[it]);
                            itype = it;
                            break;
                        }
                    }
                    if (uks) {
                        uk = strchr(uks, sep);
                        if (uk) {
                            ret = true;
                            one = def_one;
                            memset(sstart, 0, sizeof(sstart));
                            memset(sstop, 0, sizeof(sstop));
                            strcpy(sstop, uk + 1);
                            *uk = '\0';
                            strcpy(sstart, uks);
                            istart = atoi(sstart);
                            istop = atoi(sstop);
                            for (i = istart; i <= istop; i++) {
                                if (itype == BAD_TYPE) {
                                    one.port.append(sstart);
                                    max_bad_len = one.port.length();
                                } else {
                                    one.port = QString::number(i, 10);
                                }
                                one.two.clear();
                                one.type = itype;
                                one.trunks.clear();
                                if (itype == GRP_TYPE) {
                                    if (groups) {
                                        if (groups->length() > 0) {
                                            j = 0; id = -1;
                                            while (j < groups->length()) {
                                                tkgp = groups->at(j);
                                                if (tkgp.port == one.port) {
                                                    id = j;
                                                    break;
                                                }
                                                j++;
                                            }
                                            if (id != -1) one.trunks = tkgp.trunks;
                                        }
                                    }
                                }
                                *alls << one;
                                ind++;
                                if (itype == BAD_TYPE) break;
                            }
                        }//if (uk)
                    }//if (uks)
                }//else if (stx[0] != ';') {
            }
        }
        fil.close();
        if (ret) {
            total_ports = alls->length();
            if (total_ports > max_adr) {
                total_ports = 0;
                ret = false;
            }
        }
    }
    //------------------------------------------------------------------------

    free(stx);

    return ret;
}
//-----------------------------------------------------------------------
void MainWindow::InitScreen()
{
    if (!total_ports) return;

    TheWin *adr = nullptr;
    s_one s_port = def_port;
    _page = 0; _last = 0;
    int left_border = 1;
    int top_border = 0;
    ui->l_all_ports->setText("Ports " + QString::number(total_ports, 10));
    QRect rect = ui->win_area->geometry();
    int x = rect.x();
    int y = rect.y();
    if (ui->win_area->tabPosition() == QTabWidget::North) y += 27;

    for (int i = 0; i < ui->win_area->count(); i++) {
        if (i == _page) ui->win_area->setTabIcon(i, *ico_green);
                   else ui->win_area->setTabIcon(i, *ico_black);
    }
    ui->win_area->setCurrentIndex(_page);

    int w = left_border;
    int h = top_border;
    int dx = 0;
    int dy = 0;
    int sha = 0;
    int clin = 0;
    QString stemp;
    for (int i = 0; i < total_ports; i++) {
        s_port = alls->at(i);

        x += w + dx;
        rect.setX(x);
        rect.setY(y);
        rect.setWidth(wPort);
        rect.setHeight(hPort);
        adr = new TheWin(ui->win_area, &rect,  &s_port);
        if (adr) {
            map_adr.insert(s_port.port, adr);
            indexWin.push_back(adr);
            if ((i >= PortPerPage) && (adr->get_type() != BAD_TYPE)) adr->show_win(false);
        } else {
            stemp = "Error make port_object : port=" + s_port.port + " type=" + Name2PortType[s_port.type] + " status=" + Name2PortStatus[s_port.status];
            LogSave(__func__, stemp, true);
        }
        w = wPort;
        dx = Lis;
        sha++; clin++;
        if (sha == PortsPerLine) {
            sha = 0;
            x = 0;
            w = left_border;
            dx = 0;
            h = hPort;
            dy = Lis;
            y += h + dy;
        }
        if (clin == PortPerPage) {
                clin = 0;
                h = top_border;
                y = top_border;
                if (ui->win_area->tabPosition() == QTabWidget::North) y += 27;
        }
    }
    ui->win_area->setCurrentIndex(_page);

}
//-----------------------------------------------------------------------
void MainWindow::updateBar()
{
    sbar = "Server : " + ServerIPAddress + " ConfigFile : " + CfgFil + " User : " + myPwd;
    statusBar()->showMessage(sbar);
    emit sigShowHideAdminMenu(!AdminFlag);
}
//-----------------------------------------------------------------------
void MainWindow::slotSetPwd(QString pwd)
{
    myPwd = pwd;
    if (myPwd == _myPwd) AdminFlag = true; else AdminFlag = false;
    if (Dlg) delete Dlg;
    Dlg = nullptr;
    updateBar();
}
//-----------------------------------------------------------------------
void MainWindow::slotAdminOn()
{
    if ((AdminFlag) || Dlg) return;

    Dlg = new pwdDialog(nullptr, "Info");
    connect(Dlg, SIGNAL(DoneW(QString)), this, SLOT(slotSetPwd(QString)));
    Dlg->show();
}
//-----------------------------------------------------------------------
void MainWindow::slotAdminOff()
{
    AdminFlag = false;
    myPwd.clear();
    updateBar();
}
//-----------------------------------------------------------------------
void MainWindow::slotShowHideAdminMenu(bool flag)
{
    ui->actionDisable_menu->setDisabled(flag);

    ui->menuSubscriber->setDisabled(flag);
    ui->menuTrunk->setDisabled(flag);
    ui->menuCallForward->setDisabled(flag);
    ui->menuSpecialFeatures->setDisabled(flag);
}
//-----------------------------------------------------------------------
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
QString st = "";

    if(e->buttons() & Qt::RightButton) st.append("Right button pressed\n");
    if(e->buttons() & Qt::LeftButton) st.append("Left button pressed\n");
    if(e->buttons() & Qt::MiddleButton) st.append("Middle button pressed");
    if (st.length() > 0) qDebug() << st;
}
//-----------------------------------------------------------------------
void MainWindow::slot_page(int cp)
{
int i;
int last = _page;
int start_ind = last * PortPerPage;
int stop_ind = start_ind + PortPerPage;
TheWin *pad = nullptr;

    if (cp == _page) return;

    _page = cp;

    for (i = 0; i < ui->win_area->count(); i++) {
        if (i == _page) ui->win_area->setTabIcon(i, *ico_green);
                   else ui->win_area->setTabIcon(i, *ico_black);
    }
    //hide all ports on last page
    if (start_ind < total_ports) {
        for (i = start_ind; i < stop_ind; i++) {
            if (i < total_ports) {
                pad = indexWin[i];
                if (pad) pad->show_win(false);
            } else break;
        }
    }
    //show all ports on last page
    start_ind = _page * PortPerPage;
    stop_ind = start_ind + PortPerPage;
    if (start_ind < total_ports) {
        for (i = start_ind; i < stop_ind; i++) {
            if (i < total_ports) {
                pad = indexWin[i];
                if (pad) {
                    if (pad->get_type() != BAD_TYPE) pad->show_win(true);
                }
            } else break;
        }
    }

}
//-----------------------------------------------------------------------
void MainWindow::slotMakeCalc()
{
    if (Calc) {
        delete Calc;
        Calc = nullptr;
    }
    Calc = new CalcWin();
    Calc->show();
}
//-----------------------------------------------------------------------
void MainWindow::SplitAdrPort(QString *adr)
{
char *tp = nullptr, *uk = nullptr;
QString sp;
QByteArray mas;

    sp.clear();
    mas.clear();

    mas.append(*adr);
    tp = mas.data();
    uk = strchr(tp, ':');
    if (uk) {
        srv_port = atoi(uk + 1);
        sp.append(uk + 1);
        *(uk) = 0;
        srv_adr.clear();
        srv_adr.append(tp);
    } else {
        srv_adr = def_srv_adr;
        srv_port = def_srv_port;
    }
}
//-----------------------------------------------------------------------
bool MainWindow::eventFilter(QObject *obj, QEvent *evt)
{
    if (!obj) return false;
    if (evt->type() == QEvent::StatusTip) return true;
                                     else return false;
}
//-----------------------------------------------------------------------
void MainWindow::timerEvent(QTimerEvent *event)
{

    if (tmr == event->timerId()) {

        QDateTime dt = QDateTime::currentDateTime();
        QString dts(dt.toString("dd.MM.yy hh:mm:ss"));
        time_t tmp = dt.toTime_t() - time_start;
        dts.append(" / " + sec_to_time(tmp));
        ui->l_date_time->setText(dts);

        cntWait++;
        if (!conTrue) ui->l_busy->setText(QString::number(cntWait, 10));

    } else if (tmr_wait == event->timerId()) {

        killTimer(tmr_wait); tmr_wait = 0;
        LogSave(__func__, "Stop tmr_wait timer !", true);
        if (!conTrue) emit sigNewCon();

    }
}
//-----------------------------------------------------------------------
void MainWindow::slot_Release()
{
    QMessageBox::information(this, "Info", "\nCoral Plus client\nVersion " + ver + "\nBuild #" + BUILD + "\nQt framework version " + QT_VERSION_STR + "\n");
}
//-----------------------------------------------------------------------
void MainWindow::slot_About()
{
    QMessageBox::information(this, "Help", "\nSorry, no help present. (:\n");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
