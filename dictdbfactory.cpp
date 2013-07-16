#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QDebug>
#include "dictdbfactory.h"

QString getLastExecutedQuery(const QSqlQuery& query) {
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext()) {
        it.next();
        str.replace(it.key(), it.value().toString());
    }
    return str;
}

DictDbFactory::DictDbFactory(QObject *parent) :
    QObject(parent), sqlDriver("QSQLITE")
{
}

QSqlDatabase DictDbFactory::createConnection(const QString &connectionName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(connectionName);
    if (!db.open()) {
        db.lastError();
//        QMessageBox::critical(0, qApp->tr("Не удаётся открыть/создать файл словаря."),
    }
    openedConnections.append(connectionName);
//    db.driver()->hasFeature(QSqlDriver::foreign)
    return db;
}

bool DictDbFactory::initDb(const DictGlobalAttributes &attrs, const QSqlDatabase &db)
{
    if (!db.isValid()) {
        qDebug() << "Database is " << db.isOpen() << " and it's because " << db.isOpenError();
        return false;
    }
    QSqlQuery query(db);

    /*===== TODO: Checking foreign key support ======*/

    /*===== Creating tables =========*/

    /* table dict_attribues holds info about dictionary itself */
    //TODO: look for consonant strings
    //TODO: check out how to implement versioning support
    if (!query.exec(
          "create table dict_attributes ("
          "id                        integer UNIQUE NOT NULL primary key, "
          "dict_identificator        text, "
          "dict_name                 text, "
          "dict_author               text, "
          "dict_coauthors            text, "
          "dict_classification_tags  text, "
          "dict_description          text, "
          "dict_legend               text)"
          )) {
        qDebug() << "Failed to create dict_attributes table";
        qDebug() << db.lastError().text();
        return false;
      } else {
        qDebug() << "dict_attributes table created ok";
      }
    
    /* table dictionary stores the dictionary and links to foreign blobs */
    if (!query.exec(
          "create table dictionary ("
          "id                       integer UNIQUE NOT NULL primary key, "
          "word                     text, "
          "regular_form             text, "
          "transcription            text, "
          "translation              text, "
          "link_to_sounds_list      integer, "
          "link_to_praat_markup     integer, "
          "is_a_regular_form        bool, "
          "has_paradigm             bool) "
          )) {
        qDebug() << "Failed to create dictionary table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "dictionary table created ok";
    }

    /* table phonology */

    /* table fairy_tails */

    /* table dict_blobs_table */
    /* These blobs are not global; they correspond only to dictionary entries */
    if (!query.exec(
          "create table dict_blobs_table ("
          "id                       integer UNIQUE NOT NULL primary key, "
          "type                     integer, "
          "description              text, "
          "object                   blob, "
          "wordid                   integer, "
          "Foreign key(wordid)      References dictionary(id) )"
          )) {
        qDebug() << "Failed to create blob table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "Blob table created ok";
    }

    /*======== Filling initial data ==============*/

    query.prepare("insert into dict_attributes ( "
                  "dict_identificator, "
                  "dict_name, "
                  "dict_author, "
                  "dict_coauthors, "
                  "dict_classification_tags, "
                  "dict_description, "
                  "dict_legend) "
                  "values ( "
                  ":dictId, "
                  ":dictName, "
                  ":dictAuthor, "
                  ":dictCoathors, "
                  ":dictTags, "
                  ":dictDescription, "
                  ":dictLegend);"
                  );
    query.bindValue(":dictId", attrs.getDbId());
    query.bindValue(":dictName", attrs.getDictname());
    query.bindValue(":dictAuthor", attrs.getAuthor());
    //TODO: make set serialization/deserialization
    query.bindValue(":dictCoathors", *attrs.getCoauthors().begin());
    query.bindValue(":dictTags", *attrs.getTags().begin());

    query.bindValue(":dictDescription", attrs.getDescription());
    query.bindValue(":dictLegend", attrs.getLegend());
    qDebug() << "Going to execute the following query: " << query.lastQuery();

    if (!query.exec()) {
        qDebug() << "Failed to fill initial data in global_attrs table";
        qDebug() << "Query was the following: " << getLastExecutedQuery(query);
        qDebug() << db.lastError().text();
        return false;
    } else {
        return true;
    }

}
