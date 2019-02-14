#include "calc.h"
#include "ui_calc.h"

//---------------------------------------------------------------
CalcWin::CalcWin(QWidget *parent) : QMainWindow(parent), uic(new Ui::CalcWin)
{
    uic->setupUi(this);

    this->setWindowIcon(QIcon("png/icalc.png"));

    uic->displayPanel->clear();

    operatorClicked = false;
    hasStoredNumber = false;

    uic->numberGroup->connect(uic->numberGroup,
                              SIGNAL(buttonClicked(QAbstractButton*)),
                              this,
                              SLOT(numberGroup_clicked(QAbstractButton*)));
    uic->actionGroup->connect(uic->actionGroup,
                              SIGNAL(buttonClicked(QAbstractButton*)),
                              this,
                              SLOT(actionGroup_clicked(QAbstractButton*)));

    this->setFixedSize(QSize(306, 319));
}
//---------------------------------------------------------------
CalcWin::~CalcWin()
{
    delete uic;
}
//---------------------------------------------------------------
void CalcWin::numberGroup_clicked(QAbstractButton* button)
{
    QString displayLabel = uic->displayPanel->text();

    if (operatorClicked) {
        displayLabel.clear();
        operatorClicked = false;
    }

    if (displayLabel.length() >= DIGIT_LIMIT) return;

    displayLabel.append(button->text());

    uic->displayPanel->setText(displayLabel);
}
//---------------------------------------------------------------
void CalcWin::actionGroup_clicked(QAbstractButton* button)
{
    if (operatorClicked) {
        storedOperator = button->text().at(0);
    } else {
        if (hasStoredNumber) {
            calculate_result();
        } else {
            hasStoredNumber = true;
            QString displayLabel = uic->displayPanel->text();
            storedNumber = displayLabel.toDouble();
        }
        operatorClicked = true;
        storedOperator = button->text().at(0);
    }
}
//---------------------------------------------------------------
void CalcWin::on_actionDel_clicked()
{
    QString displayLabel = uic->displayPanel->text();

    if (!displayLabel.length()) return;

    displayLabel.QString::chop(1);

    uic->displayPanel->setText(displayLabel);
}
//---------------------------------------------------------------
void CalcWin::on_actionCalc_clicked()
{
    QString displayLabel = uic->displayPanel->text();

    if (!hasStoredNumber || (displayLabel.length() < 1) || operatorClicked) return;

    calculate_result();

    hasStoredNumber = false;
}
//---------------------------------------------------------------
void CalcWin::on_comma_clicked()
{
    QString displayLabel = uic->displayPanel->text();

    if (displayLabel.length() >= (DIGIT_LIMIT - 1) || displayLabel.contains('.', Qt::CaseSensitive)) return;

    if (!displayLabel.length()) displayLabel = "0";

    displayLabel.append('.');
    uic->displayPanel->setText(displayLabel);
}
//---------------------------------------------------------------
void CalcWin::on_actionClear_clicked()
{
    uic->displayPanel->clear();
    operatorClicked = hasStoredNumber = false;
}
//---------------------------------------------------------------
void CalcWin::on_actionPercent_clicked()
{
    QString displayLabel = uic->displayPanel->text();
    double percentage = displayLabel.toDouble();
    percentage *= 0.01;
    displayLabel = QString::number(percentage, 'g', DIGIT_LIMIT);
    uic->displayPanel->setText(displayLabel);
}
//---------------------------------------------------------------
void CalcWin::on_actionSign_clicked()
{
    QString displayLabel = uic->displayPanel->text();
    double percentage = displayLabel.toDouble();
    percentage *= -1;
    displayLabel = QString::number(percentage,'g', DIGIT_LIMIT);
    uic->displayPanel->setText(displayLabel);
}
//---------------------------------------------------------------
void CalcWin::calculate_result()
{
    QString displayLabel = uic->displayPanel->text();

     if (displayLabel.endsWith('.', Qt::CaseSensitive)) displayLabel.QString::chop(1);

     if (storedOperator == '+') storedNumber += displayLabel.toDouble();
     else if (storedOperator == '-') storedNumber -= displayLabel.toDouble();
     else if (storedOperator == 'x') storedNumber *= displayLabel.toDouble();
     else if (storedOperator == '/') storedNumber /= displayLabel.toDouble();

     displayLabel = QString::number(storedNumber,'g', DIGIT_LIMIT);

     uic->displayPanel->setText(displayLabel);
}
//---------------------------------------------------------------
void CalcWin::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_1:
            numberGroup_clicked(uic->num1);
            break;
        case Qt::Key_2:
            numberGroup_clicked(uic->num2);
            break;
        case Qt::Key_3:
            numberGroup_clicked(uic->num3);
            break;
        case Qt::Key_4:
            numberGroup_clicked(uic->num4);
            break;
        case Qt::Key_5:
            numberGroup_clicked(uic->num5);
            break;
        case Qt::Key_6:
            numberGroup_clicked(uic->num6);
            break;
        case Qt::Key_7:
            numberGroup_clicked(uic->num7);
            break;
        case Qt::Key_8:
            numberGroup_clicked(uic->num8);
            break;
        case Qt::Key_9:
            numberGroup_clicked(uic->num9);
            break;
        case Qt::Key_0:
            numberGroup_clicked(uic->num0);
            break;
        //Operators
        case Qt::Key_Plus:
            actionGroup_clicked(uic->actionPlus);
            break;
        case Qt::Key_Minus:
            actionGroup_clicked(uic->actionMinus);
            break;
        case Qt::Key_Asterisk:
            actionGroup_clicked(uic->actionMul);
            break;
        case Qt::Key_Slash:
            actionGroup_clicked(uic->actionDiv);
            break;
        //Comma
        case Qt::Key_Period:
            on_comma_clicked();
            break;
        //Return (enter)
        case Qt::Key_Enter:
        case Qt::Key_Return:
            on_actionCalc_clicked();
            break;
        //Backspace and delete
        case Qt::Key_Backspace:
            on_actionDel_clicked();
            break;
        case Qt::Key_Delete:
            on_actionClear_clicked();
            break;
        //Percentage
        case Qt::Key_Percent:
            on_actionPercent_clicked();
            break;
    }
}
