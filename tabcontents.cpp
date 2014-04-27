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
#include <QApplication>
#include <QClipboard>
#include <QAction>

#include "dicttabscontainer.h"
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
  dictsOpened(_dictsOpened),
  currentResult(NULL)
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
  connect(ui->saveChangedPraatButton, SIGNAL(clicked()), this, SLOT(saveChangesInPraat()));

  dictModel = new QSqlTableModel(this, db);
  soundsModel = new QSqlTableModel(this, db);
  praatModel = new QSqlTableModel(this, db);

  initializeDictModel(dictModel);

  ui->dictionaryTable->setModel(dictModel);
  ui->dictionaryTable->setColumnHidden(0,false);
  ui->dictionaryTable->setColumnWidth(0,1);
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
  connect(this, SIGNAL(search_activated()), this, SLOT(filterBlobs()));

  connect(ui->choosePraat, SIGNAL(clicked()), this, SLOT(choosePraatRight()));
  connect(ui->praatDescription_2, SIGNAL(textChanged(QString)), this, SLOT(checkPraatRightDescription()));
  connect(ui->submitPraat, SIGNAL(clicked()), this, SLOT(submitRightPraat()));
  connect(ui->sendToPraat, SIGNAL(clicked()), this, SLOT(sendToPraat()));
  connect(parent, SIGNAL(currentChanged(int)), this, SLOT(updateModel()));

  ui->showFilesButton->setHidden(true);
  connect(ui->filesBox, SIGNAL(toggled(bool)), this, SLOT(showFiles(bool)));
  connect(ui->showFilesButton, SIGNAL(clicked()), this, SLOT(showFiles()));

  ui->showFormsButton->setHidden(true);
  connect(ui->groupBox, SIGNAL(toggled(bool)), this, SLOT(showForms(bool)));
  connect(ui->showFormsButton, SIGNAL(clicked()), this, SLOT(showForms()));

  ui->showActionsOnDict->setHidden(true);
  connect(ui->groupBoxActionsOnDict, SIGNAL(toggled(bool)), this, SLOT(showActionsOnDict(bool)));
  connect(ui->showActionsOnDict, SIGNAL(clicked()), this, SLOT(showActionsOnDict()));

  ui->showActionsOnWord->setHidden(true);
  connect(ui->groupBoxActionsOnWord, SIGNAL(toggled(bool)), this, SLOT(showActionsOnWord(bool)));
  connect(ui->showActionsOnWord, SIGNAL(clicked()), this, SLOT(showActionsOnWord()));

//  connect(ui->dictionaryTable, SIGNAL(clicked(QModelIndex)), this, SLOT(copySelectedToClipboard()));
//  connect(ui->dictionaryTable, SIGNAL(activated(QModelIndex)), this, SLOT(copySelectedToClipboard()));

  copyAction = new QAction(this);
  copyAction->setShortcut(tr("Ctrl+C"));
  connect(copyAction, SIGNAL(triggered()), this, SLOT(copySelectedToClipboard()));
  ui->dictionaryTable->addAction(copyAction);

  ui->phonologyButton->setDisabled(true);

  connect(ui->simpleSearchButton, SIGNAL(clicked()), this, SLOT(simpleSearch()));
  connect(ui->simpleSearchNext, SIGNAL(clicked()), this, SLOT(simpleSearchNext()));
  connect(ui->simpleSearchPrevious, SIGNAL(clicked()), this, SLOT(simpleSearchPrevious()));
  connect(ui->simpleReplaceButton, SIGNAL(clicked()), this, SLOT(simpleReplaceCurrent()));
  connect(ui->deletePraatButton, SIGNAL(clicked()), this, SLOT(deletePraat()));
  connect(ui->deleteSoundButton, SIGNAL(clicked()), this, SLOT(deleteSound()));
  connect(ui->deleteWordButton, SIGNAL(clicked()), this, SLOT(deleteEntry()));

  connect(this->parentWidget()->parentWidget()->findChild<DictTabsContainer *>(), SIGNAL(goToWord(QVariant)), this, SLOT(goToWord(QVariant)));


}

