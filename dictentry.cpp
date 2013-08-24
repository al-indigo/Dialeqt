#include "dictentry.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>

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
  QByteArray temp;

  if (soundFileInfo.exists()) {
      //read file and compress
      QFile soundFile(soundFilename);
      if (!soundFile.open(QIODevice::ReadOnly)) {
          qDebug() << "Can't read sound file: " << soundFilename;
      } else {
          soundCompressed = qCompress(soundFile.readAll(), 8);
      }

    }
  if (praatMarkupFileInfo.exists()) {
      //read file and compress
      if (praatSoundFileInfo.exists()) {
          //read file and compress
        } else {
          qDebug() << "Something's totally wrong, sound for praat markup disappeared";
        }
    }

  return;
}
