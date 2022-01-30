#ifndef TETRIXWINDOW_H
#define TETRIXWINDOW_H

#include "precompiledHeader.h"

class QLCDNumber;
class QLabel;
class QPushButton;

class TetrixBoard;

class TetrixWindow : public QWidget
{
    Q_OBJECT

public:
    TetrixWindow();

private:
    //We use private member variables for the board, various display widgets,
    //and buttons to allow the user to start a new game, pause the current game, and quit
    QLabel *createLabel(const QString &text);

    TetrixBoard *board;
    QLabel *nextPieceLabel;
    QLCDNumber *scoreLcd;
    QLCDNumber *levelLcd;
    QLCDNumber *linesLcd;
    QPushButton *startButton;
    QPushButton *quitButton;
    QPushButton *pauseButton;
    QDial *volumeDial;
};
#endif // TETRIXWINDOW_H
