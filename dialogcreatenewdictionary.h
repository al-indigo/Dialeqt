#ifndef DIALOGCREATENEWDICTIONARY_H
#define DIALOGCREATENEWDICTIONARY_H

#include <QDialog>
#include <QSet>

#include "dictglobalattributes.h"

namespace Ui {
class DialogCreateNewDictionary;
}

class DialogCreateNewDictionary : public QDialog
{
    Q_OBJECT
    QString filename;
    QString dictname;
    QString author;
    QSet <QString> credits;
    QSet <QString> tags;
    QString description;
public:
    explicit DialogCreateNewDictionary(QWidget *parent = 0);
    ~DialogCreateNewDictionary();
    DictGlobalAttributes getData(void);
public slots:

    bool validateData(void);

protected:

    
private:
    Ui::DialogCreateNewDictionary *ui;
};

#endif // DIALOGCREATENEWDICTIONARY_H
