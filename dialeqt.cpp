#include "dialeqt.h"
#include "ui_dialeqt.h"
#include "dialogcreatenewdictionary.h"
#include "dictdbfactory.h"
#include "search.h"

#include "QFileDialog"
#include "utils.h"

Dialeqt::Dialeqt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dialeqt)
{
    ui->setupUi(this);

    searchWindow = new QDialog(this);
    searchWindowLayout = new QVBoxLayout(searchWindow);
    searchtabscontainer = new QTabWidget(searchWindow);
    searchtabscontainer->setTabsClosable(true);
    searchtabscontainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    searchtabscontainer->insertTab(0, new QWidget(), "dummy tab");

    connect(searchtabscontainer, SIGNAL(tabCloseRequested(int)), this, SLOT(closeSearchTab(int)));

    searchWindowLayout->addWidget(searchtabscontainer);
    searchWindow->setLayout(searchWindowLayout);

    ui->dictsTabsContainerWidget->setAccessibleName("dict tabs");
}

void Dialeqt::closeSearchTab(int tab) {
  delete searchtabscontainer->widget(tab);
}

Dialeqt::~Dialeqt()
{
  delete searchtabscontainer;
  delete searchWindowLayout;
  delete searchWindow;

  delete ui;
}

void Dialeqt::on_createDictMenuButton_triggered()
{
//    DialogCreateNewDictionary *newDictDialog = new DialogCreateNewDictionary(this);
//    newDictDialog->show();
    DialogCreateNewDictionary dictDialog(this);
    if( dictDialog.exec() == QDialog::Accepted ) {
        qDebug() << "accepted";
//        DictGlobalAttributes dictAttrs(&dictDialog.getData());
        DictGlobalAttributes dictAttrs(dictDialog.getData());
        dictAttrs.debugPrint();
        if (!ui->dictsTabsContainerWidget->createDictTabInitial(dictAttrs)) {
            qDebug() << "cannot create dictionary tab";
        }

    }
}

void Dialeqt::on_openDictMenuButton_triggered()
{
  QStringList dictfilenames = QFileDialog::getOpenFileNames(this, tr("Открыть словарь"), "", tr("Файлы словарей (*.sqlite)"));
  qDebug() << dictfilenames;
  foreach (QString dictfilename, dictfilenames) {
      if (dictfilename.size() > 0) {
        DictGlobalAttributes dictAttrs;
        if (!ui->dictsTabsContainerWidget->openDictTabInitial(dictAttrs, dictfilename)) {
            qDebug() << "cannot create dictionary tab";
          }
      }
    }
}

void Dialeqt::on_searchButton_triggered()
{
  QWidget *wdgt = this->findChild<QWidget *>("dictsTabsContainerWidget");

  if (wdgt != NULL) {
      qDebug() << "found tab container; resuming";
      DictTabsContainer *tabcontainer = static_cast<DictTabsContainer *> (wdgt);
      if (tabcontainer->getDictsOpened()->size() == 0) {
          errorMsg("Для того, чтобы воспользоваться поиском, необходимо, чтобы был открыт хотя бы один словарь.");
          return;
        }
      searchtabscontainer->removeTab(0);
      Search * searchTab = new Search(tabcontainer->getDictsOpened(), this);
      searchTab->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
      searchtabscontainer->insertTab(0, searchTab, "Поиск");
      searchtabscontainer->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

      searchWindow->open();
      return;
    }
  errorMsg("Виджета с табами не существует: так не должно быть, напишите разработчику");

}
