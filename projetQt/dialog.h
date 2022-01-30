#ifndef DIALOG_H
#define DIALOG_H
#include "mainWindowTetris.h"
#include "ui_mainWindowTetris.h"

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    MainWindowTetris *w;

};

#endif // DIALOG_H
