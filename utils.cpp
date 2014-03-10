#include "utils.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTime>
#include <QMessageBox>
#include <QByteArrayData>
#include <QString>
#include <QSqlQuery>

void readAndCompress(const QString & filename, QByteArray &compressedResult) {
  QFileInfo info(filename);
  if (info.exists()) {
      //read file and compress
      QFile file(filename);

      if (!file.open(QIODevice::ReadOnly)) {
          qDebug() << "Can't read the file: " << filename;
      } else {
          qDebug() << "Filesize before compression: " << info.size();
          QTime timer;

          timer.start();
//          compressedResult = qCompress(file.readAll());
          compressedResult = file.readAll();
          qDebug() << "Compression took " << timer.elapsed()/1000;
          qDebug() << "Filesize after compression: " << compressedResult.size();
          file.close();
      }
    }
}

QString createDirectory(const QString & dictId, const QString & postfix) {
  QString temp = QString("tmp/") + dictId + "/";
  QDir tempdir;

  if(!tempdir.mkpath(temp + postfix)) {
    qDebug() << "can't create temp directory for " << postfix;
  }

  return tempdir.absoluteFilePath(temp + postfix);
}

bool extractSound(const QString& dest, const QString& name, const QByteArray& compressed) {
  QFileInfo info(dest + "/" + name);
  QFile file(dest + "/" + name);
  if (!file.open(QIODevice::WriteOnly)) {
      qDebug() << "Can't write sound file to disk";
      return false;
  } else {
//      if (!file.write(qUncompress(compressed))) {
      if (!file.write(compressed)) {
          qDebug() << "Can't write to sound file anymore";
          file.close();
          return false;
      } else {
          file.close();
          return true;
      }
  }
}

void errorMsg (const QString& error_msg){
  QMessageBox errormsg;
  errormsg.setText(error_msg);
  errormsg.setIcon(QMessageBox::Critical);
  errormsg.setDefaultButton(QMessageBox::Ok);
  errormsg.exec();
  return;
}

bool confirmationMsg (const QString& question, QWidget * parent) {
  QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "Вы уверены?", question,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      return true;
    } else {
      return false;
    }
}


bool checkExistance(const QVariant& id, const QString& table, QSqlDatabase& db) {
  QSqlQuery check_existance(db);
  check_existance.prepare("SELECT EXISTS(SELECT 1 FROM " + table + " WHERE id=:id LIMIT 1);");
  check_existance.bindValue(":id", id);
  if (check_existance.exec()) {
      while (check_existance.next()) {
        qDebug() << id << " exists in table " << table;
        return true;
      }
      return false;
    }
  return false;
}

bool deleteAttachment(const QVariant& descriptionid, const QString& table, QSqlDatabase& db) {
  if (!checkExistance(descriptionid, table, db)) {
      errorMsg("Не могу удалить элемент: почему-то удаляемый элемент не был найден в базе данных.");
      return false;
    }
  QSqlQuery find_blob(db);
  find_blob.prepare("SELECT * FROM dict_blobs_description WHERE id=:id;");
  find_blob.bindValue(":id", descriptionid);
  if (find_blob.exec()) {
      while (find_blob.next()) {
          QVariant blobid = find_blob.value("blobid");
          qDebug() << find_blob.value("blobid") << ":" << find_blob.value("wordid") << ":" << find_blob.value("description");
          if (db.transaction()) {
              QSqlQuery delete_blob(db);
              delete_blob.prepare("DELETE FROM blobs WHERE id=:id;");
              delete_blob.bindValue(":id", blobid);
              if (!delete_blob.exec()) {
                  db.rollback();
                  errorMsg("Транзакция не прошла, попробуйте ещё раз, пожалуйста.");
                  return false;
                }
              QSqlQuery delete_blob_description(db);
              delete_blob_description.prepare("DELETE FROM dict_blobs_description WHERE id=:id;");
              delete_blob_description.bindValue(":id", descriptionid);
              if (!delete_blob_description.exec()) {
                  db.rollback();
                  errorMsg("Транзакция не прошла, попробуйте ещё раз, пожалуйста.");
                  return false;
                }
              db.commit();
              return true;
            }
          errorMsg("Не удалось войти в режим транзакции, попробуйте ещё раз, пожалуйста.");
        }
    }
  return false;
}

