#include "legendwindow.h"
#include "ui_legendwindow.h"

#include "utils.h"

LegendWindow::LegendWindow(DictGlobalAttributes *_attrs, QWidget *parent) :
  attrs(_attrs),
  QDialog(parent),
  ui(new Ui::legendWindow)
{
  ui->setupUi(this);
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




/*
 * Не забыть: изменения в бд.
 * 1) хранить легенду в таблице tales
 * 2) сохранять/доставать данные, как написано ниже в закомментированном коде (не копировать, просто использовать ту же схему)
 * 3) отрефакторить таблицы блобов.
 * 4) создать всё-таки глобальную таблицу блобов, где FK будет айдишник истории.
 */

/*======== Filling initial data ==============*/
/*
query.prepare("insert into dict_attributes ( "
                "dict_identificator, "
                "dict_name, "
                "dict_author, "
                "dict_coauthors, "
                "dict_classification_tags, "
                "dict_description, "
                "dict_legend) "
              "values ( "
                ":dictId, "
                ":dictName, "
                ":dictAuthor, "
                ":dictCoathors, "
                ":dictTags, "
                ":dictDescription, "
                ":dictLegend);"
              );
query.bindValue(":dictId", attrs.getDbId());
query.bindValue(":dictName", attrs.getDictname());
query.bindValue(":dictAuthor", attrs.getAuthor());
//TODO: make set serialization/deserialization
query.bindValue(":dictCoathors", *attrs.getCoauthors().begin());
query.bindValue(":dictTags", *attrs.getTags().begin());

query.bindValue(":dictDescription", attrs.getDescription());
query.bindValue(":dictLegend", attrs.getLegend());
qDebug() << "Going to execute the following query: " << query.lastQuery();

if (!query.exec()) {
    qDebug() << "Failed to fill initial data in global_attrs table";
    qDebug() << "Query was the following: " << getLastExecutedQuery(query);
    qDebug() << db.lastError().text();
    return false;
} else {
    return true;
}
*/

/*
DialogCreateNewDictionary::DialogCreateNewDictionary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateNewDictionary)
{
  ui->setupUi(this);
  connect(ui->okButton, SIGNAL(clicked()), this, SLOT(validateData()));
  connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

DialogCreateNewDictionary::~DialogCreateNewDictionary()
{
  delete ui;
}

bool DialogCreateNewDictionary::validateData() {
  if (ui->dict_filename->text().isEmpty()) {
      errorMsg("Вы не ввели имя файла словаря");
      return false;
  }
  if (ui->dict_name->text().isEmpty()) {
      errorMsg("Вы никак не назвали словарь");
      return false;
  }
  if (ui->dict_author->text().isEmpty()) {
      errorMsg("Вы не назвали себя (вы же автор словаря?)");
      return false;
  }
  if (ui->dict_credits->text().isEmpty()) {
      errorMsg("Вы не ввели имена соавторов словаря. Если вы один, напишите здесь своё имя тоже, программе будет приятно");
      return false;
  }
  if (ui->dict_tags->text().isEmpty()) {
      errorMsg("Вы не классифицировали словарь, нужно ввести хотя бы одну группу");
      return false;
  }

  if (ui->dict_description->toPlainText().isEmpty()) {
      errorMsg("Вы не ввели описание словаря");
      return false;
  }

  accept();
  return true;

}
*/

LegendWindow::~LegendWindow()
{
  delete ui;
}
