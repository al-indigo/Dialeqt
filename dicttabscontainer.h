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

  void initializeDictModel(QSqlRelationalTableModel *model);
public:
  explicit DictTabsContainer(QWidget *parent = 0);
  bool createDictTabInitial(DictGlobalAttributes dictAttrs);
  bool openDictTabInitial(DictGlobalAttributes & dictAttrs, const QString& filename);
  bool createTab(DictGlobalAttributes dictAttrs);
signals:
  
public slots:
  bool remove(int tab);
};

#endif // DICTTABSCONTAINER_H
