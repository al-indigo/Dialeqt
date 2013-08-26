#include "dictentry.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTime>

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
          compressedResult = qCompress(file.readAll());
          qDebug() << "Compression took " << timer.elapsed()/1000;
          qDebug() << "Filesize after compression: " << compressedResult.size();
          file.close();
      }
    }
}

DictEntry::DictEntry(QObject *parent,
                     QString _word,
                     QString _transcription,
                     QString _translation,
                     QString _soundFilename,
                     QString _soundDescription,
                     QString _praatFilenameMarkup,
                     QString _praatFilenameSound,
                     QString _praatDescription) :
  QObject(parent),
  word(_word),
  transcription(_transcription),
  translation(_translation),
  soundFilename(_soundFilename),
  soundDescription(_soundDescription),
  praatFilenameMarkup(_praatFilenameMarkup),
  praatFilenameSound(_praatFilenameSound),
  praatDescription(_praatDescription)
{
  // here we need to read files and compress them to push them into blobs
  QFileInfo soundFileInfo(soundFilename);
  QFileInfo praatMarkupFileInfo(praatFilenameMarkup);
  QFileInfo praatSoundFileInfo(praatFilenameSound);

  if (soundFileInfo.exists()) {
      readAndCompress(soundFilename, soundCompressed);
      qDebug() << soundCompressed.size();
  }

  if (praatMarkupFileInfo.exists() && praatSoundFileInfo.exists()) {
      readAndCompress(praatFilenameMarkup, praatMarkupCompressed);
      readAndCompress(praatFilenameSound, praatSoundCompressed);
  }

  return;
}
