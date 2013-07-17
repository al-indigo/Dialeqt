#include "tabcontents.h"
#include "ui_tabcontents.h"

TabContents::TabContents(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::TabContents)
{
  ui->setupUi(this);
}

TabContents::~TabContents()
{
  delete ui;
}
