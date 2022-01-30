
#include "tetrixboard.h"
#include "tetrixwindow.h"


TetrixWindow::TetrixWindow()
{
    // set up the user interface elements for the game
    board = new TetrixBoard;

    nextPieceLabel = new QLabel;
    nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
    nextPieceLabel->setAlignment(Qt::AlignCenter);
    nextPieceLabel->setStyleSheet("background-color:moccasin;");
    board->setNextPieceLabel(nextPieceLabel);
    board->setStyleSheet("background-image: url(:/picture/canva.jfif);");

    //Three QLCDNumber objects are used to display the score,number
    //of lives, and lines removed. These initially show default values
    scoreLcd = new QLCDNumber(2);
    scoreLcd->setSegmentStyle(QLCDNumber::Outline);
    scoreLcd->setStyleSheet("background-color: black");
    levelLcd = new QLCDNumber(2);
    levelLcd->setSegmentStyle(QLCDNumber::Outline);
    levelLcd->setStyleSheet("background-color: black");
    linesLcd = new QLCDNumber(2);
    linesLcd->setSegmentStyle(QLCDNumber::Outline);
    linesLcd->setStyleSheet("background-color: black");

    //Three buttons with shortcuts are constructed so that the user can
    //start a new game, pause the current game, and quit the application:

    //These buttons are configured so that they never receive the keyboard
    //focus; we want the keyboard focus to remain with the TetrixBoard
    //instance so that it receives all the keyboard events.
    startButton = new QPushButton(tr("&Start"));
    startButton->setStyleSheet("background-color:rgb(255, 85, 127);");
    startButton->setFocusPolicy(Qt::NoFocus);
    quitButton = new QPushButton(tr("&Quit"));
    quitButton->setFocusPolicy(Qt::NoFocus);
    quitButton->setStyleSheet("background-color:moccasin;");
    pauseButton = new QPushButton(tr("&Pause"));
    pauseButton->setFocusPolicy(Qt::NoFocus);
    pauseButton->setStyleSheet("background-color:moccasin;");

    //to control the volume or level of sound, so we set the max and the min
    //value and also we add the stylesheet and policy ..
    volumeDial = new QDial();
    volumeDial->setStyleSheet("background-color:moccasin;");
    volumeDial->setFocusPolicy(Qt::StrongFocus);
    volumeDial->setNotchesVisible(true);
    volumeDial->setMaximum(100);
    volumeDial->setMinimum(0);
    volumeDial->setValue(50);
    volumeDial->setStyleSheet("QDial{ background-color: moccasin;"
                              "QLinearGradient(spread:pad, x1:0.683, y1:1, x2:1, y2:0,"
                              " stop:0 rgba(103, 103, 103,255), stop:1 rgba(144, 144, 144, 255)); }");

    //We connect clicked() signals from the Start and Pause buttons to the board,
    //and from the Quit button to the application's  QCoreApplication::quit() slot.
    connect(startButton, &QPushButton::clicked, board, &TetrixBoard::start);
    connect(quitButton , &QPushButton::clicked, qApp, &QApplication::quit);
    connect(pauseButton, &QPushButton::clicked, board, &TetrixBoard::pause);
    //we connect the signal value changed () from the volume dial to the board by the slot set volume ()
    connect(volumeDial, SIGNAL(valueChanged(int)),board, SLOT(setVolume(int))); //lazy

//Signals from the board are also connected to the LCD widgets for the purpose of updating the score,
    //number of lives, and lines removed from the playing area.
#if __cplusplus >= 201402L
    connect(board, &TetrixBoard::scoreChanged,
            scoreLcd, qOverload<int>(&QLCDNumber::display));
    connect(board, &TetrixBoard::levelChanged,
            levelLcd, qOverload<int>(&QLCDNumber::display));
    connect(board, &TetrixBoard::linesRemovedChanged,
            linesLcd, qOverload<int>(&QLCDNumber::display));
#else
    connect(board, &TetrixBoard::scoreChanged,
            scoreLcd, QOverload<int>::of(&QLCDNumber::display));
    connect(board, &TetrixBoard::levelChanged,
            levelLcd, QOverload<int>::of(&QLCDNumber::display));
    connect(board, &TetrixBoard::linesRemovedChanged,
            linesLcd, QOverload<int>::of(&QLCDNumber::display));
#endif

    //We place the label, LCD widgets, and the board into a QGridLayout along
    //with some labels that we create with the createLabel() convenience function:
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(board, 0, 0, 15, 8);
	layout->addWidget(createLabel(tr("NEXT")), 0, 9);
	layout->addWidget(nextPieceLabel, 1, 9);
	layout->addWidget(createLabel(tr("LEVEL")), 2, 9);
	layout->addWidget(levelLcd, 3, 9);
    layout->addWidget(createLabel(tr("SCORE")), 4, 9);
    layout->addWidget(scoreLcd, 5, 9);
    layout->addWidget(createLabel(tr("LINES REMOVED")), 6, 9);
    layout->addWidget(linesLcd, 7, 9);
	layout->addWidget(createLabel(tr("    ")), 8, 9);
    layout->addWidget(quitButton, 9, 9);
    layout->addWidget(pauseButton, 10, 9);
	layout->addWidget(startButton, 11, 9);
    layout->addWidget(volumeDial, 14, 9);
    //Finally, we set the grid layout on the widget
    setLayout(layout);
}

// this function simply creates a new label on the heap
QLabel *TetrixWindow::createLabel(const QString &text)
{
    QLabel *label = new QLabel(text);
    //gives it an appropriate alignment, and returns it to the caller:
    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    return label;
}
