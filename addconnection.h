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
//  QVariant initialWordID;
//  QVariant initialWordTranscription;
  QVariant tag;
//  QSqlDatabase initialWordDB;
  QSqlDatabase acceptorWordDB;
  QSet<DictGlobalAttributes> * dictsOpened;
  QSqlQueryModel * model;
  QList<QPair<QString, QVariant>> wordsToConnect;

public:
  explicit AddConnection(QList<QPair<QString, QVariant>> _wordsToConnect, QVariant _etimology_tag, QSqlDatabase _acceptorWordDB, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent = 0);
  ~AddConnection();

private slots:
  bool cyclicConnector();
private:
  Ui::addConnection *ui;
  bool connectWords(QSqlDatabase initialWordDB, QVariant initialWordID);
};

#endif // ADDCONNECTION_H
