#include "phonologywindow.h"
#include "ui_phonologywindow.h"

PhonologyWindow::PhonologyWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PhonologyWindow)
{
  ui->setupUi(this);
}

PhonologyWindow::~PhonologyWindow()
{
  delete ui;
}
