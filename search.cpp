#include "search.h"
#include "ui_search.h"

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>

#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QDebug>

Search::Search(const QSet<DictGlobalAttributes> * attrs, QWidget *parent) :
  openedDicts(attrs),
  QDialog(parent),
//  dictsToSearch(&(ui->verticalLayout_2)),
  ui(new Ui::search)
{
  ui->setupUi(this);

//  QCheckBox testchk;
//  QLabel testlabel(tr("QWE"));
//  QLineEdit testlndt;
//  this->dictsToSearch.addWidget(&testchk, 0, 0);
//  this->dictsToSearch.addWidget(&testlabel, 0, 1);
//  this->dictsToSearch.addWidget(&testlndt, 0, 2);
//  ui->whereToSearchBox->setLayout(&(this->dictsToSearch));

//  formGroupBox = new QGroupBox(tr("Form layout"));

//      ui->whereToSearchBox->setMinimumHeight(300);
//      ui->whereToSearchBox->setMinimumWidth(600);
//      ui->whereToSearchBox->setLayout(layout);

  layout = new QGridLayout(ui->whereToSearchBox);

  QSet<int> set;
  set.insert(1);
  set.insert(2);
  set.insert(3);

  int counter = 0;
  for (QSet<DictGlobalAttributes>::const_iterator i = attrs->begin(); i != attrs->end(); ++i, counter++) {
      QCheckBox * checkbox = new QCheckBox(tr((*i).getDictname().toStdString().c_str()));
      checkbox->setProperty("row", counter);
      checkbox->setProperty("connection name", (*i).getFilename());
      layout->addWidget(checkbox, counter, 0);
//      layout->addWidget(new QLabel(tr(QVariant(counter).toString().toStdString().c_str())), counter, 1);
      layout->addWidget(new QLineEdit(), counter, 1);
  }

  ui->whereToSearchBox->setLayout(layout);

  connect(ui->findButton, SIGNAL(clicked()), this, SLOT(onSearchClick()));

}

bool Search::onSearchClick() {
  QList<QCheckBox *> ch = ui->whereToSearchBox->findChildren<QCheckBox *>();
  qDebug() << "List size:" << ch.size();
  for (QList<QCheckBox *>::iterator i = ch.begin(); i != ch.end(); ++i) {
      bool checked = ( (*i)->isChecked());
      if (checked) {
          qDebug() << (*i)->property("row");
          qDebug() << (*i)->property("connection name");
        }
    }
  return true;
}

Search::~Search()
{
  delete ui;
}
