#include "paradigmwindow.h"
#include "ui_paradigmwindow.h"

ParadigmWindow::ParadigmWindow(DictGlobalAttributes _dictAttrs, TabContents *parent) :
  TabContents(_dictAttrs, parent)
{
}

ParadigmWindow::~ParadigmWindow()
{
  delete ui;
}
