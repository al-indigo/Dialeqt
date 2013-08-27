#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QByteArray>

void readAndCompress(const QString & filename, QByteArray &compressedResult);
void errorMsg (const QString& error_msg);
QString createDirectory(const QString & dictFilename,  const QString & postfix);
bool extractSound(const QString& dest, const QString& name, const QByteArray& compressed);


#endif // UTILS_H