bool TabContents::goToWord(QVariant id) {
  QModelIndexList idx = this->dictModel->match(this->dictModel->index(0, 0), Qt::DisplayRole, id.toString(), -1, Qt::MatchExactly);
  if (idx.empty()) {
      qDebug() << "Nothing found";
      return false;
    }
  qDebug() << idx[0];
  ui->dictionaryTable->setCurrentIndex(idx[0]);
  ui->dictionaryTable->selectionModel()->setCurrentIndex(idx[0], QItemSelectionModel::Select | QItemSelectionModel::Rows);
  ui->dictionaryTable->setFocus();
  return true;
}

bool TabContents::deleteEntry() {
  QItemSelectionModel *select = ui->dictionaryTable->selectionModel();
  if (!select->hasSelection()) {
      errorMsg("Вы не выделили разметки, которые хотели бы удалить.");
      return false;
  }
  if (!confirmationMsg("Вы уверены, что вы хотите удалить выбранные слова?", this)) {
      return false;
    }
  QModelIndexList selected = select->selectedIndexes();
  foreach (QModelIndex index, selected) {
      QVariant wordid = index.sibling(index.row(),0).data();
      QVariant tag = index.sibling(index.row(), 6).data();
      if (!deleteWord(wordid, tag, db, dictsOpened)) {
        this->updateModel();
        return false;
      }
    }
  this->updateModel();
  praatModel->select();
  soundsModel->select();
  return true;
}

bool TabContents::deletePraat() {
  QItemSelectionModel *select = ui->praatList->selectionModel();
  if (!select->hasSelection()) {
      errorMsg("Вы не выделили разметки, которые хотели бы удалить.");
      return false;
  }
  if (!confirmationMsg("Вы уверены, что вы хотите удалить выбранные разметки Praat?", this)) {
      return false;
    }

  QModelIndexList selected = select->selectedIndexes();
  foreach (QModelIndex index, selected) {
    QVariant praatblobid = index.sibling(index.row(),0).data();
    if (!deleteAttachment(praatblobid, "dict_blobs_description", db)) {
      praatModel->select();
      errorMsg("Не удалось удалить разметку " + index.sibling(index.row(), 1).data().toString() + ", попробуйте ещё раз.");
      return false;
    }
    for (QList<QPair <QVariant, QString> >::iterator i = praatListToSave.begin(); i != praatListToSave.end(); ++i) {
        if (i->first == praatblobid) {
            i = praatListToSave.erase(i);
            i--;
          }
      }
  }

  praatModel->select();
  return true;
}

bool TabContents::deleteSound() {
  QItemSelectionModel *select = ui->soundsList->selectionModel();
  if (!select->hasSelection()) {
      errorMsg("Вы не выделили звуки, которые хотели бы удалить.");
      return false;
  }

  if (!confirmationMsg("Вы уверены, что вы хотите удалить выбранные звуки?", this)) {
      return false;
    }

  QModelIndexList selected = select->selectedIndexes();
  foreach (QModelIndex index, selected) {
    QVariant praatblobid = index.sibling(index.row(),0).data();
    if (!deleteAttachment(praatblobid, "dict_blobs_description", db)) {
      soundsModel->select();
      errorMsg("Не удалось удалить звук " + index.sibling(index.row(), 1).data().toString() + ", попробуйте ещё раз.");
      return false;
    }
  }

  soundsModel->select();
  return true;
}



bool TabContents::copySelectedToClipboard() {
  QAbstractItemModel * model = ui->dictionaryTable->model();
  QItemSelectionModel * selection = ui->dictionaryTable->selectionModel();
  QModelIndexList indexes = selection->selectedIndexes();

  QString selected_text;
  // You need a pair of indexes to find the row changes
  QModelIndex previous = indexes.first();
  indexes.removeFirst();
  QString last_element;

  foreach(const QModelIndex &current, indexes) {
      if (current.column() == 2) continue;
      QString text = model->data(previous).toString();
      selected_text.append(text);
      if (current.row() != previous.row()) {
          selected_text.append('\n');
      } else {
        selected_text.append('\t');
      }
      previous = current;
      last_element = model->data(current).toString();
  }
  selected_text.append(last_element + "\n");

//  QApplication.clipboard()->setText(selected_text);
  QApplication::clipboard()->setText(selected_text);
  return true;
}

