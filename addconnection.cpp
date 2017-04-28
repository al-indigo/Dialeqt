#include "addconnection.h"
#include "ui_addconnection.h"

#include <QDebug>
#include <QSqlError>
#include "utils.h"
#include "customquerydiagnostics.h"

AddConnection::AddConnection(QList<QPair<QString, QVariant> > _wordsToConnect, QVariant _etimology_tag, QSqlDatabase _acceptorWordDB, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent) :
  wordsToConnect(_wordsToConnect),
  tag(_etimology_tag),
  acceptorWordDB(_acceptorWordDB),
  dictsOpened(_dictsOpened),
  QDialog(parent),
  ui(new Ui::addConnection)
{
  ui->setupUi(this);

/*  model = new QSqlQueryModel();
  model->setQuery("SELECT dictionary.id, \
                  dictionary.transcription, \
                  dictionary.translation \
                  FROM \
                  dictionary \
                  WHERE dictionary.etimology_tag is NULL;", acceptorWordDB);
  */
  model = new QSqlTableModel(this, acceptorWordDB);
  modelSetup();

  connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
  connect(ui->dofilter, SIGNAL(clicked()), this, SLOT(modelSetup()));
  connect(ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(checkConnectedDatabases()));
  connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(checkConnectedDatabases()));
  connect(ui->acceptConnection, SIGNAL(clicked()), this, SLOT(cyclicConnector()));
}

void AddConnection::modelSetup() {
  model->setTable("dictionary");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);
  if (ui->hideconnected->isChecked()) {
    if (ui->transcription->text().isEmpty()){
      filter = QString("etimology_tag is NULL ");
    } else {
      filter = QString("etimology_tag is NULL AND transcription GLOB '") + ui->transcription->text().replace("'","''") + QString("'");
    }
  } else {
    if (ui->transcription->text().isEmpty()) {
      filter = "";
    } else {
      filter = QString("transcription GLOB '") + ui->transcription->text().replace("'","''") + QString("'");
    }
  }
  model->setFilter(filter);

  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Слово"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Перевод"));

  ui->tableView->setModel(model);
  qDebug() << "Model set, filter is: " << filter << " ; last query is: " << getLastExecutedQuery(model->query());
  ui->tableView->setSortingEnabled(true);
  ui->tableView->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
  ui->tableView->horizontalHeader()->setSortIndicator(2, Qt::AscendingOrder);
  ui->tableView->setColumnWidth(1, 100);
  ui->tableView->setColumnWidth(2, 180);

  ui->tableView->setColumnHidden(0,true);
  ui->tableView->setColumnHidden(2,true);
  ui->tableView->setColumnHidden(5,true);
  ui->tableView->setColumnHidden(6,true);
  ui->tableView->setColumnHidden(7,true);
  ui->tableView->setColumnHidden(8,true);

}


bool AddConnection::cyclicConnector() {
  if (!prepareForConnection()) return false;
  for (QList<QPair<QString, QVariant> >::Iterator i = wordsToConnect.begin(); i!= wordsToConnect.end(); i++ ) {
      QPair<QString, QVariant> temp = *i;
      for (QList<QPair<QString, QVariant> >::Iterator k = wordsToConnectWith.begin(); k!= wordsToConnectWith.end(); k++ ) {
        QPair<QString, QVariant> tempToConnectWith = *k;
        qDebug() << "Connecting the word " << temp.second << " from database " << temp.first << " with the word " << tempToConnectWith.second << " from database " << tempToConnectWith.first;
        connectWords( QSqlDatabase::database(temp.first), temp.second, QSqlDatabase::database(tempToConnectWith.first), tempToConnectWith.second);
      }
    }
  emit accept();
  return true;
}


/*
 * This is very dangerous function, I know. Very little time to finish this version, so I must return to it later
 *This is REALLY very unsafe
 * TODO: FIXME: */
