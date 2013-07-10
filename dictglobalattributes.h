#ifndef DICTGLOBALATTRIBUTES_H
#define DICTGLOBALATTRIBUTES_H

#include <QObject>
#include <QSet>

class DictGlobalAttributes : public QObject
{
    Q_OBJECT
    QString dbId;
    QString filename;
    QString dictname;
    QString author;
    QSet <QString> coauthors;
    QSet <QString> tags;
    QString description;
public:
    explicit DictGlobalAttributes(QObject *parent = 0);
             DictGlobalAttributes(QString _dbId,
                                  QString _filename,
                                  QString _dictname,
                                  QString _author,
                                  QSet <QString> _coauthors,
                                  QSet <QString> _tags,
                                  QString _description ) :
                                                            dbId(_dbId),
                                                            filename(_filename),
                                                            dictname(_dictname),
                                                            author(_author),
                                                            coauthors(_coauthors),
                                                            tags(_tags),
                                                            description(_description) {}
             DictGlobalAttributes(const DictGlobalAttributes &dictCopy) :
               dbId(dictCopy.dbId),
               filename(dictCopy.filename),
               dictname(dictCopy.dictname),
               author(dictCopy.author),
               coauthors(dictCopy.coauthors),
               tags(dictCopy.tags),
               description(dictCopy.description) {}

             void debugPrint();
signals:
    
public slots:

private:

    
};

#endif // DICTGLOBALATTRIBUTES_H
