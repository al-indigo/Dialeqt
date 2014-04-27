#include "dicttabscontainer.h"
#include <QtSql>
#include <QDebug>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QSizePolicy>
#include "tabcontents.h"
#include "customquerydiagnostics.h"
#include "customqhash.h"
#include "utils.h"

void DictTabsContainer::removeDictFromSet(DictGlobalAttributes& dict) {
  dictsOpened.remove(dict);
  dictsOpenedMap.remove(dict.getDictname());
  qDebug() << "DictAttr removed from set";
}

DictTabsContainer::~DictTabsContainer() {
  dictsOpened.clear();
  dictsOpenedMap.clear();
}


DictTabsContainer::DictTabsContainer(QWidget *parent) :
  QTabWidget(parent)
{
  this->setMinimumSize(300,400);
  this->setMaximumSize(10000,10000);
  this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  this->connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(remove(int)));
  qDebug() << "tabs container instantiated";
}

bool DictTabsContainer::remove(int tab) {
  this->widget(tab)->deleteLater();
  return true;
}

bool DictTabsContainer::goToDictAndWord(QString dictName, QVariant id) {
  qDebug() << dictName << id;
  this->setCurrentIndex(dictsOpenedMap[dictName]);
  emit goToWord(id);
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
      int tabnumber = this->addTab(new TabContents(dictAttrs, &(this->dictsOpened), this), dictAttrs.getDictname());
      this->setCurrentIndex(tabnumber);
      this->dictsOpened.insert(dictAttrs);
      this->dictsOpenedMap[dictAttrs.getDictname()] = tabnumber;

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
      errorMsg("Словарь уже открыт.");
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

                  /* version check */
                  if (query.value("dialeqt_version").toInt() < 3 ) {
                      errorMsg("В этой версии программы сменилась схема устройства базы данных словаря: вам необходимо прислать словарь на al@somestuff.ru для преобразования в новый формат (извините). Это обязательно необходимо сделать, в предыдущих версиях программы могла теряться информация о парадигмах.");
                      conn.close();
                      QSqlDatabase::removeDatabase(filename);
                      return false;
                  }

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
                  int tabnumber = this->addTab(new TabContents(dictAttrs, &(this->dictsOpened), this), dictAttrs.getDictname());
                  this->setCurrentIndex(tabnumber);
                  this->dictsOpened.insert(dictAttrs); //TODO: maybe I should do it earlier, don't remember
                  this->dictsOpenedMap[dictAttrs.getDictname()] = tabnumber;
                }
            } else {
              qDebug() << "Query to db failed; maybe it's not a dictionary";
              qDebug() << "Query was the following: " << getLastExecutedQuery(query);
              qDebug() << "Error: " << query.lastError();
              errorMsg("Проверьте, пожалуйста, что вы открываете именно файл словаря, а не какой-то другой. Если это действительно файл словаря, то возможен один из трех вариантов: \n 1) Ошибка в программе \n 2) Файл словаря испорчен \n 3) Ваш жесткий диск капризничает или умирает \n Во всех трёх случаях лучше написать на почту al@somestuff.ru и прислать файл словаря, чтобы я понял, в чем может быть дело.");
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

bool DictTabsContainer::isDictOpened(QString dbId)
{
  foreach (const DictGlobalAttributes &item, this->dictsOpened) {
      if (item.getDbId() == dbId) return true;
    }
  return false;
}
