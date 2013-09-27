#include "legendwindow.h"
#include "ui_legendwindow.h"

LegendWindow::LegendWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::legendWindow)
{
  ui->setupUi(this);
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

DictGlobalAttributes DialogCreateNewDictionary::getData()
{
  QDir dictsDir = QDir("dictionaries");

  this->filename = dictsDir.absoluteFilePath(ui->dict_filename->text()).append(".sqlite");
  this->dictname = ui->dict_name->text();
  this->author = ui->dict_author->text();
  QStringList credits_list = ui->dict_credits->text().split(",");
  foreach (const QString &item, credits_list) {
      if(0 != item.capacity()) {
        this->credits.insert(item.simplified());
      }
    }
  QStringList tags_list = ui->dict_tags->text().split(",");
  foreach (const QString &item, tags_list) {
      if(0 != item.capacity()) {
        this->tags.insert(item.simplified());
      }
    }
  this->description = ui->dict_description->toPlainText();
  QString dbId(QDateTime::currentDateTimeUtc().toString().append(dictname).append(author));

  DictGlobalAttributes dictAttrs(dbId,filename,dictname,author,credits,tags,description);

  return dictAttrs;
}
*/

LegendWindow::~LegendWindow()
{
  delete ui;
}
