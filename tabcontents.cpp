#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QFileDialog>
#include <QDateTime>
#include <QProcess>

#include "tabcontents.h"
#include "customquerydiagnostics.h"
#include "legendwindow.h"
#include "etimologywindow.h"
#include "phonologywindow.h"
#include "taleswindow.h"
#include "paradigmwindow.h"
#include "playwindow.h"
#include "utils.h"

#include "ui_tabcontents.h"


TabContents::TabContents(DictGlobalAttributes _dictAttrs, QSet<DictGlobalAttributes> * _dictsOpened, QWidget *parent) :
  dictAttrs(_dictAttrs),
  QWidget(parent),
  db(QSqlDatabase::database(dictAttrs.getFilename())),
  ui(new Ui::TabContents),
  soundChosen(false),
  praatChosen(false),
  praatRightChosen(false),
  dictsOpened(_dictsOpened)
{
  ui->setupUi(this);

  if (!db.isValid()) {
      qDebug() << "Error during tab creation: Database is " << db.isOpen() << " and it's because " << db.isOpenError();
      return;
  }

  
  connect(ui->soundButton, SIGNAL(clicked()), this, SLOT(chooseSound()));
  connect(ui->praatButton, SIGNAL(clicked()), this, SLOT(choosePraat()));

  connect(ui->submitButton , SIGNAL(clicked()), this, SLOT(submitWord()));
  connect(ui->legendButton , SIGNAL(clicked()), this, SLOT(showLegend()));
  connect(ui->etimologyButton , SIGNAL(clicked()), this, SLOT(showEtimology()));
  connect(ui->paradigmButton , SIGNAL(clicked()), this, SLOT(showParadigm()));
  connect(ui->playButton , SIGNAL(clicked()), this, SLOT(showPlay()));
  connect(ui->phonologyButton , SIGNAL(clicked()), this, SLOT(showPhonology()));
  connect(ui->talesButton , SIGNAL(clicked()), this, SLOT(showTales()));

  dictModel = new QSqlTableModel(this, db);
  soundsModel = new QSqlTableModel(this, db);
  praatModel = new QSqlTableModel(this, db);

  initializeDictModel(dictModel);

  ui->dictionaryTable->setModel(dictModel);
  ui->dictionaryTable->setColumnHidden(0,true);
  ui->dictionaryTable->setColumnHidden(2,true);
  ui->dictionaryTable->setColumnHidden(5,true);
  ui->dictionaryTable->setColumnHidden(6,true);
  ui->dictionaryTable->setColumnHidden(7,true);
  ui->dictionaryTable->setColumnHidden(8,true);
  ui->dictionaryTable->setSortingEnabled(true);
  ui->dictionaryTable->setColumnWidth(1, 100 );
  ui->dictionaryTable->setColumnWidth(3, 200);
  ui->dictionaryTable->setColumnWidth(4, 350);

  initializeSoundsModel(soundsModel);

  ui->soundsList->setModel(soundsModel);
  ui->soundsList->setColumnHidden(0,true);
  ui->soundsList->setColumnHidden(1,true);
  ui->soundsList->setColumnHidden(4,true);
  ui->soundsList->setColumnHidden(5,true);
  ui->soundsList->verticalHeader()->setVisible(false);
//  ui->soundsList->setModelColumn(2);
//  ui->soundsList->setUpdatesEnabled(true);

  initializePraatModel(praatModel);

  ui->praatList->setModel(praatModel);
  ui->praatList->setColumnHidden(0,true);
  ui->praatList->setColumnHidden(1,true);
  ui->praatList->setColumnHidden(4,true);
  ui->praatList->setColumnHidden(5,true);
  ui->praatList->verticalHeader()->setVisible(false);
//  ui->praatList->setModelColumn(2);

  connect(ui->dictionaryTable , SIGNAL(clicked(QModelIndex)), this, SLOT(filterBlobs()));
  connect(ui->dictionaryTable, SIGNAL(activated(QModelIndex)), this, SLOT(filterBlobs()));

  connect(ui->choosePraat, SIGNAL(clicked()), this, SLOT(choosePraatRight()));
  connect(ui->praatDescription_2, SIGNAL(textChanged(QString)), this, SLOT(checkPraatRightDescription()));
  connect(ui->submitPraat, SIGNAL(clicked()), this, SLOT(submitRightPraat()));
  connect(ui->sendToPraat, SIGNAL(clicked()), this, SLOT(sendToPraat()));
  connect(parent, SIGNAL(currentChanged(int)), this, SLOT(updateModel()));

//  ui->paradigmButton->setDisabled(true);
//  ui->sendToPraat->setDisabled(true);
  ui->phonologyButton->setDisabled(true);
//  ui->legendButton->setDisabled(true);
//  ui->talesButton->setDisabled(true);
  ui->deleteButton->setDisabled(true);
}

