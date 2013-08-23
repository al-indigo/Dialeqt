#ifndef DICTENTRY_H
#define DICTENTRY_H

#include <QObject>

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
  QByteArray praatCompressed; // struct is the following: [int64 points to termination of markup][compressed_markup][compressed_sound]

public:
  explicit DictEntry(QObject *parent = 0,
                     QString _word = "",
                     QString _transcription = "",
                     QString _translation = "",
                     QString _soundFilename = "",
                     QString _praatFilename = "");

  inline DictEntry(const DictEntry &entry) {
    word              = entry.word;
    transcription     = entry.transcription;
    translation       = entry.translation;
    soundFilename     = entry.soundFilename;
    praatFilenameMarkup   = entry.praatFilenameMarkup;
    praatFilenameSound  = entry.praatFilenameSound;
    soundCompressed = entry.soundCompressed;
    praatCompressed = entry.praatCompressed;
  }

  QString getWord(void) const { return word; }
  QString getTranscription(void) const { return transcription; }
  QString getTranslation(void) const { return translation; }


signals:
  
public slots:
  
};

#endif // DICTENTRY_H
