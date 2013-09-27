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

void DictGlobalAttributes::setCoauthors(QStringList _coAuthors)
{
  this->coauthors.clear();
  foreach (const QString &item, _coAuthors) {
  if(0 != item.capacity()) {
    this->coauthors.insert(item.simplified());
    }
  }
}

void DictGlobalAttributes::setTags(QStringList _tags)
{
  this->tags.clear();
  foreach (const QString &item, _tags) {
      if(0 != item.capacity()) {
        this->tags.insert(item.simplified());
      }
  }
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
