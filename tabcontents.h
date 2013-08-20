#ifndef TABCONTENTS_H
#define TABCONTENTS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include "dictglobalattributes.h"
#include "dictentry.h"
#include "editablesqlmodel.h"

namespace Ui {
  class TabContents;
}

class TabContents : public QWidget
{
  Q_OBJECT
  DictGlobalAttributes dictAttrs;
  QSqlDatabase db;
  QSqlTableModel *dictModel;
  QSqlTableModel *blobsModel;

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
  bool filterBlobs();

private:
  Ui::TabContents *ui;
  DictEntry readFields();
  void clearForms(void);
  void initializeDictModel(QSqlTableModel *model);
  void initializeBlobsModel(QSqlTableModel *model);

};

#endif // TABCONTENTS_H
