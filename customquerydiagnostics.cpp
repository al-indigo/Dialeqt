#include "customquerydiagnostics.h"
#include <QSqlDriver>
#include <QSqlError>
#include <QString>
#include <QMap>
#include <QVariant>

QString getLastExecutedQuery(const QSqlQuery& query) {
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext()) {
        it.next();
        str.replace(it.key(), it.value().toString());
    }
    return str;
}
