#ifndef SEARCH_H
#define SEARCH_H

#include <QDialog>
#include <QLayout>

#include "dictglobalattributes.h"


namespace Ui {
  class search;
}

class Search : public QDialog
{
  Q_OBJECT
  const QSet<DictGlobalAttributes> * openedDicts;
  QGridLayout dictsToSearch;
  QGridLayout * layout;

public:
  explicit Search(const QSet<DictGlobalAttributes> * attrs, QWidget *parent = 0);
  ~Search();

private slots:
  bool onSearchClick();

private:
  Ui::search *ui;
  bool checkQuery(QString line);
  bool checkVars(QString line);
  QMap<QString, QList<QString> > parseVars(QString line);
  QString constructQuery(int dict_size);
};

#endif // SEARCH_H
