#ifndef CUSTOMQUERYDIAGNOSTICS_H
#define CUSTOMQUERYDIAGNOSTICS_H

#include <QSqlQuery>

QString getLastExecutedQuery(const QSqlQuery& query);

#endif // CUSTOMQUERYDIAGNOSTICS_H
