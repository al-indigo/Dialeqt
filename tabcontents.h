#ifndef TABCONTENTS_H
#define TABCONTENTS_H

#include <QWidget>

namespace Ui {
  class TabContents;
}

class TabContents : public QWidget
{
  Q_OBJECT
  
public:
  explicit TabContents(QWidget *parent = 0);
  ~TabContents();
  
private:
  Ui::TabContents *ui;
};

#endif // TABCONTENTS_H
