#include <QUrl>
#include <QTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QFileDialog>

#include "customquerydiagnostics.h"

#include "playwindow.h"
#include "ui_playwindow.h"
#include "utils.h"

PlayWindow::PlayWindow(QStringList _playlist, QSqlDatabase _db, QString _destdir, QVariant _wordid, QWidget *parent) :
  QDialog(parent),
  soundChosen(false),
  player(this),
  playlistModel(this),
  db(_db),
  destdir(_destdir),
  wordid(_wordid),
  ui(new Ui::PlayWindow)
{
  ui->setupUi(this);
  foreach (const QString & str, _playlist) {
      playlist.addMedia(QUrl::fromLocalFile(str));
  }
  player.setPlaylist(&playlist);

  playlistModel.setPlaylist(&playlist);
  ui->playlist->setModel(&playlistModel);
  ui->playlist->setCurrentIndex(playlistModel.index(playlist.currentIndex(), 0));

  ui->rateBox->addItem("0.5x", QVariant(0.5));
  ui->rateBox->addItem("0.75x", QVariant(0.75));
  ui->rateBox->addItem("0.9x", QVariant(0.9));
  ui->rateBox->addItem("1.0x", QVariant(1.0));
  ui->rateBox->addItem("1.2x", QVariant(1.2));
  ui->rateBox->addItem("1.5x", QVariant(1.5));
  ui->rateBox->addItem("2.0x", QVariant(2.0));
  ui->rateBox->setCurrentIndex(3);

  ui->volumeSlider->setRange(0, 100);
  ui->volumeSlider->setValue(100);

  connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(changeVolume(int)));
  connect(ui->rateBox, SIGNAL(activated(int)), this, SLOT(updateRate()));
  connect(this, SIGNAL(changeRate(qreal)), &player, SLOT(setPlaybackRate(qreal)));
  connect(this, SIGNAL(changeVolume(int)), &player, SLOT(setVolume(int)));

  connect(ui->playButton, SIGNAL(clicked()), &player, SLOT(play()));
  connect(ui->pauseButton, SIGNAL(clicked()), &player, SLOT(pause()));
  connect(&player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
  connect(&player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
  connect(&playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
  connect(ui->progressBar, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
  connect(ui->playlist, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

  connect(ui->selectFile, SIGNAL(clicked()), this, SLOT(chooseFile()));
  connect(ui->soundDescription, SIGNAL(textChanged(QString)), this, SLOT(checkDescription()));
  connect(ui->submit, SIGNAL(clicked()), this, SLOT(addSoundToDatabase()));

  ui->submit->setDisabled(true);
}

void PlayWindow::chooseFile() {
  if (!this->soundChosen) {
      this->filenameToSubmit = QFileDialog::getOpenFileName(this, tr("Выберите звуковой файл для вставки в словарь"), "", tr("Звуковые файлы (*.mp3 *.wav)"));
      qDebug() << "Sound file selected: " << this->filenameToSubmit;
      if (!this->filenameToSubmit.isEmpty()) {
        ui->selectFile->setText("Сбросить звук?");
        this->soundChosen = true;
        if (!ui->soundDescription->text().isEmpty()) ui->submit->setEnabled(true);
      }
  } else {
      this->filenameToSubmit.clear();
      ui->selectFile->setText("Выберите звуковой файл");
      this->soundChosen = false;
      ui->submit->setDisabled(true);
  }
}

void PlayWindow::checkDescription() {
  if (!ui->soundDescription->text().isEmpty()) {
      ui->submit->setEnabled(true);
  } else {
      ui->submit->setDisabled(true);
  }
}

void PlayWindow::addSoundToDatabase()
{
  if (this->filenameToSubmit.isEmpty() || ui->soundDescription->text().isEmpty()) {
      errorMsg("Звуковой файл должен существовать, и его описание не может быть пустым.");
      return;
    }
  QByteArray sound;
  readAndCompress(this->filenameToSubmit, sound);
  QFileInfo fileinfo = QFileInfo(this->filenameToSubmit);
  qDebug() << sound.size();

  QVariant soundblobid;

  // here we need to push sound to blobs table, get id of blob and make a record in blobs_description_table
  QSqlQuery soundBlobQuery(db);
  soundBlobQuery.prepare("INSERT INTO blobs(mainblob) values (:soundblob)");
  soundBlobQuery.bindValue(":soundblob", sound);
  if (!soundBlobQuery.exec()) {
      qDebug() << "Can't insert sound blob";
      qDebug() << "Query was the following: " << getLastExecutedQuery(soundBlobQuery);
      qDebug() << db.lastError().text();
      this->clearForms();
      return ;
  }
  soundblobid = soundBlobQuery.lastInsertId();
  soundBlobQuery.clear();

  QSqlQuery soundDescriptionQuery(db);
  soundDescriptionQuery.prepare("INSERT INTO dict_blobs_description(type,name,description,wordid,blobid)"
                                 "VALUES (1,:name,:description,:wordid,:blobid)");
  soundDescriptionQuery.bindValue(":name", fileinfo.fileName());
  soundDescriptionQuery.bindValue(":description", ui->soundDescription->text());
  soundDescriptionQuery.bindValue(":wordid", wordid);
  soundDescriptionQuery.bindValue(":blobid", soundblobid);

  if (!soundDescriptionQuery.exec()) {
      qDebug() << "Can't insert sound blob description";
      qDebug() << "Query was the following: " << getLastExecutedQuery(soundDescriptionQuery);
      qDebug() << db.lastError().text();
      this->clearForms();
      return ;
  }
  qDebug() << "Successfully submitted sound";

  playlist.addMedia(QUrl::fromLocalFile(filenameToSubmit));

  this->clearForms();
}

void PlayWindow::durationChanged(qint64 duration)
{
    this->duration = duration/1000;
    ui->progressBar->setMaximum(duration / 1000);
}

void PlayWindow::positionChanged(qint64 progress)
{
    if (!ui->progressBar->isSliderDown()) {
        ui->progressBar->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void PlayWindow::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        playlist.setCurrentIndex(index.row());
        player.play();
    }
}

void PlayWindow::playlistPositionChanged(int currentItem)
{
    ui->playlist->setCurrentIndex(playlistModel.index(currentItem, 0));
}

void PlayWindow::seek(int seconds)
{
    player.setPosition(seconds * 1000);
}

void PlayWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->labelDuration->setText(tStr);
}

void PlayWindow::setVolume(int volume)
{
    if (ui->volumeSlider)
        ui->volumeSlider->setValue(volume);
}

qreal PlayWindow::playbackRate() const
{
    return ui->rateBox->itemData(ui->rateBox->currentIndex()).toDouble();
}

void PlayWindow::setPlaybackRate(float rate)
{
    for (int i = 0; i < ui->rateBox->count(); ++i) {
        if (qFuzzyCompare(rate, float(ui->rateBox->itemData(i).toDouble()))) {
            ui->rateBox->setCurrentIndex(i);
            return;
        }
    }

    ui->rateBox->addItem(QString("%1x").arg(rate), QVariant(rate));
    ui->rateBox->setCurrentIndex(ui->rateBox->count() - 1);
}

void PlayWindow::updateRate()
{
    emit changeRate(playbackRate());
}

void PlayWindow::clearForms() {
  ui->soundDescription->clear();
  this->filenameToSubmit.clear();
  ui->submit->setDisabled(true);
  soundChosen = false;
  ui->selectFile->setText("Выберите звуковой файл");

}

PlayWindow::~PlayWindow()
{
    disconnect(ui->playButton, SIGNAL(clicked()), &player, SLOT(play()));
    disconnect(ui->pauseButton, SIGNAL(clicked()), &player, SLOT(pause()));
    disconnect(&player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    disconnect(&player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    disconnect(&playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(playlistPositionChanged(int)));
    disconnect(ui->progressBar, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    disconnect(ui->playlist, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));
    disconnect(ui->selectFile, SIGNAL(clicked()), this, SLOT(chooseFile()));
    disconnect(ui->soundDescription, SIGNAL(textChanged(QString)), this, SLOT(checkDescription()));
    disconnect(ui->submit, SIGNAL(clicked()), this, SLOT(addSoundToDatabase()));
    delete ui;
}
