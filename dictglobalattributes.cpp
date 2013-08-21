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
/*
DictGlobalAttributes& DictGlobalAttributes::operator=(const DictGlobalAttributes& dictCopy)
{
              dbId = dictCopy.dbId;
              filename = dictCopy.filename;
              dictname(dictCopy.dictname);
              author(dictCopy.author);
              coauthors(dictCopy.coauthors);
              tags(dictCopy.tags);
              description(dictCopy.description);
              return *this;
}
*/
