
#include "precompiledHeader.h"
#include "tetrixboard.h"

TetrixBoard::TetrixBoard(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
    isStarted = false;
    isPaused = false;
    clearBoard();
    //set the path of the media
    player.setMedia(QUrl("qrc:/sound/Tetris.mp3"));
    //set the volume
    player.setVolume(volume);

    //The first (next) piece is also set up with a random shape
    nextPiece.setRandomShape();
}

//The setNextPieceLabel() function is used to pass in an externally-constructed label to the board,
//so that it can be shown alongside the playing area
void TetrixBoard::setNextPieceLabel(QLabel *label)
{
    nextPieceLabel = label;
}

//We provide a reasonable size hint and minimum size hint for the board, based on the size of the
//space for each block in the playing area:
QSize TetrixBoard::sizeHint() const
{
    return QSize(BoardWidth * 15 + frameWidth() * 2,
                 BoardHeight * 15 + frameWidth() * 2);
}
//By using a minimum size hint, we indicate to the layout in the parent widget that the board should
//not shrink below a minimum size.
QSize TetrixBoard::minimumSizeHint() const
{
    return QSize(BoardWidth * 5 + frameWidth() * 2,
                 BoardHeight * 5 + frameWidth() * 2);
}

//A new game is started when the start() slot is called. This resets the game's state, the player's
//score and level, and the contents of the board:
void TetrixBoard::start()
{
    if (isPaused)
        return;

    player.play();
    isStarted = true;
    isWaitingAfterLine = false;
    numLinesRemoved = 0;
    numPiecesDropped = 0;
    score = 0;
    level = 1;
    volume = 50;
    clearBoard();

    emit linesRemovedChanged(numLinesRemoved);
    emit scoreChanged(score);
    emit levelChanged(level);
    player.setVolume(volume);
    pieceSound.setVolume(volume);

    newPiece();
    //We also emit signals to inform other components of these changes before creating a new piece
    //that is ready to be dropped into the playing area. We start the timer that determines how often
    //the piece drops down one row on the board
    timer.start(timeoutTime(), this);
}


//The pause() slot is used to temporarily stop the current game by stopping the internal timer
void TetrixBoard::pause()
{

  //We perform checks to ensure that the game can only be paused if it is already running and not already paused.
    if (!isStarted)
        return;

    isPaused = !isPaused;
    if (isPaused) {
        timer.stop();
        player.pause();
    } else {
        timer.start(timeoutTime(), this);
        player.play();
    }
    update();
}

//the setVolume(int volume) function used to control the sound
void TetrixBoard::setVolume(int volume)
{
    if(volume == 0)
    {
        player.setMuted(true);
        pieceSound.setMuted(true);
    }
    else{
        player.setMuted(false);
        player.setVolume(volume);
        pieceSound.setMuted(false);
        pieceSound.setVolume(volume);
    }
}

//The paintEvent() function is straightforward to implement. We begin by calling the base class's
//implementation of paintEvent() before constructing a QPainter for use on the board
void TetrixBoard::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    QRect rect = contentsRect(); //Since the board is a subclass of QFrame, we obtain a QRect that
    //covers the area inside the frame decoration before drawing our own content.


    //If the game is paused, we want to hide the existing state of the board and show some text.
    //We achieve this by painting text onto the widget and returning early from the function. The rest of the painting is performed after this point.
    if (isPaused) {
        painter.setPen ( Qt::white );
        painter.drawText(rect, Qt::AlignCenter, tr("Pause"));
        return;
    }

    //The position of the top of the board is found by subtracting the total height of
    //each space on the board from the bottom of the frame's internal rectangle. For each space on
    //the board that is occupied by a piece, we call the drawSquare() function to draw a block at that position
    int boardTop = rect.bottom() - BoardHeight*squareHeight();

    for (int i = 0; i < BoardHeight; ++i) {
        for (int j = 0; j < BoardWidth; ++j) {
            TetrixShape shape = shapeAt(j, BoardHeight - i - 1);
            if (shape != NoShape)
                drawSquare(painter, rect.left() + j * squareWidth(),
                           boardTop + i * squareHeight(), shape);
        }
    }

    //Unlike the existing pieces on the board, the current piece is drawn block-by-block at its current position:
    if (curPiece.shape() != NoShape) {
        for (int i = 0; i < 4; ++i) {
            int x = curX + curPiece.x(i);
            int y = curY - curPiece.y(i);
            drawSquare(painter, rect.left() + x * squareWidth(),
                       boardTop + (BoardHeight - y - 1) * squareHeight(),
                       curPiece.shape());
        }
    }
    if(player.state()== QMediaPlayer::StoppedState)
    {
        player.play();
    }
}

