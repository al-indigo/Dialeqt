#ifndef DIALEQT_H
#define DIALEQT_H

#include <QMainWindow>
#include <QDebug>

namespace Ui {
class Dialeqt;
}

class Dialeqt : public QMainWindow
{
  Q_OBJECT
  QDialog *searchWindow;
  QLayout *searchWindowLayout;
  QTabWidget *searchtabscontainer;

public:
    explicit Dialeqt(QWidget *parent = 0);
    ~Dialeqt();

public slots:
  void closeSearchTab(int tab);
    
private slots:
    void on_createDictMenuButton_triggered();
    void on_openDictMenuButton_triggered();
    void on_searchButton_triggered();
private:
    Ui::Dialeqt *ui;
};

#endif // DIALEQT_H
