#ifndef DICTENTRY_H
#define DICTENTRY_H

#include <QObject>
#include <QFile>
#include <QPair>
class DictEntry : public QObject
{
  Q_OBJECT
  QString word;
  QString transcription;
  QString translation;
  QString soundFilename;
  QPair <QString, QString> praatPair;

public:
  explicit DictEntry(QObject *parent = 0,
                     QString _word = "",
                     QString _transcription = "",
                     QString _translation = "",
                     QString _soundFilename = "",
                     QString praatFirst = "",
                     QString praatSecond = "") : QObject(parent), word(_word), transcription(_transcription), translation(_translation), soundFilename(_soundFilename), praatPair(praatFirst,praatSecond)
                      {}

  inline DictEntry(const DictEntry &entry) {
    word              = entry.word;
    transcription     = entry.transcription;
    translation       = entry.translation;
    soundFilename     = entry.soundFilename;
    praatPair.first   = entry.praatPair.first;
    praatPair.second  = entry.praatPair.second;
  }

  QString getWord(void) const { return word; }
  QString getTranscription(void) const { return transcription; }
  QString getTranslation(void) const { return translation; }

  bool isEmpty(void) {
    return (word.isEmpty() && transcription.isEmpty() && translation.isEmpty() && soundFilename.isEmpty() && praatPair.first.isEmpty() && praatPair.second.isEmpty());
  }

signals:
  
public slots:
  
};

#endif // DICTENTRY_H