//The keyPressEvent() handler is called whenever the player presses a key while the TetrixBoard
//widget has the keyboard focus.
void TetrixBoard::keyPressEvent(QKeyEvent *event)
{
    if (!isStarted || isPaused || curPiece.shape() == NoShape) {
        QFrame::keyPressEvent(event);
        return;

        //=>If there is no current game, the game is running but paused, or if there is no current
        //shape to control, we simply pass on the event to the base class.
    }

    //We check whether the event is about any of the keys that the player uses to control the
    //current piece and, if so, we call the relevant function to handle the input:
    switch (event->key()) {
    case Qt::Key_Left:
        tryMove(curPiece, curX - 1, curY);
        break;
    case Qt::Key_Right:
        tryMove(curPiece, curX + 1, curY);
        break;
    case Qt::Key_Down:
        tryMove(curPiece.rotatedRight(), curX, curY);
        break;
    case Qt::Key_Up:
        tryMove(curPiece.rotatedLeft(), curX, curY);
        break;
    case Qt::Key_Space:
        dropDown();
        break;
    case Qt::Key_D:
        oneLineDown();
        break;
    default:
        QFrame::keyPressEvent(event);//In the case where the player presses a key that we are not
        //interested in, we again pass on the event to the base class's implementation of keyPressEvent().
    }
}

//The timerEvent() handler is called every time the class's QBasicTimer instance times out. We need to
//check that the event we receive corresponds to our timer. If it does, we can update the board:
void TetrixBoard::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        if (isWaitingAfterLine) {
            isWaitingAfterLine = false;
            newPiece();
            timer.start(timeoutTime(), this);
        } else {
            oneLineDown();
        }
    } else {
        QFrame::timerEvent(event);
    }
    //=>If a row (or line) has just been filled, we create a new piece and reset the timer; otherwise
    //we move the current piece down by one row. We let the base class handle other timer events that we receive.
}
//The clearBoard() function simply fills the board with the TetrixShape::NoShape value:
void TetrixBoard::clearBoard()
{
    for (int i = 0; i < BoardHeight * BoardWidth; ++i)
        board[i] = NoShape;
}

//The dropDown() function moves the current piece down as far as possible on the board, either until it
//is touching the bottom of the playing area or it is stacked on top of another piece:
void TetrixBoard::dropDown()
{
    int dropHeight = 0;
    int newY = curY;
    while (newY > 0) {
        if (!tryMove(curPiece, curX, newY - 1))
            break;
        --newY;
        ++dropHeight;
    }
    pieceDropped(dropHeight);

    //=>The number of rows the piece has dropped is recorded and passed to the pieceDropped() function
    //so that the player's score can be updated.
}

//The oneLineDown() function is used to move the current piece down by one row (line), either when
//the user presses the D key or when the piece is scheduled to move
void TetrixBoard::oneLineDown()
{
    if (!tryMove(curPiece, curX, curY - 1))
        pieceDropped(0);
    //=>If the piece cannot drop down by one line, we call the pieceDropped() function with zero as
    //the argument to indicate that it cannot fall any further, and that the player should receive no
    //extra points for the fall.
}

//The pieceDropped() function itself is responsible for awarding points to the player for positioning
//the current piece, checking for full rows on the board and, if no lines have been removed, creating
//a new piece to replace the current one:
void TetrixBoard::pieceDropped(int dropHeight)
{
    for (int i = 0; i < 4; ++i) {
        int x = curX + curPiece.x(i);
        int y = curY - curPiece.y(i);
        shapeAt(x, y) = curPiece.shape();
    }
    pieceSound.setMedia(QUrl("qrc:/sound/drop.wav"));
    pieceSound.play();
    ++numPiecesDropped;
    if (numPiecesDropped % 25 == 0) {
        ++level;
        timer.start(timeoutTime(), this);
        emit levelChanged(level);
    }

    score += dropHeight + 7;
    emit scoreChanged(score);
    removeFullLines();

    if (!isWaitingAfterLine)
        newPiece();
}

