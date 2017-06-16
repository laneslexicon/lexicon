class NoteMaster;
class LanesLexicon;
class Lexicon;
#include "lanesupport.h"
extern LanesLexicon * getApp();
extern NoteMaster * getNotes();
extern void statusMessage(const QString &);
extern void viewLogsMessage();
extern void testfocus();
extern Lexicon * getLexicon();
extern LaneSupport * getSupport();
#ifdef LANE
#define SETTINGS   QSettings settings(getLexicon()->settingsFileName(),QSettings::IniFormat); \
  settings.setIniCodec("UTF-8");
#else
#define SETTINGS   QSettings settings(getSupport()->settingsFileName(),QSettings::IniFormat); \
  settings.setIniCodec("UTF-8");
#endif
