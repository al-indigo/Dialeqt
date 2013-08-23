#include "dictentry.h"

#include <QFile>

DictEntry::DictEntry(QObject *parent,
                     QString _word,
                     QString _transcription,
                     QString _translation,
                     QString _soundFilename,
                     QString _praatFilename) :
  QObject(parent),
  word(_word),
  transcription(_transcription),
  translation(_translation),
  soundFilename(_soundFilename)
{
}

