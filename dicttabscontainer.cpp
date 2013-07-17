#include "dicttabscontainer.h"
#include <QtSql>
#include <QDebug>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QSizePolicy>
#include "customqhash.h"

void DictTabsContainer::initializeDictModel(QSqlRelationalTableModel *model) {
  model->setTable("dictionary");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Слово"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Перевод"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Звуковой файл"));

  model->select();
}


DictTabsContainer::DictTabsContainer(QWidget *parent) :
  QTabWidget(parent)
{
  this->setMinimumSize(300,400);
  this->setMaximumSize(10000,10000);
  this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  qDebug() << "tabs container instantiated";
}

bool DictTabsContainer::addDictTabAndCreate(DictGlobalAttributes dictAttrs)
{
  if (!this->dictsOpened.contains(dictAttrs)) {
      dbFactory.createConnection(dictAttrs.getFilename());
      if (!dbFactory.initDb(dictAttrs, QSqlDatabase::database(dictAttrs.getFilename())) ) {
          return false;
        }

      QSqlRelationalTableModel *model = new QSqlRelationalTableModel(this, QSqlDatabase::database(dictAttrs.getFilename()));
      this->initializeDictModel(model);

      QTableView *view = new QTableView;
      view->setModel(model);
//      view->setItemDelegate(new QSqlRelationalDelegate(view));


      this->addTab(view, dictAttrs.getFilename());

      return true;

    } else {
      qDebug() << "this dictionary already exists";
      return false;
    }
}
