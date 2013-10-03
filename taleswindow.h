#ifndef TALESWINDOW_H
#define TALESWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
  class TalesWindow;
}

class TalesWindow : public QDialog
{
  Q_OBJECT
  QSqlDatabase db;
  int currentTaleId;
  bool init;
  QSqlTableModel *talesListModel;
  QSqlTableModel *attachListModel;

public:
  explicit TalesWindow(QSqlDatabase _db, QWidget *parent = 0);
  ~TalesWindow();

public slots:
  bool addTale();
  bool saveCurrentTale();
  bool loadCurrentTale();
  bool uploadFile();
private:
  Ui::TalesWindow *ui;
  void initializeTalesListModel(QSqlTableModel *model);
  void initializeAttachModel(QSqlTableModel *model);
  void talesReinit();

};

#endif // TALESWINDOW_H