bool checkOpenedDicts(const QVariant& tag, QSqlDatabase& db, QSet<DictGlobalAttributes> * dictsOpened) {
  QSet<QString> connectionNamesForDicts;
  QSqlQuery getConnectedDbList(db);
  getConnectedDbList.prepare("SELECT \
                             dict_attributes.dict_identificator \
                             FROM \
                             dict_attributes \
                             JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                             JOIN dictionary ON (etimology.wordid = dictionary.id AND dictionary.etimology_tag = :tag) \
                             GROUP BY dict_attributes.id;");
  getConnectedDbList.bindValue(":tag", tag);
  if (!getConnectedDbList.exec()) {
      errorMsg("Не получается получить список связанных со словом словарей. Вы можете просмотреть связи, но не можете добавлять новые.");
      return false;
    }
  QSet<QString> openedDbIdentifiers;
  foreach (const DictGlobalAttributes &item, *dictsOpened) {
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

      errorMsg("Не все словари, с которыми связано слово, открыты. Вы можете просмотреть связи слова со словами из открытых словарей, но не можете добавить новые. Список недостающих словарей (это их идентификаторы):" + missingList);
      return false;
    }
  connectedDbIdentifiers.intersect(openedDbIdentifiers);
  foreach (const DictGlobalAttributes &dict, *dictsOpened) {
      if (connectedDbIdentifiers.contains(dict.getDbId())) {
          connectionNamesForDicts.insert(dict.getFilename());
        }
    }

  return true;
}

bool unlinkPair(const QVariant& tag, QSqlDatabase dbfrom, const QVariant& dbidto) {
  QVariant dbid;
  QSqlQuery getDbIdAttribute(dbfrom);
  getDbIdAttribute.prepare("SELECT * FROM dict_attributes WHERE dict_identificator=:dictid");
  getDbIdAttribute.bindValue(":dictid", dbidto);
  if (!getDbIdAttribute.exec()) {
      errorMsg("Что-то пошло не так: запрос на поиск связи между словарями не прошёл, попробуйте ещё раз?");
      return false;
    } else {
      int i = 0;
      while (getDbIdAttribute.next()) {
          dbid = getDbIdAttribute.value("id");
          i++;
        }
      if (i == 0) {
          return true;
        }
    }
  dbfrom.transaction();
  QSqlQuery deleteFromEtymology(dbfrom);
  deleteFromEtymology.prepare("DELETE FROM etimology WHERE (dictid=:dictid AND wordid IN (SELECT id FROM dictionary WHERE (etimology_tag NOT NULL AND etimology_tag=:tag)))");
  deleteFromEtymology.bindValue(":tag", tag);
  deleteFromEtymology.bindValue(":dictid", dbid);
  if (!deleteFromEtymology.exec()) {
      errorMsg("Не удалось удалить связь из связанного словаря, попробуйте, пожалуйста, ещё раз");
      dbfrom.rollback();
      return false;
    }
  dbfrom.commit();
  return true;
}

bool simpleUnlink (const QVariant& wordid, QSqlDatabase& db) {
  //just unlink the word in this db only
  db.transaction();
  QSqlQuery unlinkSelectedWord(db);
  unlinkSelectedWord.prepare("DELETE FROM etimology WHERE wordid=:wordid");
  unlinkSelectedWord.bindValue(":wordid", wordid);
  if (!unlinkSelectedWord.exec()) {
      db.rollback();
      qDebug() << "Word id I was trying to unlink: " << wordid.toString();
      errorMsg("Что-то пошло не так: не удалось удалить простую этимологическую связь, попробуйте ещё раз");
      return false;
    }
  QSqlQuery deleteTag(db);
  deleteTag.prepare("UPDATE dictionary SET etimology_tag = NULL WHERE id=:wordid");
  deleteTag.bindValue(":wordid", wordid);
  if (!deleteTag.exec()) {
      db.rollback();
      errorMsg("Что-то пошло не так: не удалось стереть тег связи, попробуйте ещё раз");
      return false;
    }
  db.commit();
  return true;
}

