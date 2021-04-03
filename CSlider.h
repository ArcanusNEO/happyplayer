#ifndef CSLIDER_H
#define CSLIDER_H

#include <QSlider>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QMouseEvent>

class CSlider : public QSlider {
    Q_OBJECT

private:
    int i_;
    qint64 convertTo64(int val);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

public:
    CSlider(QWidget* parent = nullptr);
    virtual ~CSlider();

public slots:
    void setRange64(qint64 min, qint64 max);
    void setValue64(qint64 val);
    void setMax64(qint64 max);

signals:
    void sliderMoved64(qint64 val);
    void sliderPressed64(qint64 val);
};

#endif // CSLIDER_H
