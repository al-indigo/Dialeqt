#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include "tabcontents.h"
#include "customquerydiagnostics.h"
#include "legendwindow.h"
#include "etimologywindow.h"
#include "phonologywindow.h"
#include "taleswindow.h"
#include "paradigmwindow.h"
#include "playwindow.h"

#include "ui_tabcontents.h"


TabContents::TabContents(DictGlobalAttributes _dictAttrs, QWidget *parent) :
  dictAttrs(_dictAttrs),
  QWidget(parent),
  db(QSqlDatabase::database(dictAttrs.getFilename())),
  ui(new Ui::TabContents)
{
  ui->setupUi(this);
/*
 *      QSqlRelationalTableModel *model = new QSqlRelationalTableModel(this, QSqlDatabase::database(dictAttrs.getFilename()));
      this->initializeDictModel(model);

      QTableView *view = new QTableView;
      view->setModel(model);
      view->setItemDelegate(new QSqlRelationalDelegate(view));


*/
//  db(QSqlDatabase::database(dictAttrs.getFilename()));

  if (!db.isValid()) {
      qDebug() << "Error during tab creation: Database is " << db.isOpen() << " and it's because " << db.isOpenError();
      return;
  }

  
//  connect(ui->soundButton, SIGNAL(clicked()), this, SLOT(chooseSound()));
  connect(ui->submitButton , SIGNAL(clicked()), this, SLOT(submitWord()));
  connect(ui->legendButton , SIGNAL(clicked()), this, SLOT(showLegend()));
  connect(ui->etimologyButton , SIGNAL(clicked()), this, SLOT(showEtimology()));
  connect(ui->paradigmButton , SIGNAL(clicked()), this, SLOT(showParadigm()));
  connect(ui->playButton , SIGNAL(clicked()), this, SLOT(showPlay()));
  connect(ui->phonologyButton , SIGNAL(clicked()), this, SLOT(showPhonology()));
  connect(ui->talesButton , SIGNAL(clicked()), this, SLOT(showTales()));
//  dictModel = new QSqlRelationalTableModel(this, db);
  dictModel = new QSqlTableModel(this, db);
  blobsModel = new QSqlTableModel(this, db);
  initializeBlobsModel(blobsModel);
  initializeDictModel(dictModel);


//  QTableView *view = new QTableView;
//  view->setModel(model);
//  view->setItemDelegate(new QSqlRelationalDelegate(view));

  ui->dictionaryTable->setModel(dictModel);
  ui->dictionaryTable->setColumnHidden(0,true);
  ui->dictionaryTable->setColumnHidden(2,true);
  ui->dictionaryTable->setColumnHidden(5,true);
  ui->dictionaryTable->setColumnHidden(6,true);
  ui->dictionaryTable->setColumnHidden(7,true);
  ui->dictionaryTable->setSortingEnabled(true);
  ui->dictionaryTable->setColumnWidth(1, 120 );
  ui->dictionaryTable->setColumnWidth(3, 240);
  ui->dictionaryTable->setColumnWidth(4, 500);

  ui->listView->setModel(blobsModel);
  ui->listView->setModelColumn(2);

  connect(ui->dictionaryTable , SIGNAL(clicked(QModelIndex)), this, SLOT(filterBlobs()));

//  ui->dictionaryTable->setItemDelegate(new QSqlRelationalDelegate(ui->dictionaryTable));
}

//bool TabContents::chooseSound() {
  
//}

bool TabContents::showLegend() {
  LegendWindow legend(this);
  legend.exec();
  return true;
}

bool TabContents::showEtimology() {
  EtimologyWindow etimology(this);
  etimology.exec();
  return true;
}

bool TabContents::showParadigm() {
  ParadigmWindow paradigm(this);
  paradigm.exec();
  return true;
}

bool TabContents::showPlay() {
  PlayWindow play(this);
  play.exec();
  return true;
}

bool TabContents::showPhonology() {
  PhonologyWindow phonology(this);
  phonology.exec();
  return true;
}

bool TabContents::showTales() {
  TalesWindow tales(this);
  tales.exec();
  return true;
}


TabContents::~TabContents()
{
  delete dictModel;
  delete blobsModel;
  delete ui;
}

void TabContents::initializeDictModel(QSqlTableModel *model) {
  model->setTable("dictionary");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Слово"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Регулярная форма"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Перевод"));

//  model->setRelation(0, QSqlRelation("dict_blobs_table", "name", "wordid"));
//  model->setFilter("relTblAl_0.'wordid' = dictionary.'id'");
//  model->setRelation(6, QSqlRelation("dict_blobs_table", "wordid", "description"));
  qDebug() << model->query().lastQuery();
  model->select();
  qDebug() << model->query().lastQuery();
  qDebug() << model->query().result();
}

