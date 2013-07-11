#include "dicttabscontainer.h"
#include <QDebug>
#include <QTableView>
DictTabsContainer::DictTabsContainer(QWidget *parent) :
  QTabWidget(parent)
{
  qDebug() << "tabs container instantiated";
  this->addTab(new QTableView(), "test");

}
