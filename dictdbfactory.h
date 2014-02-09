#ifndef DICTDBFACTORY_H
#define DICTDBFACTORY_H

#include <QObject>
#include <QSqlDatabase>
#include "dictglobalattributes.h"


/* this factory makes QSql connections in terms of qt 5.1.
 * We assume using QSQLITE3 database but it can be easily
 *switched to another RDBMS */


class DictDbFactory : public QObject
{
    Q_OBJECT
public:
    explicit DictDbFactory(QObject *parent = 0);

    
     QSqlDatabase createConnection(const QString &connectionName); //this func creates connection to db
     bool initDb(DictGlobalAttributes &attrs, const QSqlDatabase &db);

signals:
    
public slots:
    
};

#endif // DICTDBFACTORY_H
