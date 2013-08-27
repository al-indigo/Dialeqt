#include "dictentry.h"
#include "utils.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTime>

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
