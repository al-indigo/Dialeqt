#ifndef TABCONTENTS_H
#define TABCONTENTS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include "dictglobalattributes.h"
#include "dictentry.h"

namespace Ui {
  class TabContents;
}

class TabContents : public QWidget
{
  Q_OBJECT
  DictGlobalAttributes dictAttrs;
  QSqlDatabase db;
  QSqlRelationalTableModel *model;

public:
  explicit TabContents(DictGlobalAttributes _dictAttrs, QWidget *parent = 0);
  ~TabContents();

signals:

public slots:
  bool submitWord();
  bool showLegend();
  bool showEtimology();
  bool showParadigm();
  bool showPlay();
  bool showPhonology();
  bool showTales();

private:
  Ui::TabContents *ui;
  DictEntry TabContents::readFields();
  void clearForms(void);
  void TabContents::initializeDictModel(QSqlRelationalTableModel *model);
};

#endif // TABCONTENTS_H