bool TabContents::sendToPraat() {
  QItemSelectionModel *select = ui->praatList->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали элементы для отправки в Praat");
      return false;
  }
  QModelIndex index = ui->praatList->currentIndex();
  QVariant praatblobid = index.sibling(index.row(),0).data();
  QVariant praatname = index.sibling(index.row(),2).data();
  qDebug() << praatblobid;

  QSqlQuery query_blobs(db);
  query_blobs.prepare("SELECT mainblob, secblob FROM blobs WHERE blobs.id = :blobid");
  query_blobs.bindValue(":blobid", praatblobid);
  query_blobs.exec();
  qDebug() << getLastExecutedQuery(query_blobs);


  // I do it because sendpraat is dumb: it doesn't understand paths with spaces and non-ASCII
  QString dest_dir = QDir::tempPath();
  qDebug() << "temp is here: " << dest_dir;

  QString basename = QString::number(QDateTime::currentMSecsSinceEpoch());

  while (query_blobs.next()) {
      if (!extractSound(dest_dir, basename + QString(".wav"), query_blobs.value("mainblob").toByteArray())) {
          qDebug() << "Can't decompress sound blobs";
          errorMsg("Не удалось распаковать звук из праатовской пары файлов, так быть не должно. Либо у вас кончается место на диске, либо это ошибка -- напишите разработчику и не редактируйте словарь, пока он не ответит.");
          return false;
      } else {
          if (!extractSound(dest_dir, basename + QString(".TextGrid"), query_blobs.value("secblob").toByteArray())) {
              qDebug() << "Can't decompress praat markup";
              errorMsg("Не удалось распаковать разметку из праатовской пары файлов, так быть не должно. Либо у вас кончается место на диске, либо это ошибка -- напишите разработчику и не редактируйте словарь, пока он не ответит.");
              return false;
          }
      }
   }
  QProcess::startDetached("Praat.exe");
  QStringList args;
  QString arg1 = QString("praat");
  QString arg2 = QString("sound = Read from file... %1/%2.wav").arg(dest_dir, basename);
  QString arg3 = QString("markup = Read from file... %1/%2.TextGrid").arg(dest_dir, basename);
  QString arg4 = QString("selectObject(sound,markup)");
  QString arg5 = QString("View & Edit");
  qDebug() << "args for praatsend: " << arg1 << arg2 << arg3 << arg4 << arg5;
  args.append(arg1);
  args.append(arg2);
  args.append(arg3);
  args.append(arg4);
  args.append(arg5);
  QProcess::execute("sendpraat.exe", args);
  return true;
}


bool TabContents::chooseSound() {
  if (!this->soundChosen) {
      this->soundFilename = QFileDialog::getOpenFileName(this, tr("Выберите звуковой файл для вставки в словарь"), "", tr("Звуковые файлы (*.mp3 *.wav)"));
      qDebug() << "Sound file selected: " << this->soundFilename;
      if (!this->soundFilename.isEmpty()) {
        ui->soundButton->setText("Сбросить звук?");
        this->soundChosen = true;
      }
  } else {
      this->soundFilename.clear();
      ui->soundButton->setText("Выберите звуковой файл");
      this->soundChosen = false;
  }
  return true;
}

