#include "paradigmwindow.h"
#include "ui_paradigmwindow.h"

ParadigmWindow::ParadigmWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ParadigmWindow)
{
  ui->setupUi(this);
}

ParadigmWindow::~ParadigmWindow()
{
  delete ui;
}
