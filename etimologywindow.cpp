#include "etimologywindow.h"
#include "ui_etimologywindow.h"
#include <QDebug>
#include <QSet>
#include <QSqlQuery>

#include "addconnection.h"
#include "utils.h"
#include "customquerydiagnostics.h"
#include "dicttabscontainer.h"

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

  setupModel();
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

  this->checkConnectedDatabases();
  connect(ui->unlink, SIGNAL(clicked()), this, SLOT(unlink()));
  connect(ui->goToWord, SIGNAL(clicked()), this, SLOT(goToSelected()));

  connect(this, SIGNAL(goToWordInDict(QString,QVariant)), this->parentWidget()->parentWidget()->parentWidget()->parentWidget()->findChild<DictTabsContainer *>(), SLOT(goToDictAndWord(QString, QVariant)));

}

bool EtimologyWindow::unlink() {
  if (unlinkEtymology(this->wordid, this->tag, this->db, this->dictsOpened)) {
      setupModel();
      return true;
    } else {
      return false;
    }
}

void EtimologyWindow::setupModel() {
  QSqlQuery connected_dicts(db);
  connected_dicts.prepare("SELECT dict_attributes.id, \
                          dict_attributes.dict_name, \
                          dict_attributes.dict_classification_tags, \
                          dict_attributes.dict_identificator \
                          FROM \
                          dict_attributes \
                          JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                          JOIN dictionary ON (etimology.wordid = dictionary.id AND dictionary.etimology_tag = :tag) \
                          GROUP BY dict_attributes.id;");
  connected_dicts.bindValue(":tag", tag);
  connected_dicts.exec();
  model->setQuery(connected_dicts);

  qDebug() << getLastExecutedQuery(model->query());
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Название словаря"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("К каким группам относится словарь"));

  ui->treeView->setModel(model);

}

// here we understand if we can connect our word with anything -- we can't if not all the databases are opened now
bool EtimologyWindow::checkConnectedDatabases() {
  this->connectionNamesForDicts.clear();

  QSqlQuery getConnectedDbList(this->db);
  getConnectedDbList.prepare("SELECT \
                             dict_attributes.dict_identificator \
                             FROM \
                             dict_attributes \
                             JOIN etimology ON (dict_attributes.id = etimology.dictid) \
                             JOIN dictionary ON (etimology.wordid = dictionary.id AND dictionary.etimology_tag = :tag) \
                             GROUP BY dict_attributes.id;");
  getConnectedDbList.bindValue(":tag", this->tag);
  if (!getConnectedDbList.exec()) {
      errorMsg("Не получается получить список связанных со словом словарей. Вы можете просмотреть связи, но не можете добавлять новые.");
      ui->addConnectionButton->setDisabled(true);
      ui->unlink->setDisabled(true);
      return false;
    }
  QSet<QString> openedDbIdentifiers;
  foreach (const DictGlobalAttributes &item, *this->dictsOpened) {
      openedDbIdentifiers.insert(item.getDbId());
      qDebug() << "opened db list " << item.getDbId();
    }

  QSet<QString> connectedDbIdentifiers;
  while (getConnectedDbList.next()) {
      connectedDbIdentifiers.insert(getConnectedDbList.value("dict_identificator").toString());
      qDebug() << "connected db list " << getConnectedDbList.value("dict_identificator").toString();
    }

  if (!openedDbIdentifiers.contains(connectedDbIdentifiers)) {
      connectedDbIdentifiers.subtract(openedDbIdentifiers);
      QString missingList;
      foreach (const QString &item, connectedDbIdentifiers) {
          missingList += item + " ";
        }

      errorMsg("Не все словари, с которыми связано слово, открыты. Вы можете просмотреть связи слова со словами из открытых словарей, но не можете добавить новые. Список недостающих словарей (это их идентификаторы):" + missingList);
      ui->addConnectionButton->setDisabled(true);
      ui->unlink->setDisabled(true);

      return false;
    }
  connectedDbIdentifiers.intersect(openedDbIdentifiers);
  foreach (const DictGlobalAttributes &dict, *this->dictsOpened) {
      if (connectedDbIdentifiers.contains(dict.getDbId())) {
          this->connectionNamesForDicts.insert(dict.getFilename());
        }
    }

  return true;
}