//We call removeFullLines() each time a piece has been dropped. This scans the board from bottom to top,
//looking for blank spaces on each row.
void TetrixBoard::removeFullLines()
{
    int numFullLines = 0;

    for (int i = BoardHeight - 1; i >= 0; --i) {
        bool lineIsFull = true;

        for (int j = 0; j < BoardWidth; ++j) {
            if (shapeAt(j, i) == NoShape) {
                lineIsFull = false;
                break;
            }
        }

        //If a row contains no blank spaces, the rows above it are copied down by one row to compress the stack
        //of pieces, the top row on the board is cleared, and the number of full lines found is incremented.
        if (lineIsFull) {
            ++numFullLines;
            pieceSound.setMedia(QUrl("qrc:/sound/money.wav"));
            pieceSound.play();
            for (int k = i; k < BoardHeight - 1; ++k) {
                for (int j = 0; j < BoardWidth; ++j)
                    shapeAt(j, k) = shapeAt(j, k + 1);
            }
            for (int j = 0; j < BoardWidth; ++j)
                shapeAt(j, BoardHeight - 1) = NoShape;
        }
    }

    if (numFullLines > 0) {
        numLinesRemoved += numFullLines;
        score += 10 * numFullLines;
        emit linesRemovedChanged(numLinesRemoved);
        emit scoreChanged(score);

        timer.start(500, this);
        isWaitingAfterLine = true;
        curPiece.setShape(NoShape);
        update();
    } //=>If some lines have been removed, the player's score and the total number of lines removed are updated.
    //The linesRemoved() and scoreChanged() signals are emitted to send these new values to other widgets in the window.
}

//The newPiece() function places the next available piece at the top of the board, and creates a new piece with
//a random shape:
void TetrixBoard::newPiece()
{
    curPiece = nextPiece;
    nextPiece.setRandomShape();
    showNextPiece();
    curX = BoardWidth / 2 + 1;
    curY = BoardHeight - 1 + curPiece.minY();

    if (!tryMove(curPiece, curX, curY)) {
        curPiece.setShape(NoShape);
        timer.stop();
        isStarted = false;
    } //=>We place a new piece in the middle of the board at the top. The game is over if the piece can't move,
    //so we unset its shape to prevent it from being drawn, stop the timer, and unset the isStarted flag.
}

//The showNextPiece() function updates the label that shows the next piece to be dropped:
void TetrixBoard::showNextPiece()
{
    if (!nextPieceLabel)
        return;

    int dx = nextPiece.maxX() - nextPiece.minX() + 1;
    int dy = nextPiece.maxY() - nextPiece.minY() + 1;

    QPixmap pixmap(dx * squareWidth(), dy * squareHeight());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), nextPieceLabel->palette().background());

    for (int i = 0; i < 4; ++i) {
        int x = nextPiece.x(i) - nextPiece.minX();
        int y = nextPiece.y(i) - nextPiece.minY();
        drawSquare(painter, x * squareWidth(), y * squareHeight(),
                   nextPiece.shape());
    }
    nextPieceLabel->setPixmap(pixmap);
} //=>We draw the piece's component blocks onto a pixmap that is then set on the label.


//The tryMove() function is used to determine whether a piece can be positioned at the specified coordinates:
bool TetrixBoard::tryMove(const TetrixPiece &newPiece, int newX, int newY)
{
    for (int i = 0; i < 4; ++i) {
        int x = newX + newPiece.x(i);
        int y = newY - newPiece.y(i);
        if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight)
            return false;
        if (shapeAt(x, y) != NoShape)
            return false;
    }

    //We examine the spaces on the board that the piece needs to occupy and, if they are already occupied by
    //other pieces, we return false to indicate that the move has failed.
    curPiece = newPiece;
    curX = newX;
    curY = newY;
    update();
    return true;
    //=>If the piece could be placed on the board at the desired location, we update the current piece and
    //its position, update the widget, and return true to indicate success.
}


//The drawSquare() function draws the blocks (normally squares) that make up each piece using different
//colors for pieces with different shapes
void TetrixBoard::drawSquare(QPainter &painter, int x, int y, TetrixShape shape)
{
    static const QRgb colorTable[8] = {
        0x000000, 0xCC6666, 0x66CC66, 0x6666CC,
        0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00
    };

    QColor color = colorTable[int(shape)];
    painter.fillRect(x + 1, y + 1, squareWidth() - 2, squareHeight() - 2,
                     color);

    painter.setPen(color.light());
    painter.drawLine(x, y + squareHeight() - 1, x, y);
    painter.drawLine(x, y, x + squareWidth() - 1, y);

    painter.setPen(color.dark());
    painter.drawLine(x + 1, y + squareHeight() - 1,
                     x + squareWidth() - 1, y + squareHeight() - 1);
    painter.drawLine(x + squareWidth() - 1, y + squareHeight() - 1,
                     x + squareWidth() - 1, y + 1);

    //=>We obtain the color to use from a look-up table that relates each shape to an RGB value,
    //and use the painter provided to draw the block at the specified coordinates
}
