#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QDir>
#include <QDebug>

#include "dictdbfactory.h"
#include "customquerydiagnostics.h"

DictDbFactory::DictDbFactory(QObject *parent) :
    QObject(parent) {}


QSqlDatabase DictDbFactory::createConnection(const QString &connectionName) {
    QDir dictsDir = QDir();
    dictsDir.mkpath("dictionaries");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(connectionName);
    if (!db.open()) {
        qDebug() << "Can't create db " << db.databaseName();
        db.lastError();
//        QMessageBox::critical(0, qApp->tr("Не удаётся открыть/создать файл словаря."),
    }
    if (!db.isValid()) {
        qDebug() << "Connection to database is invalid " << db.connectOptions() << db.databaseName() << db.isOpenError();
    }
//    db.driver()->hasFeature(QSqlDriver::foreign)
    qDebug() << "Sql driver supports Transactions:" << db.driver()->hasFeature(QSqlDriver::Transactions);
    qDebug() << "Sql driver supports PreparedQueries:" << db.driver()->hasFeature(QSqlDriver::PreparedQueries);
    qDebug() << "Sql driver supports SimpleLocking:" << db.driver()->hasFeature(QSqlDriver::SimpleLocking);
    qDebug() << "Sql driver supports EventNotifications:" << db.driver()->hasFeature(QSqlDriver::EventNotifications);
    qDebug() << "Sql driver supports FinishQuery:" << db.driver()->hasFeature(QSqlDriver::FinishQuery);
    qDebug() << "Sql driver supports MultipleResultSets:" << db.driver()->hasFeature(QSqlDriver::MultipleResultSets);
    qDebug() << "Sql driver supports CancelQuery:" << db.driver()->hasFeature(QSqlDriver::CancelQuery);
    qDebug() << "Sql driver supports Unicode:" << db.driver()->hasFeature(QSqlDriver::Unicode);
    qDebug() << "Sql driver supports LastInsertId:" << db.driver()->hasFeature(QSqlDriver::LastInsertId);
    qDebug() << "Sql driver supports BatchOperations:" << db.driver()->hasFeature(QSqlDriver::BatchOperations);
    return db;
}

