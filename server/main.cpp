#include "mainwindow.h"
#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>


int main(int argc, char *argv[])
{
int cerr = 0;
char stz[256] = {0};

    setlocale(LC_ALL,"UTF8");

    if (argc > 1) parse_param_start(argv[1]);
    if (argc > 2) parse_param_start(argv[2]);
    if (argc > 3) parse_param_start(argv[3]);

    try {

        QLocale loc(QLocale::Russian, QLocale::RussianFederation);
        QLocale::setDefault(loc);

        QApplication a(argc, argv);

        QLockFile lockFile(QDir::temp().absoluteFilePath("CoralPlusSrv.lock"));
        if (!lockFile.tryLock(100)){
            QMessageBox::warning(0, "Внимание", "Программа уже запущена");
            LogSave(__func__, "SECOND MAIN STOP", true);
            return 1;
        }

        MainWindow wnd(nullptr, bPort, sPort, sSpeed);
        wnd.show();
        a.exec();
    }
    catch (MainWindow::TheError(er)) {
        memset(stz, 0, sizeof(stz));
        cerr = er.code;
        if ((cerr>0) && (cerr<=0x20)) {
            if (cerr & 1) sprintf(stz, "Error in get client's ip_adr (%d)\n", cerr);//ip_adr read from 'coral.ip' error
            if (cerr & 2) sprintf(stz, "Error in startTimer function (%d)\n", cerr);
            if (cerr & 4) sprintf(stz, "Error create server object (%d)\n", cerr);
            if (cerr & 8) sprintf(stz, "Error starting tcp server (%d)\n", cerr);
            if (cerr & 0x10) sprintf(stz, "Error make serial_port object(%d)\n", cerr);
        }
        if (!strlen(stz)) sprintf(stz, "Unknown Error (%d)\n", cerr);
        perror(stz);

        return cerr;
    }
    catch (std::bad_alloc) {
        perror("Error while alloc memory via call function new\n");
        return -1;
    }

    return 0;
}
