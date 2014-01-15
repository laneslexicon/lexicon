#include "hermesoptions.h"
ScriptMap::ScriptMap(const QString & fileName) {
  m_fileName = fileName;
  m_enabled = true;
}
HermesOptions::~HermesOptions() {
  QStringList keys = m_maps.keys();
  for(int i=0;i < keys.size();i++) {
    ScriptMap * map = m_maps.take(keys[i]);
    delete map;
  }
  keys = m_fonts.keys();
  for(int i=0;i < keys.size();i++) {
    ScriptFont * f = m_fonts.take(keys[i]);
    delete f;
  }
  keys = m_unicode.keys();
  for(int i=0;i < keys.size();i++) {
    UnicodeEntry * f = m_unicode.take(keys[i]);
    delete f;
  }
}
void HermesOptions::addMap(const QString & name,ScriptMap * map) {
  m_maps.insert(name,map);
}
void HermesOptions::addFont(const QString & script,ScriptFont * f) {
  m_fonts.insert(script.toCaseFolded(),f);
}
void HermesOptions::addFlashFont(const QString & script,ScriptFont * f) {
  m_flashfonts.insert(script,f);
}
void HermesOptions::addTreeFont(const QString & script,ScriptFont * f) {
  m_treefonts.insert(script,f);
}
/**
 *
 *
 */
void HermesOptions::readSettings() {
  bool ok;
  QStringList keys;
  QStringList groupkeys;
  QSettings settings;

  settings.beginGroup("Maps");
  QStringList maps = settings.childGroups();
  for(int i=0;i < maps.size();i++) {
    QString mapname = maps[i];
    settings.beginGroup(mapname);
    QStringList keys = settings.childKeys();
    if (keys.contains("file")) {
      QString file = settings.value("file").toString();

      ScriptMap * map = new ScriptMap(file);
      if (keys.contains("enabled")) {
        map->setEnabled(settings.value("enabled").toBool());
      }
      if (keys.contains("script")) {
        map->setScript(settings.value("script").toString());
      }
      map->setFileName(file);
      if (keys.contains("shortcut")) {
        map->setShortCut(settings.value("shortcut").toString());
      }
      addMap(mapname,map);
      qDebug() << "add map" << mapname << map->getFileName() << map->getShortCut();
      settings.endGroup();
    }
    else {
      //      qDebug() << "map" << maps[i] << "has no filename";
    }
  }
  settings.endGroup();
  setupKeys();
  /*
  qDebug() << "Reading Scripts settings";
  settings.beginGroup("Scripts");
  QStringList scripts =  settings.childGroups();
  for (int i=0;i < scripts.size();i++) {
    QString script = scripts[i];
    settings.beginGroup(script);
    //    qDebug() << script << "group childkeys" << settings.childKeys();
    QStringList keys = settings.childKeys();
    if (keys.contains("font")) {
      ScriptFont * f = new ScriptFont;
      f->setFontString(settings.value("font").toString());
      if (keys.contains("enabled")) {
        f->setEnabled(settings.value("enabled").toBool());
      }
      addFont(script,f);
      //      qDebug() << "added font" << script << settings.value("font").toString();
    }
    settings.endGroup();
  }
  settings.endGroup();
  */
}
void HermesOptions::writeSettings() {
  QString org = "Gabanjo";
  QString app = "My Editor";
  QSettings settings(org,app);
  /*
  settings.beginGroup("Maps");
  QMapIterator<QString,QPair<QString,QString> > mit(m_maps);
  while(mit.hasNext()) {
    mit.next();
    settings.beginGroup(mit.key());
    QPair<QString,QString> v = mit.value();
    settings.setValue("script",v.first);
    settings.setValue("file",v.second);
    settings.endGroup();
  }
  settings.endGroup();
  */
  settings.beginGroup("Scripts");
  QMapIterator<QString,ScriptFont *> it(m_fonts);
  while(it.hasNext()) {
    it.next();
    qDebug() << it.key();
    settings.beginGroup(it.key());
    ScriptFont * s = it.value();
    settings.setValue("font",s->getFontString());
    settings.setValue("enabled",s->getEnabled());
    settings.endGroup();
  }
  settings.endGroup();
}
MapSettings HermesOptions::getMapSettings() {
  MapSettings maps;
  QStringList keys = m_maps.keys();
  for(int i=0;i < keys.size();i++) {
    ScriptMap * map = m_maps.value(keys[i]);
    //    QPair<QString,QString> v = qMakePair(map->getScript(),map->getFilename());
    ScriptMap * mm = new ScriptMap(*map);
    maps.insert(keys[i],mm);
  }
  return maps;
}
FontSettings HermesOptions::getFontSettings() {
  FontSettings fonts;
  QStringList keys = m_fonts.keys();
  for(int i=0;i < keys.size();i++) {
    ScriptFont * font = m_fonts.value(keys[i]);
    ScriptFont * ff = new ScriptFont(*font);
    fonts.insert(keys[i],ff);
  }
  return fonts;
}
void HermesOptions::setMaps(const MapSettings & m) {
}
void HermesOptions::setFonts(const FontSettings & m) {
  m_fonts = m;
}
QString HermesOptions::isShortcut(const QKeySequence & k) {
  QString v;
  QString ks = k.toString(QKeySequence::NativeText);
  if (m_keys.contains(ks)) {
    v = m_keys.value(ks);
  }
  return v;
}
void HermesOptions::setupKeys() {
  QStringList k = m_maps.keys();
  for(int i=0;i < k.size();i++) {
    ScriptMap * m = m_maps.value(k[i]);
    if (m->getEnabled()) {
      QString s = m->getShortCut();
      if (! s.isEmpty()) {
        m_keys.insert(s,k[i]);
      }
    }
  }
  qDebug() << "Known shortcuts" << m_keys.keys();
}
QString HermesOptions::getOption(const QString & k) {
  QString v;
  if (m_options.contains(k)) {
    v = m_options.value(k);
  }
  return v;
}
QString HermesOptions::getFontString(const QString & script) {
  QString f;
  if (m_fonts.contains(script.toCaseFolded())) {
    ScriptFont * sf = m_fonts.value(script.toCaseFolded());
    f = sf->getFontString();
  }
  return f;
}
QString HermesOptions::getFlashFontString(const QString & script) {
  QString f;
  if (m_flashfonts.contains(script)) {
    ScriptFont * sf = m_flashfonts.value(script);
    f = sf->getFontString();
  }
  return f;
}
QString HermesOptions::getTreeFontString(const QString & script) {
  QString f;
  if (m_treefonts.contains(script)) {
    ScriptFont * sf = m_treefonts.value(script);
    f = sf->getFontString();
    //    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << f;
  }
  return f;
}
