#include "paradigmwindow.h"
#include "ui_paradigmwindow.h"

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

#include "customquerydiagnostics.h"
#include "legendwindow.h"
#include "etimologywindow.h"
#include "phonologywindow.h"
#include "taleswindow.h"
#include "paradigmwindow.h"
#include "playwindow.h"
#include "utils.h"



ParadigmWindow::ParadigmWindow(DictGlobalAttributes _dictAttrs, QVariant _wordIdSelected, QVariant _wordSelected, QVariant _transcriptionSelected, QVariant _translationSelected, QWidget *parent) :
  dictAttrs(_dictAttrs),
  wordIdSelected(_wordIdSelected),
  transcriptionSelectedID(_wordIdSelected),
  wordSelected(_wordSelected),
  transcriptionSelected(_transcriptionSelected),
  translationSelected(_translationSelected),
  QDialog(parent),
  db(QSqlDatabase::database(dictAttrs.getFilename())),
  ui(new Ui::ParadigmWindow),
  soundChosen(false),
  praatChosen(false),
  praatRightChosen(false)
{
  ui->setupUi(this);

  if (!db.isValid()) {
      qDebug() << "Error during tab creation: Database is " << db.isOpen() << " and it's because " << db.isOpenError();
      return;
  }

  ui->labelWord->setText(wordSelected.toString());
  ui->labelTrascription->setText(transcriptionSelected.toString());
  ui->labelTranslation_3->setText(translationSelected.toString());


  connect(ui->soundButton, SIGNAL(clicked()), this, SLOT(chooseSound()));
  connect(ui->praatButton, SIGNAL(clicked()), this, SLOT(choosePraat()));

  connect(ui->submitButton , SIGNAL(clicked()), this, SLOT(submitWord()));

  connect(ui->playButton_2 , SIGNAL(clicked()), this, SLOT(showPlay()));


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

  ui->soundsList_2->setModel(soundsModel);
  ui->soundsList_2->setColumnHidden(0,true);
  ui->soundsList_2->setColumnHidden(1,true);
  ui->soundsList_2->setColumnHidden(4,true);
  ui->soundsList_2->setColumnHidden(5,true);
  ui->soundsList_2->verticalHeader()->setVisible(false);
//  ui->soundsList->setModelColumn(2);
//  ui->soundsList->setUpdatesEnabled(true);

  initializePraatModel(praatModel);

  ui->praatList_2->setModel(praatModel);
  ui->praatList_2->setColumnHidden(0,true);
  ui->praatList_2->setColumnHidden(1,true);
  ui->praatList_2->setColumnHidden(4,true);
  ui->praatList_2->setColumnHidden(5,true);
  ui->praatList_2->verticalHeader()->setVisible(false);
//  ui->praatList->setModelColumn(2);

  connect(ui->dictionaryTable , SIGNAL(clicked(QModelIndex)), this, SLOT(filterBlobs()));
  connect(ui->dictionaryTable, SIGNAL(activated(QModelIndex)), this, SLOT(filterBlobs()));

  connect(ui->choosePraat_2, SIGNAL(clicked()), this, SLOT(choosePraatRight()));
  connect(ui->praatDescription_3, SIGNAL(textChanged(QString)), this, SLOT(checkPraatRightDescription()));
  connect(ui->submitPraat_2, SIGNAL(clicked()), this, SLOT(submitRightPraat()));

  ui->sendToPraat_2->setDisabled(true);
//  ui->deleteButton->setDisabled(true);
}

bool ParadigmWindow::chooseSound() {
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

bool ParadigmWindow::choosePraatRight() {

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
        ui->choosePraat_2->setText("Сбросить выбор?");
        this->praatRightChosen = true;
      }

  } else {
      this->praatRightFormFilenameMarkup.clear();
      this->praatRightFormFilenameSound.clear();
      ui->choosePraat_2->setText("Выберите файл разметки");
      this->praatRightChosen = false;
  }
  return true;
}

bool ParadigmWindow::checkPraatRightDescription() {
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();

  if (!select->hasSelection()) {
      errorMsg("Вы не выбрали слово, для которого хотите добавить разметку Praat");
      return false;
  }

  if (ui->praatDescription_3->text().isEmpty()) {
      return false;
    } else if (!this->praatRightChosen) {
      return false;
    } else {
      return true;
    }
}

