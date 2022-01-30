
#include "precompiledHeader.h"
#include "mainWindowTetris.h"
#include "dialog.h"
#include "tetrixwindow.h"

int main(int argc, char *argv[])
{
    QApplication applicationTetris(argc, argv);
    Dialog windowTetris;
    windowTetris.resize(400,400);
    windowTetris.show();
    return applicationTetris.exec();
}