bool AddConnection::connectWords(QSqlDatabase initialWordDB, QVariant initialWordID, QSqlDatabase updatedWordDB, QVariant updatedWordID)
{
  QSqlQuery get_dict_info_for_initial_db(initialWordDB);
  if (!get_dict_info_for_initial_db.exec("SELECT id,"
                                         "dict_identificator,"
                                         "dict_name, "
                                         "dict_author,"
                                         "dict_coauthors,"
                                         "dict_classification_tags, "
                                         "dict_description,"
                                         "dialeqt_version "
                                         "FROM dict_attributes WHERE id=1;")) {
      errorMsg("Не удалось получить данные о словаре перед вставкой, так быть не должно: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(get_dict_info_for_initial_db);
      qDebug() << initialWordDB.lastError().text();
      get_dict_info_for_initial_db.next();
      qDebug() << get_dict_info_for_initial_db.value(0).toString();
      qDebug() << get_dict_info_for_initial_db.value(1).toString();
      qDebug() << get_dict_info_for_initial_db.value(2).toString();
      qDebug() << get_dict_info_for_initial_db.value(3).toString();
      qDebug() << get_dict_info_for_initial_db.value(4).toString();
      qDebug() << get_dict_info_for_initial_db.value(5).toString();
      qDebug() << get_dict_info_for_initial_db.value(6).toString();
      qDebug() << get_dict_info_for_initial_db.value(7).toString();
      return false;
    }

  QSqlQuery get_dict_info_for_acceptor_db(updatedWordDB);
  if (!get_dict_info_for_acceptor_db.exec("SELECT id,"
                                          "dict_identificator,"
                                          "dict_name, "
                                          "dict_author,"
                                          "dict_coauthors,"
                                          "dict_classification_tags, "
                                          "dict_description,"
                                          "dialeqt_version "
                                          "FROM dict_attributes WHERE id=1;")) {
      errorMsg("Не удалось получить данные о втором словаре перед вставкой, так быть не должно: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(get_dict_info_for_acceptor_db);
//      return false;
    }


  QSqlQuery insert_or_replace_initial_db(initialWordDB);
  insert_or_replace_initial_db.prepare("INSERT OR REPLACE INTO dict_attributes"
                                       "(id,"
                                       "dict_identificator,"
                                       "dict_name, "
                                       "dict_author,"
                                       "dict_coauthors,"
                                       "dict_classification_tags, "
                                       "dict_description,"
                                       "dialeqt_version"
                                       ")"
                                       "VALUES ((SELECT id from dict_attributes WHERE dict_identificator = :dict_id),"
                                       ":dict_id, "
                                       ":dict_name,"
                                       ":dict_author,"
                                       ":dict_coauthors, "
                                       ":dict_classification_tags, "
                                       ":dict_description,"
                                       ":dialeqt_version);");
  get_dict_info_for_acceptor_db.next();
  insert_or_replace_initial_db.bindValue(":dict_id", get_dict_info_for_acceptor_db.value(1));
  insert_or_replace_initial_db.bindValue(":dict_name", get_dict_info_for_acceptor_db.value(2));
  insert_or_replace_initial_db.bindValue(":dict_author", get_dict_info_for_acceptor_db.value(3));
  insert_or_replace_initial_db.bindValue(":dict_coauthors", get_dict_info_for_acceptor_db.value(4));
  insert_or_replace_initial_db.bindValue(":dict_classification_tags", get_dict_info_for_acceptor_db.value(5));
  insert_or_replace_initial_db.bindValue(":dict_description", get_dict_info_for_acceptor_db.value(6));
  insert_or_replace_initial_db.bindValue(":dialeqt_version", get_dict_info_for_acceptor_db.value(7));
  if (!insert_or_replace_initial_db.exec()) {
      errorMsg("Ошибка при вставке в таблицу словарей первого словаря информации о втором: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(insert_or_replace_initial_db);
      return false;
    }

  QSqlQuery insert_etimology_initial(initialWordDB);
  insert_etimology_initial.prepare("INSERT OR REPLACE INTO etimology "
                           "(id, wordid, dictid) "
                           "VALUES "
                           "((SELECT id from etimology WHERE wordid = :wordid AND dictid = :dictid), :wordid, :dictid)"
                           ";");
  insert_etimology_initial.bindValue(":wordid", initialWordID);
  insert_etimology_initial.bindValue(":dictid", insert_or_replace_initial_db.lastInsertId());
  if (!insert_etimology_initial.exec()) {
      errorMsg("Ошибка при вставке в таблицу связей первого словаря информации о втором: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(insert_etimology_initial);
      return false;
    }

  QSqlQuery update_initial(initialWordDB);
  update_initial.prepare("UPDATE dictionary SET etimology_tag = :tag WHERE id = :id;");
  update_initial.bindValue(":tag", tag);
  update_initial.bindValue(":id", initialWordID);
  if (!update_initial.exec()) {
      qDebug() << "Can't update tag for initial word";
      qDebug() << "Query was the following: " << getLastExecutedQuery(update_initial);
      qDebug() << initialWordDB.lastError().text();
      errorMsg("Связь не проставилась на первом этапе, расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      return false;
  }

  QSqlQuery insert_or_replace_acceptor_db(updatedWordDB);
  insert_or_replace_acceptor_db.prepare("INSERT OR REPLACE INTO dict_attributes"
                                       "(id,"
                                       "dict_identificator,"
                                       "dict_name, "
                                       "dict_author,"
                                       "dict_coauthors,"
                                       "dict_classification_tags, "
                                       "dict_description,"
                                       "dialeqt_version"
                                       ")"
                                       "VALUES ((SELECT id from dict_attributes WHERE dict_identificator = :dict_id),"
                                       ":dict_id, "
                                       ":dict_name,"
                                       ":dict_author,"
                                       ":dict_coauthors, "
                                       ":dict_classification_tags, "
                                       ":dict_description,"
                                       ":dialeqt_version);");
  get_dict_info_for_initial_db.next();
  insert_or_replace_acceptor_db.bindValue(":dict_id", get_dict_info_for_initial_db.value(1));
  insert_or_replace_acceptor_db.bindValue(":dict_name", get_dict_info_for_initial_db.value(2));
  insert_or_replace_acceptor_db.bindValue(":dict_author", get_dict_info_for_initial_db.value(3));
  insert_or_replace_acceptor_db.bindValue(":dict_coauthors", get_dict_info_for_initial_db.value(4));
  insert_or_replace_acceptor_db.bindValue(":dict_classification_tags", get_dict_info_for_initial_db.value(5));
  insert_or_replace_acceptor_db.bindValue(":dict_description", get_dict_info_for_initial_db.value(6));
  insert_or_replace_acceptor_db.bindValue(":dialeqt_version", get_dict_info_for_initial_db.value(7));
  if (!insert_or_replace_acceptor_db.exec()) {
      errorMsg("Ошибка при вставке в таблицу словарей второго словаря информации о первом: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(insert_or_replace_acceptor_db);
      return false;
    }

  QSqlQuery insert_etimology_acceptor(updatedWordDB);
  insert_etimology_acceptor.prepare("INSERT OR REPLACE INTO etimology "
                           "(id, wordid, dictid) "
                           "VALUES "
                           "((SELECT id from etimology WHERE wordid = :wordid AND dictid = :dictid), :wordid, :dictid)"
                           ";");
  insert_etimology_acceptor.bindValue(":wordid", updatedWordID);
  insert_etimology_acceptor.bindValue(":dictid", insert_or_replace_acceptor_db.lastInsertId());
  if (!insert_etimology_acceptor.exec()) {
      errorMsg("Ошибка при вставке в таблицу связей второго словаря информации о первом: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(insert_etimology_acceptor);
      return false;
    }

  QSqlQuery update_acceptor(updatedWordDB);
  update_acceptor.prepare("UPDATE dictionary SET etimology_tag = :tag WHERE id = :id;");
  update_acceptor.bindValue(":tag", tag);
  update_acceptor.bindValue(":id", updatedWordID);
  if (!update_acceptor.exec()) {
      qDebug() << "Can't update tag for acceptor word";
      qDebug() << "Query was the following: " << getLastExecutedQuery(update_acceptor);
      qDebug() << updatedWordDB.lastError().text();
      errorMsg("Связь не проставилась на втором этапе, расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      return false;
  }

  return true;
}


// here we understand if we can connect our word with anything -- we can't if not all the databases are opened now
bool AddConnection::checkConnectedDatabases() {

  QModelIndex index = ui->tableView->currentIndex();
  this->selectedWordid = index.sibling(index.row(),0).data();
  qDebug() << this->selectedWordid;

  this->selectedTag = index.sibling(index.row(),6).data();
  qDebug() << this->selectedTag;

  this->connectionNamesForDicts.clear();

  QSqlQuery getConnectedDbList(this->acceptorWordDB);
  getConnectedDbList.prepare("SELECT \
                             dict_attributes.dict_identificator \
                             FROM \
                             dict_attributes \
                             JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                             JOIN dictionary ON (etimology.wordid = dictionary.id AND dictionary.etimology_tag = :tag) \
                             GROUP BY dict_attributes.id;");
  getConnectedDbList.bindValue(":tag", this->selectedTag);
  if (!getConnectedDbList.exec()) {
      errorMsg("Не получается получить список связанных со словом словарей. Вы можете просмотреть связи, но не можете добавлять новые.");
      ui->acceptConnection->setDisabled(true);
      return false;
    }
  QSet<QString> openedDbIdentifiers;
  foreach (const DictGlobalAttributes &item, *this->dictsOpened) {
      openedDbIdentifiers.insert(item.getDbId());
      qDebug() << "opened db list " << item.getDbId();
    }

  QSet<QString> connectedDbIdentifiers;
  while (getConnectedDbList.next()) {
      connectedDbIdentifiers.insert(getConnectedDbList.value("dict_identificator").toString());
      qDebug() << "connected db list " << getConnectedDbList.value("dict_identificator").toString();
    }

  if (!openedDbIdentifiers.contains(connectedDbIdentifiers)) {
      connectedDbIdentifiers.subtract(openedDbIdentifiers);
      QString missingList;
      foreach (const QString &item, connectedDbIdentifiers) {
          missingList += item + " ";
        }

      errorMsg("Не все словари, с которыми связано слово, открыты. Вы не можете поставить связь, т.к связи обновляются во всех сопутствующих словарях. Список недостающих словарей (это их идентификаторы):" + missingList);
      ui->acceptConnection->setDisabled(true);
      return false;
    }
  connectedDbIdentifiers.intersect(openedDbIdentifiers);
  foreach (const DictGlobalAttributes &dict, *this->dictsOpened) {
      if (connectedDbIdentifiers.contains(dict.getDbId())) {
          this->connectionNamesForDicts.insert(dict.getFilename());
        }
    }

  ui->acceptConnection->setEnabled(true);
  prepareForConnection();
  return true;
}

bool AddConnection::prepareForConnection () {
  QItemSelectionModel *select = ui->tableView->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого необходимо нужно добавить связь");
      return false;
  }

  wordsToConnectWith.clear();
  if (!this->selectedTag.isNull()) {
    foreach (const QString &dbitem, this->connectionNamesForDicts) {
        qDebug() << "trying to find tagged words";
        QSqlQuery findWordsByTag(QSqlDatabase::database(dbitem));
        findWordsByTag.prepare("SELECT dictionary.id FROM dictionary WHERE etimology_tag=:tag;");
        findWordsByTag.bindValue(":tag", this->selectedTag);
        if (!findWordsByTag.exec()) {
            errorMsg("Похоже, консистентность словарей нарушена. Пришлите все связанные с этим словом словари, слово, которое вы пытаетесь связать и слово с которым вы пытаетесь его связать разработчику и не трогайте ничего, пока он не ответит. Или у вас ломается жёсткий диск, но это куда менее вероятно.");
            return false;
        }
        while (findWordsByTag.next()) {
            wordsToConnectWith.append(QPair<QString, QVariant>(dbitem,findWordsByTag.value("id")));
            qDebug() << "appending word " << findWordsByTag.value("id") << " from database " << dbitem;
        }
     }
  }

  wordsToConnectWith.append(QPair<QString, QVariant>(acceptorWordDB.connectionName(),this->selectedWordid));
  qDebug() << "appending selected word " << this->selectedWordid << " from database " << acceptorWordDB.connectionName();

  return true;
}




AddConnection::~AddConnection()
{
  delete model;
  delete ui;
}