bool DictDbFactory::initDb(DictGlobalAttributes &attrs, const QSqlDatabase &db)
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
          "CREATE TABLE dict_attributes ("
              "id                       INTEGER PRIMARY KEY "
                                               "NOT NULL "
                                               "UNIQUE,"
              "dict_identificator       TEXT, "
              "dict_name                TEXT, "
              "dict_author              TEXT, "
              "dict_coauthors           TEXT, "
              "dict_classification_tags TEXT, "
              "dict_description         TEXT, "
              "dialeqt_version          INTEGER DEFAULT ( 3 ) "
          ");"
          )) {
        qDebug() << "Failed to create dict_attributes table";
        qDebug() << db.lastError().text();
        return false;
      } else {
        qDebug() << "dict_attributes table created ok";
      }

    
    /* table dictionary stores the dictionary */
    if (!query.exec(
          "CREATE TABLE dictionary ( "
              "id                 INTEGER PRIMARY KEY "
                                         "NOT NULL "
                                         "UNIQUE, "
              "word               TEXT, "
              "regular_form       INTEGER, "
              "transcription      TEXT, "
              "translation        TEXT, "
              "is_a_regular_form  BOOL, "
              "etimology_tag      TEXT "
          ");"
          )) {
        qDebug() << "Failed to create dictionary table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "dictionary table created ok";
    }

    if(!query.exec(
         "CREATE TABLE etimology ("
             "id                 INTEGER PRIMARY KEY "
                                        "NOT NULL "
                                        "UNIQUE, "
             "wordid             INTEGER NOT NULL,"
             "dictid             INTEGER NOT NULL"
         ");"
         )) {
        qDebug() << "Failed to create etimology connector table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "dictionary table created ok";
    }

    /* create index for etimology */
    if (!query.exec(
          "CREATE INDEX idx_dictionary ON dictionary ( "
              "etimology_tag COLLATE BINARY ASC  "
          ");"
          )) {
        qDebug() << "Failed to create dictionary table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "dictionary etimology index created ok";
    }

    /* table phonology */

    /* table fairy_tales */
    /* Now this table stores the legend too. Legend is always #0 */
    if (!query.exec(
          "CREATE TABLE fairy_tales ( "
              "id          INTEGER PRIMARY KEY "
                                  "NOT NULL "
                                  "UNIQUE, "
              "tale_name        TEXT, "
              "tale             TEXT"
          ");"
          )) {
        qDebug() << "Failed to create tales table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "Tales table created ok";
    }

    if (!query.exec("INSERT into fairy_tales (id, tale_name) VALUES (1, 'Легенда словаря');")) {
        qDebug() << "Failed to insert legend into fairy_tales";
        qDebug() << db.lastError().text();
        return false;
      } else {
        qDebug() << "Legend inserted ok";
      }

    /* table fairy_tales_blobs */
    /* These blobs are NOT global; they store only fairy tales and legend blobs*/
    if (!query.exec(
          "CREATE TABLE tales_blobs ( "
              "id          INTEGER PRIMARY KEY "
                                  "NOT NULL "
                                  "UNIQUE, "
              "tale_id      INTEGER NOT NULL, "
              "filename    TEXT, "
              "blob    BLOB NOT NULL, "
              "FOREIGN KEY (tale_id) REFERENCES fairy_tales (id) ON DELETE CASCADE ON UPDATE CASCADE MATCH FULL"
          ");"
          )) {
        qDebug() << "Failed to create blobs table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "Blobs table created ok";
    }

    /* table blobs */
    /* These blobs are NOT global; they store only dict blobs*/
    if (!query.exec(
          "CREATE TABLE blobs ( "
              "id          INTEGER PRIMARY KEY "
                                  "NOT NULL "
                                  "UNIQUE, "
              "mainblob    BLOB NOT NULL,"
              "secblob     BLOB"
          ");"
          )) {
        qDebug() << "Failed to create blobs table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "Blobs table created ok";
    }


    /* table dict_blobs_description */
    /* These blobs are not global; they correspond only to dictionary entries */
    if (!query.exec(
          "CREATE TABLE dict_blobs_description ( "
              "id          INTEGER PRIMARY KEY "
                                  "NOT NULL "
                                  "UNIQUE, "
              "type        INTEGER NOT NULL, "
              "name        TEXT NOT NULL, "
              "description TEXT, "
//              "object      BLOB    NOT NULL, "
              "wordid      INTEGER NOT NULL, "
              "blobid      INTEGER NOT NULL, "
              "FOREIGN KEY ( blobid ) REFERENCES blobs ( id ) ON DELETE CASCADE "
                                                                      "ON UPDATE CASCADE "
                                                                      "MATCH FULL "
          ");"
          )) {
        qDebug() << "Failed to create blobs description table";
        qDebug() << db.lastError().text();
        return false;
    } else {
        qDebug() << "Blobs description table created ok";
    }

    /*======== Filling initial data ==============*/

    query.prepare("insert into dict_attributes ( "
                    "dict_identificator, "
                    "dict_name, "
                    "dict_author, "
                    "dict_coauthors, "
                    "dict_classification_tags, "
                    "dict_description) "
                  "values ( "
                    ":dictId, "
                    ":dictName, "
                    ":dictAuthor, "
                    ":dictCoauthors, "
                    ":dictTags, "
                    ":dictDescription);"
                  );
    query.bindValue(":dictId", attrs.getDbId());
    query.bindValue(":dictName", attrs.getDictname());
    query.bindValue(":dictAuthor", attrs.getAuthor());
    //TODO: make set serialization/deserialization
    query.bindValue(":dictCoauthors", attrs.getCoauthorsQString());
    query.bindValue(":dictTags", attrs.getTagsQString());

    query.bindValue(":dictDescription", attrs.getDescription());
    qDebug() << "Going to execute the following query: " << query.lastQuery();

    if (!query.exec()) {
        qDebug() << "Failed to fill initial data in global_attrs table";
        qDebug() << "Query was the following: " << getLastExecutedQuery(query);
        qDebug() << db.lastError().text();
        return false;
    } else {
        return true;
    }

    return true;
}
