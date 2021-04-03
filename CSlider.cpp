#include "CSlider.h"

CSlider::CSlider(QWidget* parent) : QSlider(parent), i_(0) {

}

CSlider::~CSlider() {

}

void CSlider::setRange64(qint64 min, qint64 max) {
    i_ = 0;
    while (max > INT_MAX || min > INT_MAX) {
        min >>= 1;
        max >>= 1;
        ++i_;
    }
    QAbstractSlider::setRange((int)min, (int)max);
    setValue((int)min);
}

void CSlider::setValue64(qint64 val) {
    setValue((int)(val >> i_));
}

void CSlider::setMax64(qint64 max) {
    setRange64(0, max);
}

qint64 CSlider::convertTo64(int val) {
    return ((qint64)val) << i_;
}

void CSlider::mousePressEvent(QMouseEvent* event) {
    if (event->x() > width()) {
        return;
    }
    qint64 x = (qint64)((double)event->x() / width() * (maximum() - minimum()) + minimum());

    setValue64(x);
    emit sliderPressed64(x);
}

void CSlider::mouseMoveEvent(QMouseEvent* event) {
    if (event->x() > width()) {
        return;
    }
    qint64 x = (qint64)((double)event->x() / width() * (maximum() - minimum()) + minimum());

    setValue64(x);
    emit sliderMoved64(x);
}