bool TabContents::simpleSearch() {
  lastSearchResults.clear();
  if (currentResult != NULL) {
      delete currentResult;
    }

  lastSearchResults.append(this->dictModel->match(this->dictModel->index(0, 1), Qt::DisplayRole, ui->simpleSearchInput->displayText(), -1, Qt::MatchContains));
  lastSearchResults.append(this->dictModel->match(this->dictModel->index(0, 3), Qt::DisplayRole, ui->simpleSearchInput->displayText(), -1, Qt::MatchContains));
  lastSearchResults.append(this->dictModel->match(this->dictModel->index(0, 4), Qt::DisplayRole, ui->simpleSearchInput->displayText(), -1, Qt::MatchContains));

  foreach (const QModelIndex & item, lastSearchResults) {
      qDebug() << item.data() << "\n";
    }

  currentResult = new QListIterator<QModelIndex>(lastSearchResults);

  if (lastSearchResults.empty()) {
      errorMsg("Ничего не найдено!");
      return false;
    }

  ui->dictionaryTable->clearSelection();
  ui->dictionaryTable->setCurrentIndex(lastSearchResults.first());
  ui->dictionaryTable->setFocus();
  emit search_activated();

  currentResult->toFront();
  if (currentResult->hasNext()) {
      currentResult->next();
    }

  return true;

}

bool TabContents::simpleSearchNext() {
  if (lastSearchResults.empty()) {
      return false;
    }
  if (!currentResult->hasNext()) {
      currentResult->toFront();
    }

  if (currentResult->hasNext()) {
      ui->dictionaryTable->clearSelection();
      ui->dictionaryTable->setCurrentIndex(currentResult->peekNext());
      currentResult->next();
      qDebug() << ui->dictionaryTable->currentIndex().row();
      ui->dictionaryTable->setFocus();
      emit search_activated();
      return true;
    } else {
      return false;
    }

}

bool TabContents::simpleReplaceCurrent() {
  if (ui->simpleReplaceInput->text().isEmpty()) {
      errorMsg("Нельзя заменить что-то на ничего: у вас должно быть что-то в поле ввода для замены.");
      return false;
    }
  if (ui->simpleSearchInput->text().isEmpty()) {
      errorMsg("Нельзя заменить ничего на что-то: ничего хорошего из этого не выйдет.");
      return false;
    }
  if (! (lastSearchResults.contains(ui->dictionaryTable->currentIndex())) ) {
      errorMsg("Чтобы заменить что-то из результатов поиска, необходимо, чтобы выделение находилось на одном из результатов. Самый простой способ достичь этого -- нажать на кнопки '►' или 'Искать'.");
      return false;
    }

  QString currentContent = this->dictModel->data(ui->dictionaryTable->currentIndex()).toString();
  QString replacedContent = this->dictModel->data(ui->dictionaryTable->currentIndex()).toString().replace(ui->simpleSearchInput->text(), ui->simpleReplaceInput->text(), Qt::CaseSensitive);
  if (currentContent == replacedContent) {
      qDebug() << "Было: " << currentContent << " стало: " << replacedContent;
      errorMsg("В выделенной ячейке всё осталось по-прежнему: похоже, Вы заменили подстроку на саму себя. Менее вероятный (но возможный) вариант: Вы подменили строку поиска и не нажали кнопку 'Искать'. В этом случае нажмите на кнопку поиска.");
      return false;
    }

  if (! this->dictModel->setData(ui->dictionaryTable->currentIndex(), replacedContent, Qt::EditRole) ) {
      errorMsg("По неизвестной причине заменить содержимое в ячейке не удалось. Напишите об этом разработчику и опишите ситуацию как можно подробнее.");
      return false;
    } else {
      lastSearchResults.removeAt(lastSearchResults.indexOf(ui->dictionaryTable->currentIndex()));
      this->simpleSearchNext();
      return true;
    }
}

bool TabContents::simpleSearchPrevious() {
  if (lastSearchResults.empty()) {
      return false;
    }
  if (!currentResult->hasPrevious()) {
      currentResult->toBack();
    }

  if (currentResult->hasPrevious()) {
      ui->dictionaryTable->clearSelection();
      ui->dictionaryTable->setCurrentIndex(currentResult->peekPrevious());
      currentResult->previous();
      qDebug() << ui->dictionaryTable->currentIndex().row();
      ui->dictionaryTable->setFocus();
      emit search_activated();
      return true;
    } else {
      return false;
    }
}

