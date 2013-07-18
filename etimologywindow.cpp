#include "etimologywindow.h"
#include "ui_etimologywindow.h"

EtimologyWindow::EtimologyWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EtimologyWindow)
{
  ui->setupUi(this);
}

EtimologyWindow::~EtimologyWindow()
{
  delete ui;
}
