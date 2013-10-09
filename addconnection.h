#ifndef ADDCONNECTION_H
#define ADDCONNECTION_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "dictglobalattributes.h"

namespace Ui {
  class addConnection;
}

class AddConnection : public QDialog
{
  Q_OBJECT
  QVariant initialWordID;
  QVariant initialWordTranscription;
  QVariant tag;
  QSqlDatabase initialWordDB;
  QSqlDatabase acceptorWordDB;
  QSet<DictGlobalAttributes> * dictsOpened;
  QSqlQueryModel * model;

public:
  explicit AddConnection(QVariant _wordid, QVariant _word_transcription, QVariant _etimology_tag, QSqlDatabase _initialWordDB, QSqlDatabase _acceptorWordDB, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent = 0);
  ~AddConnection();

private slots:
  bool connectWords();
  
private:
  Ui::addConnection *ui;
};

#endif // ADDCONNECTION_H
