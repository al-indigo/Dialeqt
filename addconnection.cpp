#include "addconnection.h"
#include "ui_addconnection.h"

#include <QDebug>
#include <QSqlError>
#include "utils.h"
#include "customquerydiagnostics.h"

AddConnection::AddConnection(QList<QPair<QString, QVariant>> _wordsToConnect, QVariant _etimology_tag, QSqlDatabase _acceptorWordDB, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent) :
  wordsToConnect(_wordsToConnect),
  tag(_etimology_tag),
  acceptorWordDB(_acceptorWordDB),
  dictsOpened(_dictsOpened),
  QDialog(parent),
  ui(new Ui::addConnection)
{
  ui->setupUi(this);
  model = new QSqlQueryModel();
  model->setQuery("SELECT dictionary.id, \
                  dictionary.transcription, \
                  dictionary.translation \
                  FROM \
                  dictionary \
                  WHERE dictionary.etimology_tag is NULL;", acceptorWordDB);
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Перевод"));

  ui->tableView->setModel(model);
  ui->tableView->setColumnWidth(1, 100);
  ui->tableView->setColumnWidth(2, 180);
  ui->tableView->setColumnHidden(0, true);

  connect(ui->acceptConnection, SIGNAL(clicked()), this, SLOT(cyclicConnector()));
  connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
}

bool AddConnection::cyclicConnector() {
  for (QList<QPair<QString, QVariant>>::Iterator i = wordsToConnect.begin(); i!= wordsToConnect.end(); i++ ) {
      QPair<QString, QVariant> temp = *i;
      connectWords( QSqlDatabase::database(temp.first), temp.second);
    }
  return true;
}


/*
 * This is very dangerous function, I know. Very little time to finish this version, so I must return to it later
 *This is REALLY very unsafe
 * TODO: FIXME: */
bool AddConnection::connectWords(QSqlDatabase initialWordDB, QVariant initialWordID)
{
  QItemSelectionModel *select = ui->tableView->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого необходимо нужно добавить связь");
      return false;
  }
  QModelIndex index = ui->tableView->currentIndex();
  QVariant wordid = index.sibling(index.row(),0).data();
  qDebug() << wordid;

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

  QSqlQuery get_dict_info_for_acceptor_db(acceptorWordDB);
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

  QSqlQuery insert_or_replace_acceptor_db(acceptorWordDB);
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

  QSqlQuery insert_etimology_acceptor(acceptorWordDB);
  insert_etimology_acceptor.prepare("INSERT OR REPLACE INTO etimology "
                           "(id, wordid, dictid) "
                           "VALUES "
                           "((SELECT id from etimology WHERE wordid = :wordid AND dictid = :dictid), :wordid, :dictid)"
                           ";");
  insert_etimology_acceptor.bindValue(":wordid", wordid);
  insert_etimology_acceptor.bindValue(":dictid", insert_or_replace_acceptor_db.lastInsertId());
  if (!insert_etimology_acceptor.exec()) {
      errorMsg("Ошибка при вставке в таблицу связей второго словаря информации о первом: расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      qDebug() << "Query was the following: " << getLastExecutedQuery(insert_etimology_acceptor);
      return false;
    }

  QSqlQuery update_acceptor(acceptorWordDB);
  update_acceptor.prepare("UPDATE dictionary SET etimology_tag = :tag WHERE id = :id;");
  update_acceptor.bindValue(":tag", tag);
  update_acceptor.bindValue(":id", wordid);
  if (!update_acceptor.exec()) {
      qDebug() << "Can't update tag for acceptor word";
      qDebug() << "Query was the following: " << getLastExecutedQuery(update_acceptor);
      qDebug() << acceptorWordDB.lastError().text();
      errorMsg("Связь не проставилась на втором этапе, расскажите об этом разработчику и пришлите словари и транскрипции слов, которые вы пытались связать");
      return false;
  }

  emit accept();
  return true;
}

AddConnection::~AddConnection()
{
  delete model;
  delete ui;
}
