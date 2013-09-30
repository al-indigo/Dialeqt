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

QString DictGlobalAttributes::getCoauthorsQString() {
  QString coAuthors;
  foreach (const QString &item, this->getCoauthors()) {
      coAuthors.append(item + QString(","));
  }

/* WARNING! It's a workaround: my QT build doesn't support native placeholders (WTF!) so the following lines should be deleted when I fix it
  coAuthors.prepend("\"");
  coAuthors.append("\"");*/
  return coAuthors;
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

QString DictGlobalAttributes::getTagsQString() {
  QString tagsstring;
  foreach (const QString &item, this->getTags()) {
      tagsstring.append(item + QString(","));
  }
  /* WARNING! It's a workaround: my QT build doesn't support native placeholders (WTF!) so the following lines should be deleted when I fix it
  tagsstring.prepend("\"");
  tagsstring.append("\"");*/
  return tagsstring;
}