bool TabContents::choosePraatRight() {

  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого хотите добавить разметку Praat");
      return false;
  }

  if (!this->praatRightChosen) {
      this->praatRightFormFilenameMarkup =  QDir::toNativeSeparators (QFileDialog::getOpenFileName(this,
                                                                                          tr("Выберите файл разметки Praat для вставки в словарь"),
                                                                                          "",
                                                                                          tr("Файлы разметки Praat (*.TextGrid)")));
      QFileInfo fileInfo(this->praatRightFormFilenameMarkup);
      this->praatRightFormFilenameSound = QDir::toNativeSeparators (fileInfo.canonicalPath().append(fileInfo.completeBaseName().append(".wav").prepend("/")));
      qDebug() << "Praat right markup is selected: " << this->praatRightFormFilenameMarkup;
      qDebug() << "Praat right sound is selected: " << this->praatRightFormFilenameSound;
      if (!this->praatRightFormFilenameMarkup.isEmpty()) {
        ui->choosePraat->setText("Сбросить выбор?");
        this->praatRightChosen = true;
      }

  } else {
      this->praatRightFormFilenameMarkup.clear();
      this->praatRightFormFilenameSound.clear();
      ui->choosePraat->setText("Выберите файл разметки");
      this->praatRightChosen = false;
  }
  return true;
}

bool TabContents::checkPraatRightDescription() {
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого хотите добавить разметку Praat");
      return false;
  }

  if (ui->praatDescription_2->text().isEmpty()) {
      return false;
    } else if (!this->praatRightChosen) {
      return false;
    } else {
      return true;
    }
}

bool TabContents::submitRightPraat() {
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого хотите добавить разметку Praat");
      return false;
  }

  QModelIndex index = ui->dictionaryTable->currentIndex();
  QVariant wordid = index.sibling(index.row(),0).data();
  qDebug() << wordid;

  if (!this->praatRightChosen) {
      errorMsg("Вы не выбрали файл разметки");
      return false;
    }

  if (ui->praatDescription_2->text().isEmpty()) {
      errorMsg("Вы не заполнили описание для разметки, без этого нельзя её добавить");
      return false;
    }

  QByteArray praatMarkup;
  QByteArray praatSound;
  readAndCompress(this->praatRightFormFilenameMarkup, praatMarkup);
  readAndCompress(this->praatRightFormFilenameSound, praatSound);

  QFileInfo fileinfo = QFileInfo(this->praatRightFormFilenameSound);
  qDebug() << praatSound.size();

  QVariant praatblobid;

  // here we need to push sound to blobs table, get id of blob and make a record in blobs_description_table
  QSqlQuery praatBlobQuery(db);
  praatBlobQuery.prepare("INSERT INTO blobs(mainblob,secblob) values (:praatmarkupblob, :praatsoundblob)");
  praatBlobQuery.bindValue(":praatmarkupblob", praatMarkup);
  praatBlobQuery.bindValue(":praatsoundblob", praatSound);

  if (!praatBlobQuery.exec()) {
      qDebug() << "Can't insert praat blob";
      qDebug() << "Query was the following: " << getLastExecutedQuery(praatBlobQuery);
      qDebug() << db.lastError().text();
      this->clearForms();
      return false;
  }
  praatblobid = praatBlobQuery.lastInsertId();
  praatBlobQuery.clear();

  QSqlQuery praatDescriptionQuery(db);
  praatDescriptionQuery.prepare("INSERT INTO dict_blobs_description(type,name,description,wordid,blobid)"
                                 "VALUES (2,:name,:description,:wordid,:blobid)");
  praatDescriptionQuery.bindValue(":name", fileinfo.completeBaseName());
  praatDescriptionQuery.bindValue(":description", ui->praatDescription_2->text());
  praatDescriptionQuery.bindValue(":wordid", wordid);
  praatDescriptionQuery.bindValue(":blobid", praatblobid);

  if (!praatDescriptionQuery.exec()) {
      qDebug() << "Can't insert praat blob description";
      qDebug() << "Query was the following: " << getLastExecutedQuery(praatDescriptionQuery);
      qDebug() << db.lastError().text();
      this->clearForms();
      return false;
  }
  qDebug() << "Successfully submitted praat";

  this->clearForms();
  this->praatModel->select();
  return true;
}


