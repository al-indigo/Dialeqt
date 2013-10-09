#ifndef ETIMOLOGYWINDOW_H
#define ETIMOLOGYWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "dictglobalattributes.h"

namespace Ui {
  class EtimologyWindow;
}

class EtimologyWindow : public QDialog
{
  Q_OBJECT
  QVariant wordid;
  QVariant wordtranscription;
  QVariant tag;
  QSqlDatabase db;
  QSqlQueryModel *model;
  QSqlQueryModel *wordsmodel;
  QSet<DictGlobalAttributes> * dictsOpened;
public:
  explicit EtimologyWindow(QVariant _wordid, QVariant _word_transcription, QVariant _etimology_tag, QSqlDatabase _currentDb, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent = 0);
  ~EtimologyWindow();

private slots:
  bool findWords();
  bool openDbAndAddConnection();
private:
  Ui::EtimologyWindow *ui;
};

#endif // ETIMOLOGYWINDOW_H
