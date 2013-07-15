#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include "dictdbfactory.h"

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
    QSqlQuery query(db);

    /*===== Creating tables =========*/

    /* table dict_attribues holds info about dictionary itself */
    //TODO: look for consonant strings
    //TODO: check out how to implement versioning support
    query.exec("create table dict_attributes (id int primary key, "
               "dict_identicator varchar(100), dict_name varchar(100), "
               "dict_author varchar(100), dict_coauthors varchar(2000), "
               "dict_classification_tags varchar(2000), "
               "dict_description varchar(3000), dict_legend varchar(100000)");

    /* table dictionary stores the dictionary and links to foreign blobs */
    query.exec("create table dictionary (id int primary key, "
               "word varchar(100), regular_form varchar(100), transcription varchar(100), "
               "translation varchar(200), link_to_sounds_list int, link_to_praat_markup int, "
               "is_a_regular_form bool, has_paradigm bool) ");

    /* table phonology */

    /* table fairy_tails */

    /* table blobs_table */
    query.exec("create table blobs_table (id int primary key, "
               "type int, object blob)" );

    /*======== Filling initial data ==============*/

    query.prepare("insert into dict_attributes (id, "
                  "dict_identicator, dict_name, "
                  "dict_author, dict_coathors, "
                  "dict_classification_tags, "
                  "dict_description, dict_legend "
                  "values(0, "
                  ":dictId, :dictName, :dictAuthor, :dictCoathors, "
                  ":dictTags, "
                  ":dictDescription, :dictLegend)");
    query.bindValue(":dictId", attrs.getDbId());
    query.bindValue(":dictName", attrs.getDictname());
    query.bindValue(":dictAuthor", attrs.getAuthor());
    //TODO: make set serialization/deserialization
    query.bindValue(":dictCoathors", *attrs.getCoauthors().begin());
    query.bindValue(":dictTags", *attrs.getTags().begin());

    query.bindValue(":dictDescription", attrs.getDescription());
    query.bindValue(":dictLegend", attrs.getLegend());
    if (!query.exec()) {
        qDebug() << "DictDB creation failed";
        db.lastError();
        return false;
    } else {
        return true;
    }


//return connection;
}
