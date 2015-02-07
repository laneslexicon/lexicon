class NoteMaster;
class LanesLexicon;
class Lexicon;
extern LanesLexicon * getApp();
extern NoteMaster * getNotes();
extern void statusMessage(const QString &);
extern void viewLogsMessage();
extern void testfocus();
extern Lexicon * getLexicon();

#define SETTINGS   QSettings settings(getLexicon()->settingsFileName(),QSettings::IniFormat); \
  settings.setIniCodec("UTF-8");
