#include "CHelpDialog.h"
#include "ui_CHelpDialog.h"

CHelpDialog::CHelpDialog(QWidget *parent) : QDialog(parent), ui_(new Ui::CHelpDialog) {
    ui_->setupUi(this);
    setWindowFlags(Qt::Drawer);
}

CHelpDialog::~CHelpDialog() {
    delete ui_;
}
