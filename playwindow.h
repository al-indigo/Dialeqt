#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <QWidget>
#include <QDialog>
namespace Ui {
  class PlayWindow;
}

class PlayWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit PlayWindow(QWidget *parent = 0);
  ~PlayWindow();
  
private:
  Ui::PlayWindow *ui;
};

#endif // PLAYWINDOW_H
