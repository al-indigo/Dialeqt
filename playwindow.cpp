#include <QUrl>
#include <QTime>

#include "playwindow.h"
#include "ui_playwindow.h"

PlayWindow::PlayWindow(QStringList _playlist, QWidget *parent) :
  QDialog(parent),
  player(this),
  playlistModel(this),
  ui(new Ui::PlayWindow)
{
  ui->setupUi(this);
  QList <QUrl> playlisturls = QUrl::fromStringList(_playlist);
  foreach (const QUrl & item, playlisturls) {
      playlist.addMedia(item);
    }
  player.setPlaylist(&playlist);

  playlistModel.setPlaylist(&playlist);
  ui->playlist->setModel(&playlistModel);
  ui->playlist->setCurrentIndex(playlistModel.index(playlist.currentIndex(), 0));

  connect(ui->playButton, SIGNAL(clicked()), &player, SLOT(play()));
  connect(ui->pauseButton, SIGNAL(clicked()), &player, SLOT(pause()));
  connect(&player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
  connect(&player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
  connect(&playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
  connect(ui->progressBar, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
  connect(ui->playlist, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

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

PlayWindow::~PlayWindow()
{
    disconnect(ui->playButton, SIGNAL(clicked()), &player, SLOT(play()));
    disconnect(ui->pauseButton, SIGNAL(clicked()), &player, SLOT(pause()));
    disconnect(&player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    disconnect(&player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    disconnect(&playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(playlistPositionChanged(int)));
    disconnect(ui->progressBar, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    disconnect(ui->playlist, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    delete ui;
}
