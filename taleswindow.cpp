#include "taleswindow.h"
#include "ui_taleswindow.h"

TalesWindow::TalesWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::TalesWindow)
{
  ui->setupUi(this);
}

TalesWindow::~TalesWindow()
{
  delete ui;
}
