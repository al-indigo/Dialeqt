#ifndef DICTTABSCONTAINER_H
#define DICTTABSCONTAINER_H

#include <QTabWidget>
#include <QSet>
#include <QSqlRelationalTableModel>
#include "dictglobalattributes.h"
#include "dictdbfactory.h"

class DictTabsContainer : public QTabWidget
{
  Q_OBJECT
  DictDbFactory dbFactory;
  QSet<DictGlobalAttributes> dictsOpened;
  QMap<QString, int> dictsOpenedMap;

  void initializeDictModel(QSqlRelationalTableModel *model);
public:
  explicit DictTabsContainer(QWidget *parent = 0);
  ~DictTabsContainer();
  bool createDictTabInitial(DictGlobalAttributes dictAttrs);
  bool openDictTabInitial(DictGlobalAttributes & dictAttrs, const QString& filename);
  bool createTab(DictGlobalAttributes dictAttrs);
  bool isDictOpened(QString dbId);
  const QSet<DictGlobalAttributes> * getDictsOpened() { return &dictsOpened; }
  void removeDictFromSet(DictGlobalAttributes& dict);
signals:
  bool goToWord(QVariant id);
  
public slots:
  bool remove(int tab);
  bool goToDictAndWord(QString dictName, QVariant id);
};

#endif // DICTTABSCONTAINER_H
