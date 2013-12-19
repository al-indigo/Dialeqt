#ifndef PARADIGMWINDOW_H
#define PARADIGMWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include "dictglobalattributes.h"
#include "dictentry.h"
#include "editablesqlmodel.h"

namespace Ui {
  class ParadigmWindow;
}

class ParadigmWindow : public QDialog
{
protected:
  Q_OBJECT
  DictGlobalAttributes dictAttrs;
  QSqlDatabase db;
  QSqlTableModel *dictModel;
  QSqlTableModel *soundsModel;
  QSqlTableModel *praatModel;

  QVariant wordIdSelected;
  QVariant wordSelected;
  QVariant transcriptionSelected;
  QVariant transcriptionSelectedID;
  QVariant translationSelected;

  bool soundChosen;
  bool praatChosen;
  bool praatRightChosen;

  QString soundFilename;
  QString praatFilenameMarkup;
  QString praatFilenameSound;

  QString praatRightFormFilenameMarkup;
  QString praatRightFormFilenameSound;
  QString praatRightFormDescription;
public:
  explicit ParadigmWindow(DictGlobalAttributes _dictAttrs, QVariant _wordIdSelected, QVariant _wordSelected, QVariant _transcriptionSelected, QVariant _translationSelected, QWidget *parent = 0);
  ~ParadigmWindow();

signals:

public slots:
  bool submitWord();
  bool showPlay();
  bool filterBlobs();
  bool chooseSound();
  bool choosePraat();
  bool choosePraatRight();
  bool checkPraatRightDescription();
  bool submitRightPraat();

protected:
  Ui::ParadigmWindow *ui;
  DictEntry readFields();
  void clearForms(void);
  void initializeDictModel(QSqlTableModel *model);
  void initializeSoundsModel(QSqlTableModel *model);
  void initializePraatModel(QSqlTableModel *model);
  bool isValidInput(void);

};


#endif // PARADIGMWINDOW_H
