#ifndef DICTENTRY_H
#define DICTENTRY_H

#include <QObject>
#include <QFileInfo>

class DictEntry : public QObject
{
  Q_OBJECT
  QString word;
  QString transcription;
  QString translation;
  QString soundFilename;
  QString soundDescription;
  QString praatFilenameMarkup;
  QString praatFilenameSound;
  QString praatDescription;

  QByteArray soundCompressed;
  QByteArray praatMarkupCompressed;
  QByteArray praatSoundCompressed;

public:
  explicit DictEntry(QObject *parent = 0,
                     QString _word = "",
                     QString _transcription = "",
                     QString _translation = "",
                     QString _soundFilename = "",
                     QString _soundDescription = "",
                     QString _praatFilenameMarkup = "",
                     QString _praatFilenameSound = "",
                     QString _praatDescription = "");

  inline DictEntry(const DictEntry &entry) {
    word              = entry.word;
    transcription     = entry.transcription;
    translation       = entry.translation;
    soundFilename     = entry.soundFilename;
    soundDescription  = entry.soundDescription;
    praatFilenameMarkup   = entry.praatFilenameMarkup;
    praatFilenameSound  = entry.praatFilenameSound;
    praatDescription = entry.praatDescription;
    soundCompressed = entry.soundCompressed;
    praatMarkupCompressed = entry.praatMarkupCompressed;
    praatSoundCompressed = entry.praatSoundCompressed;
  }

  QString getWord(void) const { return word; }
  QString getTranscription(void) const { return transcription; }
  QString getTranslation(void) const { return translation; }

  QString getSoundFilename (void) const {
    QFileInfo fileInfo(this->soundFilename);
    return fileInfo.fileName();
  }

  QString getSoundDescription(void) const { return soundDescription; }

  QString getBasePraatSoundFilename (void) const {
    QFileInfo fileInfo(this->praatFilenameSound);
    return fileInfo.baseName();
  }

//it's redundunt func, just in case
  QString getBasePraatMarkupFilename (void) const {
    QFileInfo fileInfo(this->praatFilenameMarkup);
    return fileInfo.baseName();
  }

  QString getPraatDescription (void) const { return this->praatDescription; }

  QByteArray* getSoundPointer() { return &soundCompressed; }
  QByteArray* getPraatSoundPointer() { return &praatSoundCompressed; }
  QByteArray* getPraatMarkupPointer() { return &praatMarkupCompressed; }


signals:
  
public slots:
  
};

#endif // DICTENTRY_H
