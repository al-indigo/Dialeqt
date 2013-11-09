#ifndef ADDCONNECTION_H
#define ADDCONNECTION_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QString>

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
//  QSqlQueryModel * model;
  QSqlTableModel * model;
  QList<QPair<QString, QVariant>> wordsToConnect;
  QList<QPair<QString, QVariant>> wordsToConnectWith;
  QSet<QString> connectionNamesForDicts;
  QVariant selectedTag;
  QVariant selectedWordid;
  QString filter;

public:
  explicit AddConnection(QList<QPair<QString, QVariant>> _wordsToConnect, QVariant _etimology_tag, QSqlDatabase _acceptorWordDB, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent = 0);
  ~AddConnection();

private slots:
  bool cyclicConnector();
  void modelSetup();
private:
  Ui::addConnection *ui;
  bool connectWords(QSqlDatabase initialWordDB, QVariant initialWordID, QSqlDatabase updatedWordDB, QVariant updatedWordID);
  bool checkConnectedDatabases();
  bool prepareForConnection();

};

#endif // ADDCONNECTION_H
