#include "addconnection.h"
#include "ui_addconnection.h"

#include <QDebug>
#include "utils.h"
#include "customquerydiagnostics.h"

AddConnection::AddConnection(QVariant _wordid, QVariant _word_transcription, QVariant _etimology_tag, QSqlDatabase _initialWordDB, QSqlDatabase _acceptorWordDB, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent) :
  initialWordID(_wordid),
  initialWordTranscription(_word_transcription),
  tag(_etimology_tag),
  initialWordDB(_initialWordDB),
  acceptorWordDB(_acceptorWordDB),
  dictsOpened(_dictsOpened),
  QDialog(parent),
  ui(new Ui::addConnection)
{
  ui->setupUi(this);
  model = new QSqlQueryModel();
  model->setQuery("SELECT dictionary.id, \
                  dictionary.transcription, \
                  dictionary.translation \
                  FROM \
                  dictionary \
                  WHERE dictionary.etimology_tag is NULL;", acceptorWordDB);
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Перевод"));

  ui->tableView->setModel(model);
  ui->tableView->setColumnWidth(1, 100);
  ui->tableView->setColumnWidth(2, 180);
  ui->tableView->setColumnHidden(0, true);

  connect(ui->acceptConnection, SIGNAL(clicked()), this, SLOT(connectWords()));
}

bool AddConnection::connectWords()
{
  QItemSelectionModel *select = ui->tableView->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого необходимо нужно добавить связь");
      return false;
  }
  QModelIndex index = ui->tableView->currentIndex();
  QVariant wordid = index.sibling(index.row(),0).data();
  qDebug() << wordid;

  QSqlQuery update_initial(initialWordDB);
  update_initial.prepare("UPDATE dictionary SET etimology_tag = :tag WHERE id = :id;");
  update_initial.bindValue(":tag", tag);
  update_initial.bindValue(":id", initialWordID);
  if (!update_initial.exec()) {
      qDebug() << "Can't update tag for initial word";
      qDebug() << "Query was the following: " << getLastExecutedQuery(update_initial);
      qDebug() << initialWordDB.lastError().text();
      errorMsg("Связь не проставилась на первом этапе, расскажите об этом разработчику и скопируйте транскрипции слов, которые вы пытались связать");
      return false;
  }

  return true;
}

AddConnection::~AddConnection()
{
  delete model;
  delete ui;
}
