#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui_(new Ui::MainWindow), player_(new QMediaPlayer), playList_(new QMediaPlaylist), timer_(new QTimer(this)), inited_(false) {
    QMenu* fileMenu, * aboutMenu;

    ui_->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    player_->setVolume(ui_->volSlider->value());
    playList_->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    player_->setPlaylist(playList_);
    player_->setVideoOutput(ui_->videoWidget);
    fileMenu = new QMenu(ui_->fileButton);
    aboutMenu = new QMenu(ui_->helpButton);
    fileMenu->addAction(ui_->actionOpenFile);
    fileMenu->addAction(ui_->actionExit);
    aboutMenu->addAction(ui_->actionAbout);
    ui_->fileButton->setMenu(fileMenu);
    ui_->helpButton->setMenu(aboutMenu);
    connect(this->timer_, &QTimer::timeout, this, &MainWindow::hideMouse);
    connect(ui_->fullScrButton, &QPushButton::clicked, this, &MainWindow::setFullScr);
    connect(ui_->volButton, &QPushButton::clicked, this, &MainWindow::setMute);
    connect(ui_->volSlider, &QSlider::valueChanged, this, &MainWindow::setVolBtnIcon);
    connect(ui_->maximizeButton, &QPushButton::clicked, this, &MainWindow::onMaxBtnClicked);
    connect(ui_->minimizeButton, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(ui_->closeButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(player_, &QMediaPlayer::stateChanged, this, &MainWindow::onPlayerStateChanged);
    connect(ui_->nextButton, &QPushButton::clicked, playList_, &QMediaPlaylist::next);
    connect(ui_->prevButton, &QPushButton::clicked, playList_, &QMediaPlaylist::previous);
    connect(ui_->mainButton, &QPushButton::clicked, this, &MainWindow::onMainButtonClicked);
    connect(ui_->actionOpenFile, &QAction::triggered, this, &MainWindow::onOpenFileTrigrd);
    connect(ui_->actionAbout, &QAction::triggered, this, &MainWindow::onAboutTrigrd);
    connect(ui_->volSlider, &QSlider::valueChanged, player_, &QMediaPlayer::setVolume);
    connect(ui_->posSlider, &CSlider::sliderMoved64, player_, &QMediaPlayer::setPosition);
    connect(ui_->posSlider, &CSlider::sliderPressed64, player_, &QMediaPlayer::setPosition);
    connect(player_, &QMediaPlayer::positionChanged, ui_->posSlider, &CSlider::setValue64);
    connect(player_, &QMediaPlayer::durationChanged, ui_->posSlider, &CSlider::setMax64);
    connect(player_, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(player_, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(ui_->actionExit, &QAction::triggered, this, &MainWindow::close);
}

MainWindow::~MainWindow() {
    delete ui_;
    delete playList_;
    delete player_;
}

void MainWindow::onOpenFileTrigrd() {
    ins2List(QFileDialog::getOpenFileName(this, "打开"));
    player_->play();
}

void MainWindow::onAboutTrigrd() {
    CHelpDialog* hd = new CHelpDialog(this);

    hd->exec();
    delete hd;
}

void MainWindow::ins2List(const QString &path) {
    QUrl url;
    int x;

    qDebug() << path;
    if (path.contains("://")) {
        url = QUrl(path);
    } else {
        url = QUrl("file:///" + path);
    }
    if (!url.isValid()) {
        return;
    }
    if (path.right(4).toLower() == ".m3u") {
        playList_->load(url);
        playList_->setCurrentIndex(0);
    } else {
        x = playList_->currentIndex();
        playList_->insertMedia((x >= 0 ? x : 0), url);
        playList_->setCurrentIndex(x >= 0 ? x : 0);
    }
}

void MainWindow::onDurationChanged(qint64 len) {
    ui_->endTimeLabel->setText(" " + QString::number(len / 1000 / 60 / 60) + ":" + QString::number(len / 1000 / 60 % 60) + ":" + QString::number(len / 1000 % 60));
}

void MainWindow::onMainButtonClicked() {
    if (player_->state() == QMediaPlayer::PlayingState) {
        player_->pause();
    } else {
        player_->play();
    }
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::State state) {
    if (state == QMediaPlayer::PlayingState) {
        ui_->mainButton->setText("暂停");
    } else {
        ui_->mainButton->setText("播放");
    }
}

void MainWindow::onPositionChanged(qint64 pos) {
    ui_->currentTimeLabel->setText(" " + QString::number(pos / 1000 / 60 / 60) + ":" + QString::number(pos / 1000 / 60 % 60) + ":" + QString::number(pos / 1000 % 60));
}

void MainWindow::dropEvent(QDropEvent* event) {
    QList <QUrl> urls = event->mimeData()->urls();
    int i;

    qDebug() << urls.size();
    for (i = 0; i < urls.size(); ++i) {
        ins2List(urls[i].toString());
    }
    player_->play();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    event->accept();
}

void MainWindow::onMaxBtnClicked() {
    if (windowState() == Qt::WindowNoState) {
        setWindowState(Qt::WindowMaximized);
        adjustPos();
    } else {
        setWindowState(Qt::WindowNoState);
        resize(800, 600);
    }
}

void MainWindow::setMute() {
    ui_->volSlider->setValue(0);
}

void MainWindow::setVolBtnIcon(int vol) {
    if (!vol) {
        ui_->volButton->setText("🔇");
    }
    else if (vol <= 30) {
        ui_->volButton->setText("🔈");
    }
    else {
        ui_->volButton->setText("🔉");
    }
}

void MainWindow::adjustPos() {
    ui_->systemGroupBox->setGeometry(0, 0, width(), ui_->systemGroupBox->height());
    ui_->videoWidget->setGeometry(5, ui_->systemGroupBox->height(), width() - 10,
                                  height() - ui_->procGroupBox->height() - ui_->playControlGroupBox->height() - ui_->systemGroupBox->height());
    ui_->playControlGroupBox->setGeometry(0, height() - ui_->playControlGroupBox->height(), width(), ui_->playControlGroupBox->height());
    ui_->procGroupBox->setGeometry(0, height() - ui_->playControlGroupBox->height() - ui_->procGroupBox->height(), width(), ui_->procGroupBox->height());
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    if (windowState() == Qt::WindowFullScreen) {
        ui_->systemGroupBox->hide();
        ui_->procGroupBox->hide();
        ui_->playControlGroupBox->hide();
        ui_->videoWidget->setGeometry(0, 0, width(), height());
        return;
    }
    ui_->systemGroupBox->show();
    ui_->procGroupBox->show();
    ui_->playControlGroupBox->show();
    adjustPos();
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (windowState() != Qt::WindowNoState) {
        setCursor(Qt::ArrowCursor);
        return;
    }
    int x = event->x(), y = event->y(), w = width(), h = height();

    igmx_ = event->globalX();
    igmy_ = event->globalY();
    igwx_ = this->x();
    igwy_ = this->y();
    igww_ = width();
    igwh_ = height();
    if (x >= _MAINWINDOW_PADDING && x < w - _MAINWINDOW_PADDING && y > ui_->systemGroupBox->height() + ui_->systemGroupBox->y() && y < h - _MAINWINDOW_PADDING) {
        setCursor(Qt::ArrowCursor);
        mouseRegion_ = 0;
    } else if (x >= 0 && x < _MAINWINDOW_PADDING && y >= 0 && y < _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeFDiagCursor);
        mouseRegion_ = 1;
    } else if (x >= _MAINWINDOW_PADDING && x < w - _MAINWINDOW_PADDING && y >= 0 && y < _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeVerCursor);
        mouseRegion_ = 2;
    } else if (x >= w - _MAINWINDOW_PADDING && x < w && y >= 0 && y < _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeBDiagCursor);
        mouseRegion_ = 3;
    } else if (x >= w - _MAINWINDOW_PADDING && x < w && y > ui_->systemGroupBox->height() + ui_->systemGroupBox->y() && y < h - _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeHorCursor);
        mouseRegion_ = 4;
    } else if (x >= w - _MAINWINDOW_PADDING && x < w && y >= h - _MAINWINDOW_PADDING && y < h) {
        setCursor(Qt::SizeFDiagCursor);
        mouseRegion_ = 5;
    } else if (x >= _MAINWINDOW_PADDING && x < w - _MAINWINDOW_PADDING && y >= h - _MAINWINDOW_PADDING && y < h) {
        setCursor(Qt::SizeVerCursor);
        mouseRegion_ = 6;
    } else if (x >= 0 && x < _MAINWINDOW_PADDING && y >= h - _MAINWINDOW_PADDING && y < h) {
        setCursor(Qt::SizeBDiagCursor);
        mouseRegion_ = 7;
    } else if (x >= 0 && x < _MAINWINDOW_PADDING && y > ui_->systemGroupBox->height() + ui_->systemGroupBox->y() && y < h - _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeHorCursor);
        mouseRegion_ = 8;
    } else if (x >= 0 && x < w && y >= _MAINWINDOW_PADDING && y <= ui_->systemGroupBox->height() + ui_->systemGroupBox->y()) {
        setCursor(Qt::ArrowCursor);
        mouseRegion_ = 9;
    } else {
        setCursor(Qt::ArrowCursor);
        mouseRegion_ = 0;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    int x = event->x(), y = event->y(), w = width(), h = height(), rw = igww_, rh = igwh_;

    if (windowState() != Qt::WindowNoState) {
        setCursor(Qt::ArrowCursor);
        if (windowState() == Qt::WindowFullScreen) {
            if (y >= height() - ui_->playControlGroupBox->height() - ui_->procGroupBox->height() && y < height()) {
                ui_->playControlGroupBox->show();
                ui_->playControlGroupBox->setGeometry(0, height() - ui_->playControlGroupBox->height(), width(), ui_->playControlGroupBox->height());
                ui_->procGroupBox->show();
                ui_->procGroupBox->setGeometry(0, height() - ui_->playControlGroupBox->height() - ui_->procGroupBox->height(), width(), ui_->procGroupBox->height());
                timer_->stop();
            } else if (y >= 0 && y <= ui_->systemGroupBox->height()) {
                ui_->systemGroupBox->show();
                ui_->systemGroupBox->setGeometry(0, 0, width(), ui_->systemGroupBox->height());
                timer_->stop();
            } else {
                ui_->systemGroupBox->hide();
                ui_->playControlGroupBox->hide();
                ui_->procGroupBox->hide();
                timer_->start(1500);
            }
        }
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        if (mouseRegion_ == 9) {
            move(event->globalX() - igmx_ + igwx_, event->globalY() - igmy_ + igwy_);
        } else if (mouseRegion_) {
            if (mouseRegion_ >= 3 && mouseRegion_ <= 5) {
                rw = event->globalX() - igmx_ + igww_;
            } else if (mouseRegion_ == 1 || mouseRegion_ == 7 || mouseRegion_ == 8) {
                rw = igmx_ - event->globalX() + igww_;
                if (rw >= minimumWidth()) {
                    move(event->globalX() - igmx_ + igwx_, this->y());
                }
            }
            if (mouseRegion_ >= 5 && mouseRegion_ <= 7) {
                rh = event->globalY() - igmy_ + igwh_;
            } else if (mouseRegion_ >= 1 && mouseRegion_ <= 3) {
                rh = igmy_ - event->globalY() + igwh_;
                if (rh >= minimumHeight()) {
                    move(this->x(), event->globalY() - igmy_ + igwy_);
                }
            }
            resize(rw, rh);
            return;
        }
    } else if (event->buttons() & Qt::RightButton) {

    }

    if (x >= _MAINWINDOW_PADDING && x < w - _MAINWINDOW_PADDING && y > ui_->systemGroupBox->height() + ui_->systemGroupBox->y() && y < h - _MAINWINDOW_PADDING) {
        setCursor(Qt::ArrowCursor);
    } else if (x >= 0 && x < _MAINWINDOW_PADDING && y >= 0 && y < _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (x >= _MAINWINDOW_PADDING && x < w - _MAINWINDOW_PADDING && y >= 0 && y < _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeVerCursor);
    } else if (x >= w - _MAINWINDOW_PADDING && x < w && y >= 0 && y < _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeBDiagCursor);
    } else if (x >= w - _MAINWINDOW_PADDING && x < w && y > ui_->systemGroupBox->height() + ui_->systemGroupBox->y() && y < h - _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeHorCursor);
    } else if (x >= w - _MAINWINDOW_PADDING && x < w && y >= h - _MAINWINDOW_PADDING && y < h) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (x >= _MAINWINDOW_PADDING && x < w - _MAINWINDOW_PADDING && y >= h - _MAINWINDOW_PADDING && y < h) {
        setCursor(Qt::SizeVerCursor);
    } else if (x >= 0 && x < _MAINWINDOW_PADDING && y >= h - _MAINWINDOW_PADDING && y < h) {
        setCursor(Qt::SizeBDiagCursor);
    } else if (x >= 0 && x < _MAINWINDOW_PADDING && y > ui_->systemGroupBox->height() + ui_->systemGroupBox->y() && y < h - _MAINWINDOW_PADDING) {
        setCursor(Qt::SizeHorCursor);
    } else if (x >= 0 && x < w && y >= _MAINWINDOW_PADDING && y <= ui_->systemGroupBox->height() + ui_->systemGroupBox->y()) {
        setCursor(Qt::ArrowCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        ui_->mainButton->click();
    } else if (event->key() == Qt::Key_Right) {
        player_->setPosition(player_->position() + 5000);
    } else if (event->key() == Qt::Key_Left) {
        player_->setPosition(player_->position() - 5000);
    } else if (event->key() == Qt::Key_Up) {
        ui_->volSlider->setValue(ui_->volSlider->value() + 1);
    } else if (event->key() == Qt::Key_Down) {
        ui_->volSlider->setValue(ui_->volSlider->value() - 1);
    } else if (event->key() == Qt::Key_Return && !event->isAutoRepeat() && windowState() != Qt::WindowFullScreen) {
        setFullScr();
    } else if (event->key() == Qt::Key_Escape && !event->isAutoRepeat() && windowState() == Qt::WindowFullScreen) {
        setWindowState(Qt::WindowNoState);
        resize(800, 600);
    }
}

void MainWindow::setFullScr() {
    setWindowState(Qt::WindowNoState);
    setWindowState(Qt::WindowFullScreen);
    resizeEvent(nullptr);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->x() >= ui_->videoWidget->x() && event->x() <= ui_->videoWidget->x() + ui_->videoWidget->width() &&
        event->y() >= ui_->videoWidget->y() && event->y() <= ui_->videoWidget->y() + ui_->videoWidget->height()) {
        ui_->mainButton->click();
    } else if (event->x() >= 0 && event->x() < width() && event->y() >= _MAINWINDOW_PADDING && event->y() <= ui_->systemGroupBox->height() + ui_->systemGroupBox->y()) {
        onMaxBtnClicked();
    }
}

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::lightGray));
    painter.drawRect(rect());
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect(2, 2, width() - 4, height() - 4);
}

void MainWindow::hideMouse() {
    timer_->stop();
    if (player_->state() != QMediaPlayer::StoppedState) {
        setCursor(Qt::BlankCursor);
    }
}