bool TabContents::choosePraat() {
  if (!this->praatChosen) {
      this->praatFilenameMarkup =  QDir::toNativeSeparators (QFileDialog::getOpenFileName(this,
                                                                                          tr("Выберите файл разметки Praat для вставки в словарь"),
                                                                                          "",
                                                                                          tr("Файлы разметки Praat (*.TextGrid)")));
      QFileInfo fileInfo(this->praatFilenameMarkup);
      if (!this->praatFilenameMarkup.isEmpty()) {
        this->praatFilenameSound = QDir::toNativeSeparators (fileInfo.canonicalPath().append(fileInfo.completeBaseName().append(".wav").prepend("/")));
        qDebug() << "Praat markup is selected: " << this->praatFilenameMarkup;
        qDebug() << "Praat sound is selected: " << this->praatFilenameSound;

        ui->praatButton->setText("Сбросить выбор?");
        this->praatChosen = true;
      }
  } else {
      this->praatFilenameMarkup.clear();
      this->praatFilenameSound.clear();
      ui->praatButton->setText("Выберите файл разметки");
      this->praatChosen = false;
  }
  return true;
}

bool TabContents::showLegend() {
  LegendWindow legend(&dictAttrs, this);
  if ( legend.exec() == QDialog::Accepted ) {
       qDebug() << "accepted";
       legend.setNewDictData();
       dictAttrs.debugPrint();
       QSqlQuery dictAttributesUpdate(db);
       dictAttributesUpdate.prepare("UPDATE dict_attributes "
                                    "SET dict_coauthors = :coauthors, dict_classification_tags = :tags, dict_description = :description "
                                    "WHERE id=1;");
       dictAttributesUpdate.bindValue(":coauthors", QVariant(dictAttrs.getCoauthorsQString()));
       dictAttributesUpdate.bindValue(":tags", dictAttrs.getTagsQString());
       dictAttributesUpdate.bindValue(":description", dictAttrs.getDescription());

       if (!dictAttributesUpdate.exec()) {
          //TODO: Cleanup
           qDebug() << "Can't insert sound blob description";
           qDebug() << "Query was the following: " << getLastExecutedQuery(dictAttributesUpdate);
           qDebug() << db.lastError().text();
           return false;
       }
       qDebug() << "Successfully updated dict info";
    }
  return true;
}

bool TabContents::showEtimology() {
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого необходимо отобразить этимологию");
      return false;
  }
  QModelIndex index = ui->dictionaryTable->currentIndex();
  QVariant wordid = index.sibling(index.row(),0).data();
  qDebug() << wordid;
  QVariant word = index.sibling(index.row(),1).data();
  qDebug() << word;
  QVariant transcription = index.sibling(index.row(),2).data();
  qDebug() << transcription.toString();
  QVariant tag = index.sibling(index.row(),6).data();
  qDebug() << tag;
  if (tag == "") {
      QString cur_time = QString::number(QDateTime::currentMSecsSinceEpoch());
//      QString ct;
//      ct.setNum(cur_time,10);
      tag = cur_time + transcription.toString();
      qDebug() << tag;
    }

  EtimologyWindow etimology(wordid, transcription, tag, db, dictsOpened, this);
  etimology.exec();
  this->dictModel->select();
  ui->dictionaryTable->setCurrentIndex(index);
  return true;
}

bool TabContents::updateModel() {
  QModelIndex index = ui->dictionaryTable->currentIndex();
  this->dictModel->select();
  ui->dictionaryTable->setCurrentIndex(index);
  qDebug() << "Model has been updated";
  return true;
}

bool TabContents::showParadigm() {
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого необходимо отобразить парадигму");
      return false;
  }
  QModelIndex index = ui->dictionaryTable->currentIndex();
  QVariant wordid = index.sibling(index.row(),0).data();
  qDebug() << wordid;
  QVariant word = index.sibling(index.row(),1).data();
  qDebug() << word;
  QVariant transcription = index.sibling(index.row(),2).data();
  qDebug() << transcription;
  QVariant translation = index.sibling(index.row(),3).data();
  qDebug() << translation;

  ParadigmWindow paradigm(dictAttrs, wordid, word, transcription, translation, this);
  paradigm.exec();

  return true;
}

