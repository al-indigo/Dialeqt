#include "dicttabscontainer.h"
#include <QtSql>
#include <QDebug>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QSizePolicy>
#include "tabcontents.h"
#include "customqhash.h"
#include "customquerydiagnostics.h"

DictTabsContainer::DictTabsContainer(QWidget *parent) :
  QTabWidget(parent)
{
  this->setMinimumSize(300,400);
  this->setMaximumSize(10000,10000);
  this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
//  this->connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(remove(int)));
  qDebug() << "tabs container instantiated";
}

bool DictTabsContainer::remove(int tab) {
  this->removeTab(tab);
  return true;
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

bool DictTabsContainer::openDictTabInitial(DictGlobalAttributes & dictAttrs, const QString& filename)
{
  QSqlDatabase conn = QSqlDatabase::database(filename, true);
  if (conn.isValid()) {
      qDebug() << "Opened the dict, conn is valid";
      qDebug() << "That means that dict is already opened, we should move to the tab where the dict is";
      return false;
  } else {
      qDebug() << "Dict connection doesn't exist yet', opening";
      conn = dbFactory.createConnection(filename);
      if (conn.isValid()) {
          qDebug() << "Opened the dictionary db, everything's okay";
          qDebug() << "Now we should extract dict attributes, add them to the list (and check if they are here already) and open a tab for the dictionary";
          QSqlQuery query(conn);
          if (query.exec("Select * from 'dict_attributes' where dict_attributes.id = 1")) {
              while (query.next()) {
                  qDebug() << "Opened db, which has the following attributes: ";
                  qDebug() << query.value("id").toString();
                  qDebug() << query.value("dict_identificator").toString();
                  qDebug() << query.value("dict_name").toString();
                  qDebug() << query.value("dict_author").toString();
                  qDebug() << query.value("dict_coauthors").toString();
                  qDebug() << query.value("dict_classification_tags").toString();
                  qDebug() << query.value("dict_description").toString();
                  qDebug() << query.value("dialeqt_version").toString();

                  QString dictname = query.value("dict_name").toString();
                  QString author = query.value("dict_author").toString();
                  QStringList credits_list = query.value("dict_coauthors").toString().split(",");
                  QSet<QString> credits;
                  foreach (const QString &item, credits_list) {
                      if(0 != item.capacity()) {
                        credits.insert(item.simplified());
                      }
                    }
                  qDebug() << "Original credits " << credits_list << "; Credits list " << credits;
                  QStringList tags_list = query.value("dict_classification_tags").toString().split(",");
                  QSet<QString> tags;
                  foreach (const QString &item, tags_list) {
                      if(0 != item.capacity()) {
                        tags.insert(item.simplified());
                      }
                    }
                  qDebug() << "Tags list " << tags;
                  QString description = query.value("dict_description").toString();
                  QString dbId = query.value("dict_identificator").toString();;

                  DictGlobalAttributes tmpDictAttrs(dbId, filename, dictname, author, credits, tags, description);
                  qDebug() << "Constructed TEMP dictAttrs";
                  tmpDictAttrs.debugPrint();

                  dictAttrs = tmpDictAttrs;
                  qDebug() << "Opened a dictionary and constructed the following attributes";
                  dictAttrs.debugPrint();
                  this->addTab(new TabContents(dictAttrs, this), dictAttrs.getDictname());
                }
            } else {
              qDebug() << "Query to db failed; maybe it's not a dictionary";
              qDebug() << "Query was the following: " << getLastExecutedQuery(query);
              qDebug() << "Error: " << query.lastError();
              return false;
            }
          return true;
      } else {
          qDebug() << "Still can't open the dictionary db, WROOOOONG";
          return false;
      }
  }

  return true;
}

bool DictTabsContainer::createTab(DictGlobalAttributes dictAttrs)
{
  return true;
}