bool ParadigmWindow::submitRightPraat() {
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

  if (ui->praatDescription_3->text().isEmpty()) {
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
  praatDescriptionQuery.bindValue(":description", ui->praatDescription_3->text());
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


bool ParadigmWindow::choosePraat() {
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

bool ParadigmWindow::showPlay() {
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

ParadigmWindow::~ParadigmWindow()
{
  delete dictModel;
  delete soundsModel;
  delete praatModel;
  delete ui;
}

void ParadigmWindow::initializeDictModel(QSqlTableModel *model) {
  model->setTable("dictionary");
  model->setEditStrategy(QSqlTableModel::OnFieldChange);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("Слово"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Регулярная форма"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Транскрипция"));
  model->setHeaderData(4, Qt::Horizontal, QObject::tr("Перевод"));
  model->setFilter(QString("is_a_regular_form = 0 AND regular_form = \"") + transcriptionSelectedID.toString() + QString("\""));
  qDebug() << model->query().lastQuery();
  model->select();
  qDebug() << model->query().lastQuery();
  qDebug() << model->query().result();
}

bool ParadigmWindow::filterBlobs() {
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

void ParadigmWindow::initializeSoundsModel(QSqlTableModel *model) {
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

void ParadigmWindow::initializePraatModel(QSqlTableModel *model) {
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



bool ParadigmWindow::isValidInput(void) {
  // 1 -- text inputs are blank
  if (ui->wordForm->text().isEmpty() && ui->transcritptionForm->text().isEmpty() && ui->translationForm->text().isEmpty()) {
      errorMsg("Вы должны заполнить хотя бы одно из текстовых полей для создания слова");
      return false;
  }
  // 2 -- sound is selected, but no description
  if (!this->soundFilename.isEmpty() && ui->soundDescription_2->text().isEmpty()) {
      errorMsg("Вы выбрали звук, но не написали описание: так нельзя");
      return false;
  }
  // 3 -- sound is not selected, but description is ready
  if (this->soundFilename.isEmpty() && !ui->soundDescription_2->text().isEmpty()) {
      errorMsg("Вы описали аудиозапись, но не выбрали её: так нельзя");
      return false;
  }
  // 4 -- sound file doesn't exist
  if (!this->soundFilename.isEmpty() && !QFile(this->soundFilename).exists()) {
      errorMsg("Кажется, аудиофайл изчез, пока вы заполняли остальные поля. Сбросьте выбор и попробуйте ещё раз");
      return false;
  }
  // 5 -- praat is selected, but no description
  if ( !(this->praatFilenameSound.isEmpty() || this->praatFilenameMarkup.isEmpty()) && ui->praatDescription_4->text().isEmpty()) {
      errorMsg("Вы выбрали файлы для Praat, но не описали их; так нельзя");
      return false;
  }
  // 6 -- praat is not selected, but description is ready
  if ((this->praatFilenameSound.isEmpty() || this->praatFilenameMarkup.isEmpty()) && !ui->praatDescription_4->text().isEmpty()) {
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

DictEntry ParadigmWindow::readFields()
{
  QString word(ui->wordForm->text());
  QString transcription(ui->transcritptionForm->text());
  QString translation(ui->translationForm->text());

  QString soundDescription;
  QString praatDescription;

  if (this->soundChosen) {
      soundDescription = (ui->soundDescription_2->text());
  } else {
      this->soundFilename = "";
  }

  if (this->praatChosen) {
      praatDescription = (ui->praatDescription_4->text());
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

bool ParadigmWindow::submitWord()
{
  /* check that we have at least one filled field */
  if (!this->isValidInput()) {
      return false;
  }

  DictEntry entry(readFields());
  qDebug() << "Now we should get an entry to sqlite";

  QSqlRecord record = dictModel->record();
  record.setValue("word", entry.getWord());
  record.setValue("regular_form" , transcriptionSelectedID);
  record.setValue("transcription" , entry.getTranscription());
  record.setValue("translation", entry.getTranslation());
  record.setValue("is_a_regular_form" , false);
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

void ParadigmWindow::clearForms() {
  ui->wordForm->clear();
  ui->transcritptionForm->clear();
  ui->translationForm->clear();
  ui->soundDescription_2->clear();
  ui->praatDescription_4->clear();
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
  ui->choosePraat_2->setText("Выберите файл разметки");
  ui->praatDescription_3->clear();
}
