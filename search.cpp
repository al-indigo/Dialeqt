#include "search.h"
#include "ui_search.h"
#include "utils.h"

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QProgressBar>

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
      checkbox->setChecked(true);
      layout->addWidget(checkbox, counter, 0);
//      layout->addWidget(new QLabel(tr(QVariant(counter).toString().toStdString().c_str())), counter, 1);
      QLineEdit * line = new QLineEdit();

      line->setToolTip("В этом поле вы можете задать локальные переменные поиска для каждого из словарей. \nО переменных подробнее написано в всплывающей подсказке в поле 'глобальные переменные поиска'");
      layout->addWidget(line, counter, 1);
  }

  ui->whereToSearchBox->setLayout(layout);

  connect(ui->findButton, SIGNAL(clicked()), this, SLOT(onSearchClick()));
}


bool Search::checkVars(QString line) {
  QStringList varsList = line.split(" ");
  QRegExp rx("(.=(.+(,)?)+)*");
  foreach (const QString &item, varsList) {
      if (!rx.exactMatch(item)) return false;
  }

  return true;
}

bool Search::checkQuery(QString line) {
  if (line.contains("delete", Qt::CaseInsensitive) || line.contains("update", Qt::CaseInsensitive) || line.contains("insert", Qt::CaseInsensitive)) {
      errorMsg("Пожалуйста, не пытайтесь сломать программу (я знаю, это довольно легко, но в то же время абсолютно бесполезно).");
      return false;
    }
  return true;
}

QMap<QString, QList<QString> > Search::parseVars(QString line) {
  QMap<QString, QList<QString> > vars;
  return vars;
}

QStringList constructDictionary(const QStringList& initialLine, QString vars) {
  QStringList dictionary;
  QStringList plainDictionary; //this list contains lines that don't need to have replaces

  foreach (const QString &line, initialLine) {
    if (!line.contains("$")) {
        plainDictionary.append(line);
      } else {
        dictionary.append(line);
      }
    }

  foreach(const QString& item, dictionary) {
      qDebug() << "For replace: " << item;
    }

  foreach(const QString& item, plainDictionary) {
      qDebug() << "Don't need changes: " << item;
    }

  foreach(const QString& item, dictionary) {
      qDebug() << "Replacing: " << item;
    }

  QStringList varsSeparate = vars.split(" ");

  foreach(const QString& item, varsSeparate) {
      qDebug() << "Vars separate: " << item;
    }

  foreach (const QString& line, dictionary) {
    foreach (const QString& item, varsSeparate) {
        QString varname = item[0];
        varname.prepend("$");
        if (!line.contains(varname)) {
            qDebug() << line << " doesn't contain $" << varname;
            continue;
          } else {
            QString onlyValues = item.mid(2);
            QStringList valuesList = onlyValues.split(",");
            long int sizeBefore = dictionary.size();
            foreach (const QString& value, valuesList) {
                if (value.trimmed().isEmpty()) {
                    continue;
                  }
                qDebug() << "replacing " << varname << " with " << value;

                foreach (const QString& word, dictionary) {
                    QString tempWord = word;
                    qDebug() << tempWord;
                    tempWord.replace(varname, value);
                    if (tempWord != word) {
                      dictionary.append(tempWord);
                      qDebug() << tempWord << " " << tempWord.replace(varname, value);
                      }
                  }
              }
            for (int i = 0; i < sizeBefore; i++) {
                dictionary.removeFirst();
              }
          }
      }
    }

  dictionary.append(plainDictionary);
  return dictionary;

}

QString constructQuery(QString line, QMap<QString, QList<QString> > vars) {
  return QString("");
}

bool Search::onSearchClick() {
  QProgressBar * progress = new QProgressBar(this);
  ui->verticalLayout_2->addWidget(progress);
  progress->setValue(20);
  if (!this->checkVars(ui->globalVarsLine->text())) {
      errorMsg("Вы неправильно заполнили поле глобальных переменных. Наведите мышь на поле ввода глобальных переменных на пару секунд, и выскочит подсказка, в которой написано, как правильно задавать переменные.");
    }

  QList<QCheckBox *> ch = ui->whereToSearchBox->findChildren<QCheckBox *>();
  qDebug() << "List size:" << ch.size();
  for (QList<QCheckBox *>::iterator i = ch.begin(); i != ch.end(); ++i) {
      bool checked = ( (*i)->isChecked());
      if (checked) {
          qDebug() << (*i)->property("row");
          qDebug() << (*i)->property("connection name");

          QLineEdit * varline = (QLineEdit *) layout->itemAtPosition((*i)->property("row").toInt(), 1)->widget();
          if (varline != NULL) {
            qDebug() << "Checking var line";
            if (!this->checkVars(varline->text())) {
                QString msg = "Вы неправильно заполнили поле переменных. Наведите мышь на поле ввода глобальных переменных на пару секунд, и выскочит подсказка, в которой написано, как правильно задавать переменные. \nОшибка в строке: ";
                msg.append(varline->text());
                errorMsg(msg);
              }
            QStringList dict;
            dict.push_back(ui->searchLine->text());
            dict = constructDictionary(dict, varline->text());
            foreach(const QString& item, dict) {
                qDebug() << "Replaced: " << item;
              }
          }

        }
    }

  ui->verticalLayout_2->removeWidget(progress);
  delete progress;
  return true;
}

Search::~Search()
{
  delete ui;
}
