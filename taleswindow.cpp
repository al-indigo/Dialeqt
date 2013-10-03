#include "taleswindow.h"
#include "ui_taleswindow.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSortFilterProxyModel>
#include <QFileInfo>
#include <QFileDialog>
#include "utils.h"
#include "customquerydiagnostics.h"


TalesWindow::TalesWindow(QSqlDatabase _db, QWidget *parent) :
  db(_db),
  currentTaleId(1),
  init(true),
  QDialog(parent),
  ui(new Ui::TalesWindow)
{
  ui->setupUi(this);

  talesReinit();
  loadCurrentTale();

  connect(ui->submitNewTale, SIGNAL(clicked()), this, SLOT(addTale()));
  connect(this, SIGNAL(finished(int)), ui->talesList, SLOT(update()));
  connect(ui->close, SIGNAL(clicked()), this, SLOT(saveCurrentTale()));
  connect(ui->talesList, SIGNAL(currentIndexChanged(int)), this, SLOT(loadCurrentTale()));
  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveCurrentTale()));
  connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
  connect(ui->attachFile, SIGNAL(clicked()), this, SLOT(uploadFile()));
}

void TalesWindow::talesReinit() {
  QSqlQuery query_select(db);
  query_select.exec("SELECT tale_name, id from fairy_tales order by id;");
  ui->talesList->clear();
  int i = 0;
  while (query_select.next()) {
      qDebug() << "Inserting item # " << i << ": " << query_select.value(0).toString();
      ui->talesList->addItem(query_select.value(0).toString(), query_select.value(1));
      i++;
  }
  ui->talesList->setCurrentIndex(i-1);
  currentTaleId = ui->talesList->currentIndex();

}

bool TalesWindow::loadCurrentTale() {
  if (!init) {
      saveCurrentTale();
  } else {
      init = false;
  }
  ui->taleText->clear();
  QSqlQuery query_loadtext(db);
  query_loadtext.prepare("SELECT tale FROM fairy_tales WHERE id = :id;");
  query_loadtext.bindValue(":id", ui->talesList->itemData(currentTaleId));
  if (!query_loadtext.exec()) {
      qDebug() << "Can't insert new fairy tale";
      qDebug() << "Query was the following: " << getLastExecutedQuery(query_loadtext);
      qDebug() << db.lastError().text();
      ui->talesName->clear();
      errorMsg("Не удалось загрузить текст истории, сложно сказать по какой причине (пришлите разработчику файл базы, чтобы можно было понять, в чем дело)");
      currentTaleId = ui->talesList->currentIndex();
      return false;
  }
  if (query_loadtext.next()) {
      ui->taleText->setPlainText(query_loadtext.value(0).toString());
  }
  currentTaleId = ui->talesList->currentIndex();
  return true;
}

bool TalesWindow::saveCurrentTale() {
  QSqlQuery query_savetext(db);
  query_savetext.prepare("UPDATE fairy_tales SET tale = :tale WHERE id = :id;");
  query_savetext.bindValue(":tale", ui->taleText->toPlainText());
  query_savetext.bindValue(":id", ui->talesList->itemData(currentTaleId));
  if (!query_savetext.exec()) {
      qDebug() << "Can't insert new fairy tale";
      qDebug() << "Query was the following: " << getLastExecutedQuery(query_savetext);
      qDebug() << db.lastError().text();
      ui->talesName->clear();
      errorMsg("Текст истории не сохранился, сложно сказать по какой причине (пришлите разработчику файл базы, чтобы можно было понять, в чем дело)");
      currentTaleId = ui->talesList->currentIndex();
      return false;
  }
  currentTaleId = ui->talesList->currentIndex();
  return true;
}

bool TalesWindow::uploadFile() {
  QString filename = QFileDialog::getOpenFileName(this, tr("Выберите файл для вставки в историю"), "", tr("Медиафайлы (*.mp3 *.wav *.ogg *.TextGrid *.mp4 *.gif *.png *.jpg *.JPEG *.BMP *.tiff *.flac)"));

  if (filename.isEmpty()) return false;

  qDebug() << "file selected: " << filename;
  QByteArray blob;
  readAndCompress(filename, blob);
  QFileInfo fileinfo = QFileInfo(filename);

  QSqlQuery blobQuery(db);
  blobQuery.prepare("INSERT INTO tales_blobs(tale_id,filename,blob) values (:tale_id, :filename, :blob) ;");
  blobQuery.bindValue(":tale_id", currentTaleId);
  blobQuery.bindValue(":filename", fileinfo.fileName());
  blobQuery.bindValue(":blob", blob);
  if (!blobQuery.exec()) {
      qDebug() << "Can't insert file to tale";
      qDebug() << "Query was the following: " << getLastExecutedQuery(blobQuery);
      qDebug() << db.lastError().text();
      errorMsg("Файл не удалось вставить, сложно сказать по какой причине (пришлите разработчику файл базы и файл, который вы пытались вставить, чтобы можно было понять, в чем дело)");

      return false;
  }
  return true;
}

bool TalesWindow::addTale() {
  currentTaleId = ui->talesList->currentIndex();
  if (ui->talesName->text().isEmpty()) {
      errorMsg("Вы не можете добавить историю без названия");
      return false;
  } else {
      QSqlQuery insertTale(db);
      insertTale.prepare("INSERT INTO fairy_tales(tale_name) values (:tale_name);");
      insertTale.bindValue(":tale_name", QVariant(ui->talesName->text()));
      if (!insertTale.exec()) {
          qDebug() << "Can't insert new fairy tale";
          qDebug() << "Query was the following: " << getLastExecutedQuery(insertTale);
          qDebug() << db.lastError().text();
          ui->talesName->clear();
          errorMsg("История не добавилась, сложно сказать по какой причине (пришлите разработчику файл базы, чтобы можно было понять, в чем дело)");
          return false;
      }
      saveCurrentTale();

      ui->taleText->clear();
      ui->talesName->clear();

      talesReinit();

      return true;
  }
}

void TalesWindow::initializeAttachModel(QSqlTableModel *model) {
  model->setTable("tales_blobs");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  qDebug() << currentTaleId;

  model->setFilter(QString("tale_id = ").append(QString(currentTaleId)));

  qDebug() << model->query().lastQuery();
  model->select();
  qDebug() << model->query().lastQuery();
  qDebug() << model->query().result();
}

TalesWindow::~TalesWindow()
{
  delete ui;
}
