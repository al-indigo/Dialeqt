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
protected:
  Q_OBJECT
  DictGlobalAttributes dictAttrs;
  QSqlDatabase db;
  QSqlTableModel *dictModel;
  QSqlTableModel *soundsModel;
  QSqlTableModel *praatModel;

  bool soundChosen;
  bool praatChosen;
  bool praatRightChosen;

  QString soundFilename;
  QString praatFilenameMarkup;
  QString praatFilenameSound;

  QString praatRightFormFilenameMarkup;
  QString praatRightFormFilenameSound;
  QString praatRightFormDescription;

   QSet<DictGlobalAttributes> * dictsOpened;
public:
  explicit TabContents(DictGlobalAttributes _dictAttrs,  QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent = 0);
  ~TabContents();

  DictGlobalAttributes & getDictAttrs();

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
  bool chooseSound();
  bool choosePraat();
  bool choosePraatRight();
  bool checkPraatRightDescription();
  bool submitRightPraat();
  bool sendToPraat();
  bool updateModel();
  virtual bool showFiles(bool isShown=true);
  virtual bool showForms(bool isShown=true);
  virtual bool showActionsOnDict(bool isShown=true);
  virtual bool showActionsOnWord(bool isShown=true);

protected:
  Ui::TabContents *ui;
  DictEntry readFields();
  void clearForms(void);
  void initializeDictModel(QSqlTableModel *model);
  void initializeSoundsModel(QSqlTableModel *model);
  void initializePraatModel(QSqlTableModel *model);
  bool isValidInput(void);

};

#endif // TABCONTENTS_H