bool TabContents::filterBlobs() {
  qDebug() << "filtering blobs";
  QString tmp = "dict_blobs_table.wordid = ";

  QModelIndexList test = ui->dictionaryTable->selectionModel()->selection().indexes();
  int row = test.at(0).row();
  this->dictModel->data(this->dictModel->index(row,0));
  QString param = this->dictModel->data(this->dictModel->index(row,0)).toString();
  tmp.append(param);
  const QString filter(tmp);
  blobsModel->setFilter(filter);
  qDebug() << blobsModel->query().lastQuery();
  qDebug() << blobsModel->query().result();

  return true;
}

void TabContents::initializeBlobsModel(QSqlTableModel *model) {
  model->setTable("dict_blobs_table");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Тип"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Имя"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Описание"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Объект"));
  model->setHeaderData(5, Qt::Horizontal, QObject::tr("Ссылка на слово"));

  model->select();

  /* testing purposes */
  int row = model->rowCount();
  model->insertRows(row,1);
  model->setData(model->index(row, 1), 1);
  model->setData(model->index(row, 2), "blob1");
  model->setData(model->index(row, 3), " первого типа со ссылкой на первый пункт");
  model->setData(model->index(row, 4), "выаываывыва");
  model->setData(model->index(row, 5), 1);

  model->submitAll();
  model->database().commit();

  row = model->rowCount();
  model->insertRows(row,1);
  model->setData(model->index(row, 1), 1);
  model->setData(model->index(row, 2), "blob2");
  model->setData(model->index(row, 3), " первого типа без ссылки");
  model->setData(model->index(row, 4), "выаываывыва");
  model->setData(model->index(row, 5), NULL);

  model->submitAll();
  model->database().commit();

  row = model->rowCount();
  model->insertRows(row,1);
  model->setData(model->index(row, 1), 1);
  model->setData(model->index(row, 2), "blob3");
  model->setData(model->index(row, 3), " первого типа со ссылкой на первый пункт");
  model->setData(model->index(row, 4), "выаываывsdaыва");
  model->setData(model->index(row, 5), 1);

  model->submitAll();
  model->database().commit();

  row = model->rowCount();
  model->insertRows(row,1);
  model->setData(model->index(row, 1), 2);
  model->setData(model->index(row, 2), "blob4");
  model->setData(model->index(row, 3), " второго типа без ссылки");
  model->setData(model->index(row, 4), "выаываывыва");
  model->setData(model->index(row, 5), 12);

  model->submitAll();
  model->database().commit();

  model->select();

}

DictEntry TabContents::readFields()
{
  QString word(ui->wordForm->text());
  QString transcription(ui->transcritptionForm->text());
  QString translation(ui->translationForm->text());
  QString soundFilename;
  QPair <QString, QString> praatPair;

  DictEntry entry(this, word, transcription, translation, soundFilename, praatPair.first, praatPair.second);
  return entry;
}

bool TabContents::submitWord()
{
  /* check that we have at least one filled field */

  DictEntry entry(readFields());
  if (entry.isEmpty()) {
      QMessageBox errormsg;
      errormsg.setText("Вы должны заполнить хотя бы одно из текстовых полей для создания слова");
      errormsg.setIcon(QMessageBox::Critical);
      errormsg.setDefaultButton(QMessageBox::Ok);
      errormsg.exec();
      return false;
    }
  qDebug() << "Now we should get an entry to sqlite";


  QSqlRecord record = dictModel->record();
  record.setValue(1, entry.getWord());
  qDebug() << "after set data: " << dictModel->query().lastQuery();
  record.setValue(2, entry.getTranscription());
  record.setValue(3, entry.getTranscription());
  record.setValue(4, entry.getTranslation());
  record.setValue(5, true);
  record.setValue(6, false);
  dictModel->insertRecord(-1, record);



  dictModel->submitAll();
  qDebug() << dictModel->query().lastQuery();
//  dictModel->database().commit();
  qDebug() << "Executed the query on commit: " << dictModel->query().lastQuery();
  dictModel->select();

  this->clearForms();
  return true;
}

void TabContents::clearForms() {
  ui->wordForm->clear();
  ui->transcritptionForm->clear();
  ui->translationForm->clear();
}