bool TabContents::showPlay() {
  QStringList playlist;
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого хотите послушать/добавить аудиофайлы");
      return false;
  }
  QModelIndex index = ui->dictionaryTable->currentIndex();
  QVariant wordid = index.sibling(index.row(),0).data();
  qDebug() << wordid;

  QSqlQuery query_descriptions(db);
  query_descriptions.prepare("SELECT blobid, name FROM dict_blobs_description WHERE dict_blobs_description.wordid = :wordid and dict_blobs_description.type = 1");
  query_descriptions.bindValue(":wordid", wordid);
  query_descriptions.exec();
  QString dest_dir = createDirectory(dictAttrs.getDictname(), "audio");

  while (query_descriptions.next()) {
      QSqlQuery query_blobs(db);
      query_blobs.prepare("SELECT mainblob FROM blobs WHERE blobs.id = :blobid");
      query_blobs.bindValue(":blobid",query_descriptions.value("blobid"));
      qDebug() << getLastExecutedQuery(query_blobs);
      query_blobs.exec();
      while (query_blobs.next()) {
          if (!extractSound(dest_dir, query_descriptions.value("name").toString(), query_blobs.value("mainblob").toByteArray())) {
              qDebug() << "Can't decompress sound blobs";
          } else {
              playlist.append(dest_dir + "/" + query_descriptions.value("name").toString());
          }
        }
    }

  PlayWindow play(playlist, db, dest_dir, wordid, this);
  play.exec();
  return true;
}

bool TabContents::showPhonology() {
  PhonologyWindow phonology(this);
  phonology.exec();
  return true;
}

bool TabContents::showTales() {
  TalesWindow tales(db, this);

  tales.exec();
  return true;
}


TabContents::~TabContents()
{
  delete dictModel;
  delete soundsModel;
  delete praatModel;
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
  model->setFilter(QString("is_a_regular_form = 1"));

  qDebug() << model->query().lastQuery();
  model->select();
  qDebug() << model->query().lastQuery();
  qDebug() << model->query().result();
}

bool TabContents::filterBlobs() {
  qDebug() << "filtering blobs";
  QString filterSoundsQuery = "type = 1 and wordid = ";
  QString filterPraatQuery = "dict_blobs_description.type = 2 and dict_blobs_description.wordid = ";

  QModelIndexList model_index = ui->dictionaryTable->selectionModel()->selection().indexes();
  int row = model_index.at(0).row();
  QString param = this->dictModel->data(this->dictModel->index(row,0)).toString();
  filterSoundsQuery.append(param);
  filterPraatQuery.append(param);
  const QString filtersounds(filterSoundsQuery);
  const QString filterpraat(filterPraatQuery);
  soundsModel->setFilter(filtersounds);
  praatModel->setFilter(filterpraat);

  soundsModel->select();
  praatModel->select();

  qDebug() << soundsModel->query().lastQuery();
  qDebug() << praatModel->query().lastQuery();
  qDebug() << soundsModel->query().record().count();
  qDebug() << soundsModel->rowCount();
  qDebug() << soundsModel->query().result();
  qDebug() << praatModel->query().result();
  qDebug() << praatModel->query().record().count();

  return true;
}

void TabContents::initializeSoundsModel(QSqlTableModel *model) {
  model->setTable("dict_blobs_description");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Тип"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Имя"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Описание"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Ссылка на слово"));
  model->setHeaderData(5, Qt::Horizontal, QObject::tr("Ссылка на блоб"));

  model->setFilter("type = 1");
  model->select();
  qDebug() << "initializing soundsmodel";
  qDebug() << model->query().lastQuery();
  qDebug() << model->query().result();
  qDebug() << model->query().record().count();
  qDebug() << model->rowCount();
}

void TabContents::initializePraatModel(QSqlTableModel *model) {
  model->setTable("dict_blobs_description");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Тип"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Имя"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Описание"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Ссылка на слово"));
  model->setHeaderData(5, Qt::Horizontal, QObject::tr("Ссылка на блоб"));

  model->setFilter("type = 2");

  model->select();

}



