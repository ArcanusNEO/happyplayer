#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QObject>
#include <QPushButton>
#include <QThread>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QString>
#include <QFileDialog>
#include <QVideoWidget>
#include <QAudio>
#include <QAudioDecoder>
#include <QMouseEvent>

#include "CSlider.h"
#include "CHelpDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
#define _MAINWINDOW_PADDING 3

private:
    Ui::MainWindow *ui_;
    QMediaPlayer* player_;
    QMediaPlaylist* playList_;
    qint64 pos;
    int mouseRegion_, igmx_, igmy_, igwx_, igwy_, igww_, igwh_;
    QTimer* timer_;
    bool inited_;

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ins2List(const QString &url);
    void adjustPos();

public slots:
    void onOpenFileTrigrd();
    void onAboutTrigrd();
    void onDurationChanged(qint64 len);
    void onPositionChanged(qint64 pos);
    void onMainButtonClicked();
    void onPlayerStateChanged(QMediaPlayer::State state);
    void onMaxBtnClicked();
    void setMute();
    void setVolBtnIcon(int vol);
    void setFullScr();
    void hideMouse();
};
#endif // MAINWINDOW_H
