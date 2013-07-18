#ifndef ETIMOLOGYWINDOW_H
#define ETIMOLOGYWINDOW_H

#include <QWidget>
#include <QDialog>

namespace Ui {
  class EtimologyWindow;
}

class EtimologyWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit EtimologyWindow(QWidget *parent = 0);
  ~EtimologyWindow();
  
private:
  Ui::EtimologyWindow *ui;
};

#endif // ETIMOLOGYWINDOW_H