bool TabContents::isValidInput(void) {
  // 1 -- text inputs are blank
  if (ui->wordForm->text().isEmpty() && ui->transcritptionForm->text().isEmpty() && ui->translationForm->text().isEmpty()) {
      errorMsg("Вы должны заполнить хотя бы одно из текстовых полей для создания слова");
      return false;
  }
  // 2 -- sound is selected, but no description
  if (!this->soundFilename.isEmpty() && ui->soundDescription->text().isEmpty()) {
      errorMsg("Вы выбрали звук, но не написали описание: так нельзя");
      return false;
  }
  // 3 -- sound is not selected, but description is ready
  if (this->soundFilename.isEmpty() && !ui->soundDescription->text().isEmpty()) {
      errorMsg("Вы описали аудиозапись, но не выбрали её: так нельзя");
      return false;
  }
  // 4 -- sound file doesn't exist
  if (!this->soundFilename.isEmpty() && !QFile(this->soundFilename).exists()) {
      errorMsg("Кажется, аудиофайл изчез, пока вы заполняли остальные поля. Сбросьте выбор и попробуйте ещё раз");
      return false;
  }
  // 5 -- praat is selected, but no description
  if ( !(this->praatFilenameSound.isEmpty() || this->praatFilenameMarkup.isEmpty()) && ui->praatDescription->text().isEmpty()) {
      errorMsg("Вы выбрали файлы для Praat, но не описали их; так нельзя");
      return false;
  }
  // 6 -- praat is not selected, but description is ready
  if ((this->praatFilenameSound.isEmpty() || this->praatFilenameMarkup.isEmpty()) && !ui->praatDescription->text().isEmpty()) {
      errorMsg("Вы описали файлы для Praat, но не выбрали их, это нужно сделать");
      return false;
  }
  // 7 -- praat markup doesn't exist
  if (!(this->praatFilenameMarkup.isEmpty()) && !QFile(this->praatFilenameMarkup).exists()) {
      qDebug() << !(this->praatFilenameMarkup.isEmpty());
      qDebug() << QFile(this->praatFilenameMarkup).exists();
      errorMsg("Похоже, к файлу разметки Praat нет файла звука. Проверьте, что они называются одинаково (за исключением расширений .wav и .praat) и что оба файла лежат в одной и той же папке");
      return false;
  }
  // 8 -- praat sound doesn't exist
  if (!(this->praatFilenameSound.isEmpty()) && !QFile(this->praatFilenameSound).exists()) {
      errorMsg("Похоже, вы выбрали файл звука, но парного файла разметки к нему не существует. Проверьте, что они называются одинаково (за исключением расширений .wav и .praat) и что оба файла лежат в одной и той же папке");
      return false;
  }

  // 0 -- everything is ok
  return true;
}

DictEntry TabContents::readFields()
{
  QString word(ui->wordForm->text());
  QString transcription(ui->transcritptionForm->text());
  QString translation(ui->translationForm->text());

  QString soundDescription;
  QString praatDescription;

  if (this->soundChosen) {
      soundDescription = (ui->soundDescription->text());
  } else {
      this->soundFilename = "";
  }

  if (this->praatChosen) {
      praatDescription = (ui->praatDescription->text());
  } else {
      this->praatFilenameMarkup = "";
      this->praatFilenameSound = "";
      praatDescription = "";
  }

  return DictEntry(this,
               word,
               transcription,
               translation,
               this->soundFilename,
               soundDescription,
               this->praatFilenameMarkup,
               this->praatFilenameSound,
               praatDescription);

}