bool EtimologyWindow::prepareForConnection () {
  wordsToConnect.clear();
  foreach (const QString &dbitem, this->connectionNamesForDicts) {
      QSqlQuery findWordsByTag(QSqlDatabase::database(dbitem));
      findWordsByTag.prepare("SELECT dictionary.id FROM dictionary WHERE etimology_tag=:tag;");
      findWordsByTag.bindValue(":tag", this->tag);
      if (!findWordsByTag.exec()) {
          errorMsg("Похоже, консистентность словарей нарушена. Пришлите все связанные с этим словом словари, слово, которое вы пытаетесь связать и слово с которым вы пытаетесь его связать разработчику и не трогайте ничего, пока он не ответит. Или у вас ломается жёсткий диск, но это куда менее вероятно.");
          return false;
        }
      while (findWordsByTag.next()) {
          wordsToConnect.append(QPair<QString, QVariant>(dbitem,findWordsByTag.value("id")));
        }
    }
  wordsToConnect.append(QPair<QString, QVariant>(db.connectionName(),wordid));
  return true;
}

bool EtimologyWindow::openDbAndAddConnection() {
  this->checkConnectedDatabases();

  this->prepareForConnection();

  AddConnection connection_window(wordsToConnect, tag, QSqlDatabase::database(ui->dictsDropdown->itemData(ui->dictsDropdown->currentIndex()).toString()), dictsOpened);

  connection_window.exec();

  this->checkConnectedDatabases(); //it's needed for consistency check

//  model->setQuery(this->wordsModelQuery, db);
  this->setupModel();

  return true;
}

bool EtimologyWindow::findWords() {
  QModelIndexList model_index = ui->treeView->selectionModel()->selection().indexes();
  int row = model_index.at(0).row();
  QString param = this->model->data(this->model->index(row,3)).toString();

  foreach (const DictGlobalAttributes &item, *this->dictsOpened) {
    if (item.getDbId() == param) {
        qDebug() << "Selected db is opened now";
        QSqlQuery modelquery(QSqlDatabase::database(item.getFilename()));
        modelquery.prepare("SELECT dictionary.id, dictionary.transcription FROM dictionary WHERE etimology_tag=:tag;");
        modelquery.bindValue(":tag", tag);
        modelquery.exec();
//        QString dirty = QString("SELECT dictionary.id, dictionary.transcription FROM dictionary WHERE etimology_tag=") + QString("'") + tag.toString() + QString("';");
        wordsmodel->setQuery(modelquery);
//        wordsmodel->setQuery("SELECT dictionary.id, dictionary.transcription FROM dictionary WHERE etimology_tag='1381452591621test1';", QSqlDatabase::database(item.getFilename()));
//        wordsmodel->setQuery(dirty, QSqlDatabase::database(item.getFilename()));
        ui->tableView->setModel(wordsmodel);
        wordsmodel->setHeaderData(1, Qt::Horizontal, QObject::tr("Транскрипция"));
        ui->tableView->setColumnHidden(0, true);
        return true;
    }
  }
  qDebug() << "Selected db is closed now";
  errorMsg("Этот словарь сейчас не открыт в программе: чтобы посмотреть взаимосвязи для этого словаря, вам сначала нужно его открыть.");

  return false;
}

bool EtimologyWindow::goToSelected() {
  QItemSelectionModel *select = ui->tableView->selectionModel();
  if (!select->hasSelection()) {
      errorMsg("Вы не выделили слово, к которому хотите перейти.");
      return false;
  }

  QModelIndexList selected = select->selectedIndexes();
  foreach (QModelIndex index, selected) {
    QVariant id = index.sibling(index.row(),0).data();
    QModelIndexList dictionaries = ui->treeView->selectionModel()->selectedIndexes();
    QString dict = dictionaries[0].data().toString();
    emit goToWordInDict(dict, id);
    }

  return true;
}

EtimologyWindow::~EtimologyWindow()
{
  delete wordsmodel;
  delete model;
  delete ui;
}
