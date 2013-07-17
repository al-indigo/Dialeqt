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
    QString legend;
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
                                                            description(_description),
                                                            legend("NO_LEGEND") {}
             DictGlobalAttributes(const DictGlobalAttributes &dictCopy) :
               dbId(dictCopy.dbId),
               filename(dictCopy.filename),
               dictname(dictCopy.dictname),
               author(dictCopy.author),
               coauthors(dictCopy.coauthors),
               tags(dictCopy.tags),
               description(dictCopy.description) {}

             friend bool operator==(const DictGlobalAttributes &left, const DictGlobalAttributes &right) {
               if (left.dbId != right.dbId) {
                   return false;
                 }
               return true;
             }

             void debugPrint();
             QString getDbId(void) const { return dbId; }
             QString getFilename(void) const { return filename; }
             QString getDictname(void) const { return dictname; }
             QString getAuthor(void) const {return author; }
             QSet <QString> getCoauthors(void) const { return coauthors; }
             QSet <QString> getTags(void) const { return tags; }
             QString getDescription(void) const { return description; }
             QString getLegend(void) const { return legend; }

signals:
    
public slots:

private:

    
};

#endif // DICTGLOBALATTRIBUTES_H
