#include "utils.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTime>
#include <QMessageBox>
#include <QByteArrayData>

void readAndCompress(const QString & filename, QByteArray &compressedResult) {
  QFileInfo info(filename);
  if (info.exists()) {
      //read file and compress
      QFile file(filename);

      if (!file.open(QIODevice::ReadOnly)) {
          qDebug() << "Can't read the file: " << filename;
      } else {
          qDebug() << "Filesize before compression: " << info.size();
          QTime timer;

          timer.start();
//          compressedResult = qCompress(file.readAll());
          compressedResult = file.readAll();
          qDebug() << "Compression took " << timer.elapsed()/1000;
          qDebug() << "Filesize after compression: " << compressedResult.size();
          file.close();
      }
    }
}

QString createDirectory(const QString & dictId, const QString & postfix) {
  QString temp = QString("tmp/") + dictId + "/";
  QDir tempdir;

  if(!tempdir.mkpath(temp + postfix)) {
    qDebug() << "can't create temp directory for " << postfix;
  }

  return tempdir.absoluteFilePath(temp + postfix);
}

bool extractSound(const QString& dest, const QString& name, const QByteArray& compressed) {
  QFileInfo info(dest + "/" + name);
  QFile file(dest + "/" + name);
  if (!file.open(QIODevice::WriteOnly)) {
      qDebug() << "Can't write sound file to disk";
      return false;
  } else {
//      if (!file.write(qUncompress(compressed))) {
      if (!file.write(compressed)) {
          qDebug() << "Can't write to sound file anymore";
          file.close();
          return false;
      } else {
          file.close();
          return true;
      }
  }
}

void errorMsg (const QString& error_msg){
  QMessageBox errormsg;
  errormsg.setText(error_msg);
  errormsg.setIcon(QMessageBox::Critical);
  errormsg.setDefaultButton(QMessageBox::Ok);
  errormsg.exec();
  return;
}