bool TabContents::submitWord()
{
  /* check that we have at least one filled field */
  if (!this->isValidInput()) {
      return false;
  }

  DictEntry entry(readFields());
  qDebug() << "Now we should get an entry to sqlite";


  QSqlRecord record = dictModel->record();
  record.setValue("word", entry.getWord());
  record.setValue("regular_form" , entry.getTranscription());
  record.setValue("transcription" , entry.getTranscription());
  record.setValue("translation", entry.getTranslation());
  record.setValue("is_a_regular_form" , true);
  dictModel->insertRecord(-1, record);

  dictModel->submitAll();
  qDebug() << "after set data: " << dictModel->query().lastQuery();

  record.clear();

  QVariant wordid = dictModel->query().lastInsertId();

  if (this->soundChosen || this->praatChosen) {
    QVariant soundblobid;

    // here we need to push sound to blobs table, get id of blob and make a record in blobs_description_table
    if (this->soundChosen) {
        QSqlQuery soundBlobQuery(db);
        soundBlobQuery.prepare("INSERT INTO blobs(mainblob) values (:soundblob)");
        soundBlobQuery.bindValue(":soundblob", *(entry.getSoundPointer()));
        if (!soundBlobQuery.exec()) {
           //TODO: Cleanup
            qDebug() << "Can't insert sound blob";
            qDebug() << "Query was the following: " << getLastExecutedQuery(soundBlobQuery);
            qDebug() << db.lastError().text();
            this->clearForms();
            return false;
        }
        soundblobid = soundBlobQuery.lastInsertId();
        soundBlobQuery.clear();

        QSqlQuery soundDescriptionQuery(db);
        soundDescriptionQuery.prepare("INSERT INTO dict_blobs_description(type,name,description,wordid,blobid)"
                                       "VALUES (1,:name,:description,:wordid,:blobid)");
        soundDescriptionQuery.bindValue(":name", entry.getSoundFilename());
        soundDescriptionQuery.bindValue(":description", entry.getSoundDescription());
        soundDescriptionQuery.bindValue(":wordid", wordid);
        soundDescriptionQuery.bindValue(":blobid", soundblobid);

        if (!soundDescriptionQuery.exec()) {
           //TODO: Cleanup
            qDebug() << "Can't insert sound blob description";
            qDebug() << "Query was the following: " << getLastExecutedQuery(soundDescriptionQuery);
            qDebug() << db.lastError().text();
            this->clearForms();
            return false;
        }
        qDebug() << "Successfully submitted sound";
    }
  }

  if (this->praatChosen) {
      QVariant praatblobid;

      QSqlQuery praatBlobQuery(db);
      praatBlobQuery.prepare("INSERT INTO blobs(mainblob,secblob) values (:praatmarkupblob,:praatsoundblob)");
      praatBlobQuery.bindValue(":praatmarkupblob", *(entry.getPraatMarkupPointer()));
      praatBlobQuery.bindValue(":praatsoundblob", *(entry.getPraatSoundPointer()));
      if (!praatBlobQuery.exec()) {
         //TODO: Cleanup
          qDebug() << "Can't insert praat blob";
          qDebug() << "Query was the following: " << getLastExecutedQuery(praatBlobQuery);
          qDebug() << db.lastError().text();
          this->clearForms();
          return false;
      }
      praatblobid = praatBlobQuery.lastInsertId();
      praatBlobQuery.clear();

      QSqlQuery praatDescriptionQuery(db);
      praatDescriptionQuery.prepare("INSERT INTO dict_blobs_description(type,name,description,wordid,blobid)"
                                     "VALUES (2,:name,:description,:wordid,:blobid)");
      praatDescriptionQuery.bindValue(":name", entry.getBasePraatMarkupFilename());
      praatDescriptionQuery.bindValue(":description", entry.getPraatDescription());
      praatDescriptionQuery.bindValue(":wordid", wordid);
      praatDescriptionQuery.bindValue(":blobid", praatblobid);

      if (!praatDescriptionQuery.exec()) {
         //TODO: Cleanup
          qDebug() << "Can't insert sound blob description";
          qDebug() << "Query was the following: " << getLastExecutedQuery(praatDescriptionQuery);
          qDebug() << db.lastError().text();
          this->clearForms();
          return false;
      }
      qDebug() << "Successfully submitted praat blob";
  }

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
  ui->soundDescription->clear();
  ui->praatDescription->clear();
  this->praatChosen = false;
  ui->praatButton->setText("Выберите файл разметки");
  this->soundChosen = false;
  ui->soundButton->setText("Выберите звуковой файл");
  this->soundFilename.clear();
  this->praatFilenameMarkup.clear();
  this->praatFilenameSound.clear();
  this->praatRightFormFilenameMarkup.clear();
  this->praatRightFormFilenameSound.clear();
  this->praatRightFormDescription.clear();
  this->praatRightChosen = false;
  ui->choosePraat->setText("Выберите файл разметки");
  ui->praatDescription_2->clear();
}
