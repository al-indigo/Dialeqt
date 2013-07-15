#include <QDateTime>

#include "dialogcreatenewdictionary.h"
#include "ui_dialogcreatenewdictionary.h"
#include "dictglobalattributes.h"

DialogCreateNewDictionary::DialogCreateNewDictionary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateNewDictionary)
{
  ui->setupUi(this);
}

DialogCreateNewDictionary::~DialogCreateNewDictionary()
{
  delete ui;
}

DictGlobalAttributes DialogCreateNewDictionary::getData()
{
  this->filename = ui->dict_filename->text();
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
