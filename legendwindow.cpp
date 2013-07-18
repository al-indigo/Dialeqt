#include "legendwindow.h"
#include "ui_legendwindow.h"

LegendWindow::LegendWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::legendWindow)
{
  ui->setupUi(this);
}

LegendWindow::~LegendWindow()
{
  delete ui;
}
