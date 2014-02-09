#include "simplesearch.h"
#include "ui_simplesearch.h"

SimpleSearch::SimpleSearch(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SimpleSearch)
{
  ui->setupUi(this);
}

SimpleSearch::~SimpleSearch()
{
  delete ui;
}
