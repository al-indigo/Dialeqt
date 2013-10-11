#include "etimologywindow.h"
#include "ui_etimologywindow.h"
#include <QDebug>
#include <QSet>
#include <QSqlQuery>

#include "addconnection.h"
#include "utils.h"

EtimologyWindow::EtimologyWindow(QVariant _wordid, QVariant _word_transcription, QVariant _etimology_tag, QSqlDatabase _current_db, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent) :
  wordid(_wordid),
  wordtranscription(_word_transcription),
  tag(_etimology_tag),
  db(_current_db),
  dictsOpened(_dictsOpened),
  QDialog(parent),
  ui(new Ui::EtimologyWindow)
{
  ui->setupUi(this);
  model = new QSqlQueryModel();
  wordsmodel = new QSqlQueryModel();
  QString dirty = QString("SELECT dict_attributes.id, \
                          dict_attributes.dict_name, \
                          dict_attributes.dict_classification_tags, \
                          dict_attributes.dict_identificator \
                          FROM \
                          dict_attributes \
                          JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                          JOIN dictionary ON (etimology.wordid = dictionary.id AND dictionary.etimology_tag = ") + QString("'") + tag.toString() + QString("') ") + QString("GROUP BY dict_attributes.id;");
/*  model->setQuery("SELECT dict_attributes.id, \
                  dict_attributes.dict_name, \
                  dict_attributes.dict_classification_tags, \
                  dict_attributes.dict_identificator \
                  FROM \
                  dict_attributes \
                  JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                  JOIN dictionary ON (etimology.wordid = dictionary.id) \
                  GROUP BY dict_attributes.id;", db); */
  model->setQuery(dirty, db);
  qDebug() << dirty;
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Название словаря"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("К каким группам относится словарь"));

  ui->treeView->setModel(model);
  ui->treeView->setColumnHidden(0,true);
  ui->treeView->setColumnHidden(3,true);

  ui->treeView->setColumnWidth(1, 350);
  ui->treeView->setColumnWidth(2, 150);

  foreach (const DictGlobalAttributes &item, *this->dictsOpened) {
      ui->dictsDropdown->addItem(item.getDictname(), item.getFilename());
    }

  connect(ui->treeView , SIGNAL(clicked(QModelIndex)), this, SLOT(findWords()));
  connect(ui->treeView, SIGNAL(activated(QModelIndex)), this, SLOT(findWords()));
  connect(ui->addConnectionButton, SIGNAL(clicked()), this, SLOT(openDbAndAddConnection()));
  connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
}

bool EtimologyWindow::openDbAndAddConnection() {
  AddConnection connection_window(wordid, wordtranscription, tag, db, QSqlDatabase::database(ui->dictsDropdown->itemData(ui->dictsDropdown->currentIndex()).toString()), dictsOpened);
  connection_window.exec();
  return true;
}

bool EtimologyWindow::findWords() {
  QModelIndexList model_index = ui->treeView->selectionModel()->selection().indexes();
  int row = model_index.at(0).row();
  QString param = this->model->data(this->model->index(row,3)).toString();

  foreach (const DictGlobalAttributes &item, *this->dictsOpened) {
    if (item.getDbId() == param) {
        qDebug() << "Selected db is opened now";
//        QSqlQuery modelquery(QSqlDatabase::database(item.getFilename()));
//        modelquery.prepare("SELECT dictionary.id, dictionary.transcription FROM dictionary WHERE etimology_tag=:tag;");
//        modelquery.bindValue(":tag", tag);
        QString dirty = QString("SELECT dictionary.id, dictionary.transcription FROM dictionary WHERE etimology_tag=") + QString("'") + tag.toString() + QString("';");
//        wordsmodel->setQuery(modelquery);
//        wordsmodel->setQuery("SELECT dictionary.id, dictionary.transcription FROM dictionary WHERE etimology_tag='1381452591621test1';", QSqlDatabase::database(item.getFilename()));
        wordsmodel->setQuery(dirty, QSqlDatabase::database(item.getFilename()));
        ui->listView->setModel(wordsmodel);
        ui->listView->setModelColumn(1);
        return true;
    }
  }
  qDebug() << "Selected db is closed now";
  errorMsg("Этот словарь сейчас не открыт в программе: чтобы посмотреть взаимосвязи для этого словаря, вам сначала нужно его открыть.");

  return false;
}

EtimologyWindow::~EtimologyWindow()
{
  delete wordsmodel;
  delete model;
  delete ui;
}
