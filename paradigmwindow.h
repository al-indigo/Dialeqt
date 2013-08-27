#ifndef PARADIGMWINDOW_H
#define PARADIGMWINDOW_H

#include <QWidget>
#include <QDialog>
#include "tabcontents.h"
namespace Ui {
  class ParadigmWindow;
}

class ParadigmWindow : public TabContents
{
  Q_OBJECT
  
public:
  explicit ParadigmWindow(DictGlobalAttributes _dictAttrs, TabContents *parent=0);
  ~ParadigmWindow();
  
private:
  Ui::ParadigmWindow *ui;
};

#endif // PARADIGMWINDOW_H
