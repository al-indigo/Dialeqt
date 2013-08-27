#include <QDateTime>
#include <QDir>

#include "dialogcreatenewdictionary.h"
#include "ui_dialogcreatenewdictionary.h"
#include "dictglobalattributes.h"
#include "utils.h"

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
