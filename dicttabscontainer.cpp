#include "dicttabscontainer.h"
#include <QtSql>
#include <QDebug>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QSizePolicy>
#include "tabcontents.h"
#include "customqhash.h"


DictTabsContainer::DictTabsContainer(QWidget *parent) :
  QTabWidget(parent)
{
  this->setMinimumSize(300,400);
  this->setMaximumSize(10000,10000);
  this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  qDebug() << "tabs container instantiated";
}

bool DictTabsContainer::createDictTabInitial(DictGlobalAttributes dictAttrs)
{
  if (!this->dictsOpened.contains(dictAttrs)) {
      dbFactory.createConnection(dictAttrs.getFilename());
      if (!dbFactory.initDb(dictAttrs, QSqlDatabase::database(dictAttrs.getFilename())) ) {
          return false;
        }


//      this->addTab(view, dictAttrs.getFilename());
      this->addTab(new TabContents(dictAttrs, this), dictAttrs.getDictname());

      return true;

    } else {
      qDebug() << "this dictionary already exists";
      return false;
    }
}

bool DictTabsContainer::openDictTabInitial(DictGlobalAttributes dictAttrs)
{
  return true;
}

bool DictTabsContainer::createTab(DictGlobalAttributes dictAttrs)
{
  return true;
}
