#ifndef SIMPLESEARCH_H
#define SIMPLESEARCH_H

#include <QDialog>

namespace Ui {
  class SimpleSearch;
}

class SimpleSearch : public QDialog
{
  Q_OBJECT

public:
  explicit SimpleSearch(QWidget *parent = 0);
  ~SimpleSearch();

private:
  Ui::SimpleSearch *ui;
};

#endif // SIMPLESEARCH_H
