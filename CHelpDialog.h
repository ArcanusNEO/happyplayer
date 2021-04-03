#ifndef CHELPDIALOG_H
#define CHELPDIALOG_H

#include <QDialog>

namespace Ui {
class CHelpDialog;
}

class CHelpDialog : public QDialog {
    Q_OBJECT

private:
    Ui::CHelpDialog *ui_;

public:
    explicit CHelpDialog(QWidget *parent = nullptr);
    ~CHelpDialog();
};

#endif // CHELPDIALOG_H
