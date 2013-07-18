#ifndef TALESWINDOW_H
#define TALESWINDOW_H

#include <QWidget>
#include <QDialog>

namespace Ui {
  class TalesWindow;
}

class TalesWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit TalesWindow(QWidget *parent = 0);
  ~TalesWindow();
  
private:
  Ui::TalesWindow *ui;
};

#endif // TALESWINDOW_H
