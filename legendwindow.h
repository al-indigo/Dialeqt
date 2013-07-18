#ifndef LEGENDWINDOW_H
#define LEGENDWINDOW_H

#include <QDialog>
#include <QWidget>

namespace Ui {
  class legendWindow;
}

class LegendWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit LegendWindow(QWidget *parent = 0);
  ~LegendWindow();
  
private:
  Ui::legendWindow *ui;
};

#endif // LEGENDWINDOW_H
