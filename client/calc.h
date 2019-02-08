#ifndef CALC_H
#define CALC_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QKeyEvent>

class CalcWin;
//-----------------------------------------------------
namespace Ui {
    class CalcWin;
}
//-----------------------------------------------------
class CalcWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalcWin(QWidget *parent = nullptr);
    ~CalcWin();

private slots:
    void numberGroup_clicked(QAbstractButton*);
    void actionGroup_clicked(QAbstractButton*);

    void on_actionDel_clicked();
    void on_actionCalc_clicked();
    void on_comma_clicked();
    void on_actionClear_clicked();
    void on_actionPercent_clicked();
    void on_actionSign_clicked();

private:
    Ui::CalcWin *uic;
    const int DIGIT_LIMIT = 16;
    bool operatorClicked, hasStoredNumber;
    QChar storedOperator;
    double storedNumber;
    void calculate_result();

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // CALC_H
