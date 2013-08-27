#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QAbstractItemView>
#include <QSqlDatabase>

#include "playlistmodel.h"

namespace Ui {
  class PlayWindow;
}

class PlayWindow : public QDialog
{
  Q_OBJECT

  Ui::PlayWindow *ui;
  bool soundChosen;
  QSqlDatabase db;
  QString destdir;
  QVariant wordid;
  QMediaPlayer player;
  QMediaPlaylist playlist;
  PlaylistModel playlistModel;
  QAbstractItemView *playlistView;
  QString filenameToSubmit;
  qint64 duration;

public:
  explicit PlayWindow(QStringList _playlist, QSqlDatabase _db, QString _destdir, QVariant _wordid, QWidget *parent = 0);
  ~PlayWindow();
  
  qreal playbackRate() const;

private:
  void updateDurationInfo(qint64 currentInfo);
  void AddSoundToDatabase();
  void clearForms();

private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);
    void updateRate();
    void setPlaybackRate(float rate);
    void setVolume(int volume);
    void chooseFile();
    void checkDescription();
    void addSoundToDatabase();


signals:
    void changeRate(qreal rate);
    void changeVolume(int volume);

};

#endif // PLAYWINDOW_H
