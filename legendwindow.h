#ifndef LEGENDWINDOW_H
#define LEGENDWINDOW_H

#include "dictglobalattributes.h"

#include <QDialog>
#include <QWidget>

namespace Ui {
  class legendWindow;
}

class LegendWindow : public QDialog
{
  Q_OBJECT
  DictGlobalAttributes *attrs;

public:
  explicit LegendWindow(DictGlobalAttributes *_attrs, QWidget *parent = 0);
  ~LegendWindow();

  void setNewDictData();

private slots:
  bool validateData();

private:
  Ui::legendWindow *ui;
};

#endif // LEGENDWINDOW_H
