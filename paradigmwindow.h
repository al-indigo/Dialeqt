#ifndef PARADIGMWINDOW_H
#define PARADIGMWINDOW_H

#include <QWidget>
#include <QDialog>
namespace Ui {
  class ParadigmWindow;
}

class ParadigmWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit ParadigmWindow(QWidget *parent = 0);
  ~ParadigmWindow();
  
private:
  Ui::ParadigmWindow *ui;
};

#endif // PARADIGMWINDOW_H