bool TabContents::showFiles(bool isShown) {
  ui->filesBox->setHidden(!isShown);
  ui->filesBox->setChecked(isShown);
  ui->showFilesButton->setHidden(isShown);
  return isShown;
}

bool TabContents::showForms(bool isShown) {
  ui->groupBox->setHidden(!isShown);
  ui->groupBox->setChecked(isShown);
  ui->showFormsButton->setHidden(isShown);
  return isShown;
}

bool TabContents::showActionsOnDict(bool isShown) {
  ui->groupBoxActionsOnDict->setHidden(!isShown);
  ui->groupBoxActionsOnDict->setChecked(isShown);
  ui->showActionsOnDict->setHidden(isShown);
  return isShown;
}

bool TabContents::showActionsOnWord(bool isShown) {
  ui->groupBoxActionsOnWord->setHidden(!isShown);
  ui->groupBoxActionsOnWord->setChecked(isShown);
  ui->showActionsOnWord->setHidden(isShown);
  return isShown;
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
  praatListToSave.append(QPair<QVariant, QString>(praatblobid, basename));
  return true;
}

bool TabContents::saveChangesInPraat() {
  QString temp_dir = QDir::tempPath();
  QStringList args;
  QString arg1 = QString("praat");
  QString arg2 = QString("execute %1/scripts/saveallobjectstotemp.praat %2").arg(QDir::currentPath(), temp_dir);
  qDebug() << "args for praatsend: " << arg1 << arg2;
  args.append(arg1);
  args.append(arg2);
  QProcess::execute("sendpraat.exe", args);

  for (int i=0; i < praatListToSave.size(); i++) {
      QVariant blobid = praatListToSave[i].first;
      QString basename = praatListToSave[i].second;

      qDebug() << "temp is here: " << temp_dir;
      QString markuppath = QDir::toNativeSeparators(temp_dir + QString("/") + basename + QString(".TextGrid"));
      QString soundpath = QDir::toNativeSeparators(temp_dir + QString("/") + basename + QString(".wav"));

      QByteArray praatMarkup;
      QByteArray praatSound;
      readAndCompress(markuppath, praatMarkup);
      readAndCompress(soundpath, praatSound);

      // here we need to push sound to blobs table, get id of blob and make a record in blobs_description_table
      QSqlQuery praatBlobQuery(db);
      praatBlobQuery.prepare("UPDATE blobs SET mainblob = :praatmarkupblob, secblob = :praatsoundblob WHERE id = :blobid");
      praatBlobQuery.bindValue(":praatmarkupblob", praatMarkup);
      praatBlobQuery.bindValue(":praatsoundblob", praatSound);
      praatBlobQuery.bindValue(":blobid", blobid);

      if (!praatBlobQuery.exec()) {
          qDebug() << "Can't update praat blob";
          qDebug() << "Query was the following: " << getLastExecutedQuery(praatBlobQuery);
          qDebug() << db.lastError().text();
          this->clearForms();
          praatModel->select();
          return false;
      }
  }
  praatModel->select();
  praatListToSave.clear();
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
  this->updateModel();
  ui->dictionaryTable->setCurrentIndex(index);
  return true;
}

bool TabContents::updateModel() {
  QModelIndex index = ui->dictionaryTable->currentIndex();
  this->dictModel->select();
  while (this->dictModel->canFetchMore()) {
      this->dictModel->fetchMore();
  }
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


DictGlobalAttributes & TabContents::getDictAttrs() {
  return dictAttrs;
}


TabContents::~TabContents()
{
  qDebug() << "I'm in tab destructor";
  delete copyAction;
  delete dictModel;
  delete soundsModel;
  delete praatModel;

  QString connname = db.connectionName();
  QSqlDatabase tempconnection = QSqlDatabase::database(connname, false);

  if (tempconnection.isOpen()) {
    tempconnection.commit();
    tempconnection.close();
  }

  QSqlDatabase::removeDatabase(connname);

  static_cast<DictTabsContainer *>(this->parentWidget()->parentWidget())->removeDictFromSet(dictAttrs);

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
//  record.setValue("regular_form" , entry.getTranscription());
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

  this->updateModel();


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
  ui->praatButton->setText("Файл разметки Praat");
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
