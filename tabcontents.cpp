#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include "tabcontents.h"
#include "customquerydiagnostics.h"

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

  connect(ui->submitButton , SIGNAL(clicked()), this, SLOT(submitWord()));

  model = new QSqlRelationalTableModel(this, db);
  initializeDictModel(model);
  ui->dictionaryTable->setModel(model);

}

TabContents::~TabContents()
{
  delete model;
  delete ui;
}

void TabContents::initializeDictModel(QSqlRelationalTableModel *model) {
  model->setTable("dictionary");
  model->setEditStrategy(QSqlTableModel::OnRowChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Слово"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Регулярная форма"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Перевод"));
  model->setHeaderData(5, Qt::Horizontal, QObject::tr("Звуковой файл"));
  model->setHeaderData(6, Qt::Horizontal, QObject::tr("Праат"));

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
      errormsg.setText("Вы должны заполнить хотя бы из текстовых полей для создания слова");
      errormsg.setIcon(QMessageBox::Critical);
      errormsg.setDefaultButton(QMessageBox::Ok);
      errormsg.exec();
      return false;
    }
  qDebug() << "Now we should get an entry to sqlite";

/*  QSqlQuery query(db);
  query.prepare("insert into dictionary("
                   "word, "
                   "regular_form, "
                   "transcription, "
                   "translation, "
                   "is_a_regular_form, "
                   "has_paradigm ) "
                 "values ("
                   ":word, "
                   ":transcription, "
                   ":transcription, "
                   ":translation, "
                   "1, "
                   "0 )"
                 );
  query.bindValue(":word", entry.getWord());
  query.bindValue(":transcription", entry.getTranscription());
  query.bindValue(":translation", entry.getTranslation());

  if (!query.exec()) {
      qDebug() << "Failed to add word";
      qDebug() << "Query was the following: " << getLastExecutedQuery(query);
      qDebug() << db.lastError().text();

      this->clearForms();
      return false;
  }
  */
  int row = model->rowCount();
  qDebug() << "rowcount " << row;
  model->insertRows(row,1);
  model->setData(model->index(row, 1), entry.getWord());
  if(model->submitAll()) {
        model->database().commit();
    }
//  model->setQuery(query);
//  model->insertRecord(-1, query.record());
//  model->query().record();
  //model->insertRowIntoTable(model->query().record());
//  QSqlRecord rec;

  this->clearForms();
  return true;
}

void TabContents::clearForms() {
  ui->wordForm->clear();
  ui->transcritptionForm->clear();
  ui->translationForm->clear();
}
