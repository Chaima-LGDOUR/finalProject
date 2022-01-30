#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include "precompiledHeader.h"
#include "tetrixpiece.h"

class QLabel;

class TetrixBoard : public QFrame
{
    Q_OBJECT

public:
    TetrixBoard(QWidget *parent = 0);

    //Apart from the setNextPieceLabel() function and the start() and pause() public slots,
    //we only provide public functions to reimplement QWidget::sizeHint() and QWidget::minimumSizeHint().
    //The signals are used to communicate changes to the player's information to the TetrixWindow instance.

    void setNextPieceLabel(QLabel *label);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void start();
    void pause();
    void setVolume(int volume);

signals:
    void scoreChanged(int score);
    void levelChanged(int level);
    void linesRemovedChanged(int numLines);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    enum { BoardWidth = 10, BoardHeight = 22 };


    //The rest of the functionality is provided by reimplementations of protected event handlers and private functions
    TetrixShape &shapeAt(int x, int y) { return board[(y * BoardWidth) + x]; }
    int timeoutTime() { return 1000 / (1 + level); }
    int squareWidth() { return contentsRect().width() / BoardWidth; }
    int squareHeight() { return contentsRect().height() / BoardHeight; }
    void clearBoard();
    void dropDown();
    void oneLineDown();
    void pieceDropped(int dropHeight);
    void removeFullLines();
    void newPiece();
    void showNextPiece();
    bool tryMove(const TetrixPiece &newPiece, int newX, int newY);
    void drawSquare(QPainter &painter, int x, int y, TetrixShape shape);

    QBasicTimer timer;
    QPointer<QLabel> nextPieceLabel;
    bool isStarted;
    bool isPaused;
    bool isWaitingAfterLine;
    TetrixPiece curPiece;
    TetrixPiece nextPiece;
    int curX;
    int curY;
    int numLinesRemoved;
    int numPiecesDropped;
    int score;
    int level;
    int volume;
    //The board is composed of a fixed-size array whose elements correspond to spaces for individual blocks.
    //Each element in the array contains a TetrixShape value corresponding to the type of shape that occupies
    //that element.
    TetrixShape board[BoardWidth * BoardHeight];
    //we add QMediaPlayer to playback songs,
    QMediaPlayer player;
    QMediaPlayer pieceSound;
};
#endif // TETRIXBOARD_H
