#ifndef PHONOLOGYWINDOW_H
#define PHONOLOGYWINDOW_H

#include <QWidget>
#include <QDialog>

namespace Ui {
  class PhonologyWindow;
}

class PhonologyWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit PhonologyWindow(QWidget *parent = 0);
  ~PhonologyWindow();
  
private:
  Ui::PhonologyWindow *ui;
};

#endif // PHONOLOGYWINDOW_H
