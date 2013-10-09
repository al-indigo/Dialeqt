#include "legendwindow.h"
#include "ui_legendwindow.h"

#include "utils.h"
#include <QDebug>

LegendWindow::LegendWindow(DictGlobalAttributes *_attrs, QWidget *parent) :
  attrs(_attrs),
  QDialog(parent),
  ui(new Ui::legendWindow)
{
  ui->setupUi(this);
  qDebug() << "Opened attrs window, setting the following params";
  attrs->debugPrint();
  ui->labelAuthor->setText(attrs->getAuthor());
  ui->labelDictName->setText(attrs->getDictname());
  QString coAuthors;
  foreach (const QString &item, attrs->getCoauthors()) {
      coAuthors.append(item + QString(","));
    }
  QString tags;
  foreach (const QString &item, attrs->getTags()) {
      tags.append(item + QString(","));
    }
  ui->textCoauthors->setPlainText(coAuthors);
  ui->textDescription->setPlainText(attrs->getDescription());
  ui->textTags->setPlainText(tags);
  connect(ui->saveCloseButton, SIGNAL(clicked()), this, SLOT(validateData()));
  connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

bool LegendWindow::validateData() {
  if (ui->textCoauthors->toPlainText().isEmpty()) {
      errorMsg("Вы не ввели имена соавторов словаря. Если вы один, напишите здесь своё имя тоже, программе будет приятно");
      return false;
  }
  if (ui->textTags->toPlainText().isEmpty()) {
      errorMsg("Вы не классифицировали словарь, нужно ввести хотя бы одну группу");
      return false;
  }

  if (ui->textDescription->toPlainText().isEmpty()) {
      errorMsg("Вы не ввели описание словаря");
      return false;
  }

  accept();
  return true;

}

void LegendWindow::setNewDictData()
{
  QStringList credits_list = ui->textCoauthors->toPlainText().split(",");
  attrs->setCoauthors(credits_list);

  QStringList tags_list = ui->textTags->toPlainText().split(",");
  attrs->setTags(tags_list);

  attrs->setDescription(ui->textDescription->toPlainText());

  return;
}

LegendWindow::~LegendWindow()
{
  delete ui;
}
