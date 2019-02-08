#include "itDialog.h"
#include "ui_itDialog.h"

//**************************************************************************************
pwdDialog::pwdDialog(QWidget *parent, QString title) : QDialog(parent), uiw(new Ui::pwdDialog)
{
    uiw->setupUi(this);

    this->setWindowIcon(QIcon("png/Key.png"));
    this->setFixedSize(this->size());

    setWindowTitle(title);

    str = "????";
    uiw->line->setEchoMode(QLineEdit::EchoMode::Password);
    uiw->line->setText(str);

    connect(uiw->but_ok, SIGNAL(pressed()), this, SLOT(slotOk()));

}
//-----------------------------------------------
pwdDialog::~pwdDialog()
{
    this->disconnect();
    delete uiw;
}
//-----------------------------------------------
void pwdDialog::slotOk()
{
    str = uiw->line->text();

    emit DoneW(str);
}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************

