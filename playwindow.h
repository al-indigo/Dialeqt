#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QAbstractItemView>
#include "playlistmodel.h"

namespace Ui {
  class PlayWindow;
}

class PlayWindow : public QDialog
{
  Q_OBJECT

  Ui::PlayWindow *ui;
  QMediaPlayer player;
  QMediaPlaylist playlist;
  PlaylistModel playlistModel;
  QAbstractItemView *playlistView;
  qint64 duration;

public:
  explicit PlayWindow(QStringList _playlist, QWidget *parent = 0);
  ~PlayWindow();
  
private:
  void updateDurationInfo(qint64 currentInfo);
private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);
};

#endif // PLAYWINDOW_H
