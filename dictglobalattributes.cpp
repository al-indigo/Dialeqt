#include "dictglobalattributes.h"
#include <QDebug>

DictGlobalAttributes::DictGlobalAttributes(QObject *parent) :
    QObject(parent)
{

}

void DictGlobalAttributes::debugPrint()
{
  qDebug() << dbId;
  qDebug() << filename;
  qDebug() << dictname;
  qDebug() << author;
  qDebug() << coauthors;
  qDebug() << tags;
  qDebug() << description;
}
