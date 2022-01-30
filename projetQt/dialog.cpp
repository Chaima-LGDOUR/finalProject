#include "dialog.h"
#include "ui_dialog.h"
#include "mainWindowTetris.h"
#include "ui_mainWindowTetris.h"
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setFixedSize(600,500);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
 w = new MainWindowTetris(this);
 w->show();

}



