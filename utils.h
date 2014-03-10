#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QByteArray>
#include <QSqlDatabase>

#include "dictglobalattributes.h"

void readAndCompress(const QString & filename, QByteArray &compressedResult);
void errorMsg (const QString& error_msg);
bool confirmationMsg (const QString& question, QWidget * parent);

QString createDirectory(const QString & dictFilename,  const QString & postfix);
bool extractSound(const QString& dest, const QString& name, const QByteArray& compressed);
bool checkExistance(const QVariant& id, const QString& table, QSqlDatabase& db);
bool deleteAttachment(const QVariant& descriptionid, const QString& table, QSqlDatabase& db);
bool unlinkEtymology(const QVariant& wordid, const QVariant& tag, QSqlDatabase& db, QSet<DictGlobalAttributes> * dictsOpened);
bool deleteWord(const QVariant& wordid, const QVariant& tag, QSqlDatabase& db, QSet<DictGlobalAttributes> * dictsOpened);


#endif // UTILS_H
