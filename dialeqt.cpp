#include "dialeqt.h"
#include "ui_dialeqt.h"
#include "dialogcreatenewdictionary.h"
#include "dictdbfactory.h"

Dialeqt::Dialeqt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dialeqt)
{
    ui->setupUi(this);
}

Dialeqt::~Dialeqt()
{
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
        if (!ui->dictsTabsContainerWidget->addDictTabAndCreate(dictAttrs)) {
            qDebug() << "cannot create dictionary tab";
          }

    }
}