// Two cases: if the word is the only one with this tag, we can delete all references from other dicts
// If there are 2 and more words with this tag we just need to clear the tag and del from table "etimology"
bool unlinkEtymology(const QVariant& wordid, const QVariant& tag, QSqlDatabase& db, QSet<DictGlobalAttributes> * dictsOpened) {
  if (tag.isNull()) {
      return true;
    }
  if (!checkOpenedDicts(tag, db, dictsOpened)) {
      return false;
    }
  int taggedwords = 0;
  QSqlQuery checkWordsWithThisTag(db);
  checkWordsWithThisTag.prepare("SELECT * FROM dictionary WHERE (etimology_tag NOT NULL AND etimology_tag=:tag)");
  checkWordsWithThisTag.bindValue(":tag", tag);
  if (checkWordsWithThisTag.exec()) {
      while (checkWordsWithThisTag.next()) {
          taggedwords++;
      }
  }
  if (taggedwords == 0) {
      return true; // word has no tag, so everything's done already
    } else if (taggedwords == 1) {
      //unlink all pairs; then unlink the word itself
      QSet<QString> connectionNamesForDicts;
      QSqlQuery getConnectedDbList(db);
      getConnectedDbList.prepare("SELECT \
                                 dict_attributes.dict_identificator \
                                 FROM \
                                 dict_attributes \
                                 JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                                 JOIN dictionary ON (etimology.wordid = dictionary.id AND dictionary.etimology_tag = :tag) \
                                 GROUP BY dict_attributes.id;");
      getConnectedDbList.bindValue(":tag", tag);
      if (!getConnectedDbList.exec()) {
          errorMsg("Не получается получить список связанных со словом словарей. Вы можете просмотреть связи, но не можете добавлять новые.");
          return false;
        }
      QSet<QString> openedDbIdentifiers;
      foreach (const DictGlobalAttributes &item, *dictsOpened) {
          openedDbIdentifiers.insert(item.getDbId());
          qDebug() << "opened db list " << item.getDbId();
        }

      QSet<QString> connectedDbIdentifiers;
      while (getConnectedDbList.next()) {
          connectedDbIdentifiers.insert(getConnectedDbList.value("dict_identificator").toString());
          qDebug() << "connected db list " << getConnectedDbList.value("dict_identificator").toString();
        }

      connectedDbIdentifiers.intersect(openedDbIdentifiers);
      foreach (const DictGlobalAttributes &dict, *dictsOpened) {
          if (connectedDbIdentifiers.contains(dict.getDbId())) {
              connectionNamesForDicts.insert(dict.getFilename());
            }
        }

      QSqlQuery getIdentificator(db);
      getIdentificator.exec("SELECT * FROM dict_attributes WHERE id=1");
      QVariant dbident;
      while (getIdentificator.next()) {
          dbident = getIdentificator.value("dict_identificator");
        }
      if (!dbident.isValid()) {
          errorMsg("Не удалось определить идентификатор исходной базы данных; попробуйте ещё раз, пожалуйста.");
        }

      foreach (const QString& connname, connectionNamesForDicts) {
          if (!unlinkPair(tag, QSqlDatabase::database(connname), dbident)) {
              return false;
            }
        }
      if (!simpleUnlink(wordid, db)) {
          return false;
        }

    } else if (taggedwords > 1) {
      if (!simpleUnlink(wordid, db)) {
          return false;
        }
    }

  return true;
}

bool deleteWord(const QVariant& wordid, const QVariant& tag, QSqlDatabase& db, QSet<DictGlobalAttributes> * dictsOpened) {
  if (!unlinkEtymology(wordid, tag, db, dictsOpened)) {
      errorMsg("Не удалось удалить слово, попробуйте ещё раз, пожалуйста. Если не удастся снова, напишите разработчику.");
      return false;
    }

  QSqlQuery findAttachments(db);
  findAttachments.prepare("SELECT * FROM dict_blobs_description WHERE wordid=:wordid");
  findAttachments.bindValue(":wordid", wordid);
  if (!findAttachments.exec()) {
      errorMsg("Не удалось произвести поиск файлов, относящихся к слову: попробуйте, пожалуйста, ещё раз");
      return false;
    } else {
      while (findAttachments.next()) {
          if (!deleteAttachment(findAttachments.value("id"), "dict_blobs_description", db)) {
              errorMsg("Не удалось удалить один из файлов, относящихся к слову: попробуйте, пожалуйста, ещё раз");
              return false;
            }
        }
    }

  QSqlQuery findParadigms(db);
  findParadigms.prepare("SELECT * FROM dictionary WHERE regular_form=:wordid");
  findParadigms.bindValue(":wordid", wordid);
  findParadigms.exec();
  while (findParadigms.next()) {
      if (!deleteWord(findParadigms.value("id"), findParadigms.value("etimology_tag"), db, dictsOpened)) {
          errorMsg("Не удалось удалить парадигму слова, попробуйте ещё раз");
          return false;
        }
    }

  db.transaction();
  QSqlQuery deleteW(db);
  deleteW.prepare("DELETE FROM dictionary WHERE id=:wordid");
  deleteW.bindValue(":wordid", wordid);
  if (!deleteW.exec()) {
      db.rollback();
      errorMsg("Не удалось удалить слово, попробуйте ещё раз, пожалуйста");
      return false;
    }
  db.commit();

  return true;

}
