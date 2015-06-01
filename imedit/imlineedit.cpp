#include "scripts.h"
#include "inputmapper.h"
#include "imlineedit.h"
#define SID_MAPS_NULL_MAP_NAME "Null map name"
#define SID_MAPS_ENABLED  "Map enabled"
#define SID_MAPS_DEBUG    "Debug"
#ifdef LANE
#include "QsLog.h"
#else
#define QLOG_DEBUG() qDebug()
#endif
ImLineEdit::ImLineEdit(QWidget * parent)
  : QLineEdit(parent)
{
  setObjectName("arabicedit");
  m_mapper = im_new();
  m_prev_char = 0;
  m_debug = false;
  m_forceLTR = false;
  m_discard = false;
  m_enabled = true;
  //  this->setText("كتب");
  connect(this,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
  //this->setText("abcd");
}
ImLineEdit::~ImLineEdit() {
  im_free(m_mapper);
}
void ImLineEdit::enableMapping(bool v) {
  m_enabled = v;
}
void ImLineEdit::setDebug(bool v) {
  m_debug = v;
}
QStringList ImLineEdit::getMaps() const {
  return m_mapper->getMaps();
}
QString ImLineEdit::currentMap() const {
return m_activeMap;
}
QString ImLineEdit::getNullMap() const {
  return m_nullMap;
}
bool ImLineEdit::loadMap(const QString & filename,const QString & mapname) {
  QLOG_DEBUG() << Q_FUNC_INFO << filename << mapname;
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly)) {
    // TODO emit(logMessage(QString("Error loading file %1: %2 ").arg(fileName).arg(f.errorString())));
    return false;
  }
  if (mapname.isEmpty()) {
    im_load_map_from_json(m_mapper,filename.toUtf8().constData(),0);
  }
  else {
    im_load_map_from_json(m_mapper,filename.toUtf8().constData(),mapname.toUtf8().constData());
  }
  return true;
}
bool ImLineEdit::setCurrentMap(const QString & name) {
  if (! m_mapper->hasMap(name)) {
    return false;
  }
  if ((name == "-none-") || name.isEmpty()) {
    m_activeMap.clear();
  }
  else {
    m_activeMap = name;
  }

  QVariant v = im_get_property(this->m_mapper,name,"discard");
  if (! v.isValid()) {
    m_discard = false;
  }
  else {
    m_discard = v.toBool();
  }
  return true;
}
void ImLineEdit::shortcutActivated() {
  if (m_activeMap.isEmpty()) {
    QShortcut * s = (QShortcut *)QObject::sender();
    m_activeMap = s->property("keymap").toString();
  }
  else {
    m_activeMap.clear();
  }
}
/**
 * REMOVE THIS
 *
 * It no longer loads the maps, but only sets up the shortcut
 * @param settings
 */
void ImLineEdit::readSettings(const QString & fileName) {
  QSettings settings(fileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Maps");
  qDebug() << settings.childKeys();
  m_nullMap = settings.value(SID_MAPS_NULL_MAP_NAME,"Native").toString();
  m_enabled = settings.value(SID_MAPS_ENABLED,true).toBool();
  qDebug() << Q_FUNC_INFO << fileName << "Enabled" << m_enabled;
  //  m_debug = settings.value(SID_MAPS_DEBUG,false).toBool();
  QStringList groups = settings.childGroups();
  for(int i=0;i < groups.size();i++) {
    settings.beginGroup(groups[i]);
    //   QString file = settings.value("file",QString()).toString();
    //    if (! file.isEmpty()) {
    //      loadMap(file,groups[i]);
    //    }
    QString scut = settings.value("shortcut",QString()).toString();
    if (! scut.isEmpty()) {
      QKeySequence k = QKeySequence(scut);
      QShortcut * sc = new QShortcut(k,this);
      sc->setProperty("keymap", groups[i]);
      sc->setContext(Qt::WidgetShortcut);
      connect(sc,SIGNAL(activated()),this,SLOT(shortcutActivated()));
    }
    bool  enabled = settings.value("enabled",false).toBool();
    if (enabled) {
      m_activeMap = groups[i];
    }
    settings.endGroup();
  }
}
void ImLineEdit::keyPressEvent(QKeyEvent * event) {
  ushort pc;
  int pos = this->cursorPosition();
  if (!m_enabled ) {
    //    emit(charInserted(event->text().unicode()->unicode(),pos));
    return QLineEdit::keyPressEvent(event);
  }

  if (event->modifiers() & Qt::ControlModifier) {
    return QLineEdit::keyPressEvent(event);
  }
  if (m_debug) {
    QString t;
    QTextStream out(&t);
    out << "ImEdit in: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << event->key() << " " << UcdScripts::getScript(event->key());
    out.reset();
    out << "  " << event->text();
    emit(logMessage(t)); //QString("ImEdit got: %1 %2").arg(event->key()).arg(qPrintable(event->text()))));
  }

  if (m_activeMap.isEmpty() ||  m_activeMap.isNull())   {
    //    emit(charInserted(event->text().unicode()->unicode(),pos));
    return QLineEdit::keyPressEvent(event);
  }

  //event->text().toUtf8().data());
  const QChar * uc = event->text().unicode();
  pc = uc->unicode();
  if (pc == 0) {
    if (m_debug) {
      emit(logMessage("ImEdit unicode value 0 exit"));
    }
    return QLineEdit::keyPressEvent(event);
  }

  im_char * cc = im_convert(this->m_mapper,this->m_activeMap.toUtf8().constData(),uc->unicode(),m_prev_char);
  /**
   * when entering characters above xffff (e.g Ugaritic character 0x1038e
   * QChar cannot be used, so we go for wide character
   */

  if (cc->processed) {
    event->ignore();
    wchar_t wc(cc->uc);
    cc->c = QString::fromWCharArray(&wc,1);
    QKeyEvent * nevent = new QKeyEvent(QEvent::KeyPress, cc->uc, Qt::NoModifier,cc->c);
    m_prev_char = cc->uc;
    if (cc->consume) {
      this->cursorBackward(true,1);
    }
    ///   why was I originally using the event queue ?
    ///   for wide characters we have to insert them directly, the event queue route
    ///   doesn't work
        this->insert(cc->c);
    //    QLineEdit::keyPressEvent(nevent);
    if (m_debug) {
      QString t;
      QTextStream out(&t);
      out << "ImEdit out: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << nevent->key();
      out.reset();
      out << nevent->text();
      emit(logMessage(t)); //QString("ImEdit out: %1 %2").arg(nevent->key()).arg(qPrintable(nevent->text()))));
    }
    //     QApplication::postEvent(event->target, nevent);
    //    emit(charInserted(cc->uc,pos));
    return;
  }
  else {
    if (! m_discard ) {
    m_prev_char = pc;
    //    emit(charInserted(m_prev_char,pos));
    emit(logMessage("character not processed"));
    QLineEdit::keyPressEvent(event);
    }
  }
  /*
  ushort pc;
  QLOG_DEBUG() << Q_FUNC_INFO << QString("Debug:%1, Enabled : %2, Map : %3").arg(m_debug).arg(m_enabled).arg(m_activeMap);
  if (event->modifiers() & Qt::ControlModifier) {
    return QLineEdit::keyPressEvent(event);
  }
  if (!m_enabled )
    return QLineEdit::keyPressEvent(event);


  if (m_debug) {
    QString t;
    QTextStream out(&t);
    out << "ImLineEdit in: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << event->key() << " " << UcdScripts::getScript(event->key());
    out.reset();
    out << " " << event->text();
    QLOG_DEBUG() << t;
  }
  if ( m_debug && m_activeMap.isEmpty()) {
    QLOG_DEBUG() << Q_FUNC_INFO << "keymaps enabled" << m_keymapsEnabled << "no active map";
    return QLineEdit::keyPressEvent(event);
  }
  if (m_debug)
    QLOG_DEBUG() << this->text() << this->text().size() << this->cursorPosition();

  if ((m_forceLTR) && (this->cursorPosition() == 1)) {
    if (UcdScripts::getScript(event->key()) == "Unknown") {
      event->ignore();
      this->setCursorPosition(1);
      return;
    }
  }
  const QChar * uc = event->text().unicode();
  pc = uc->unicode();
  if (pc == 0) {
    if (m_debug) {

    }
    return QLineEdit::keyPressEvent(event);
  }
  im_char * cc = im_convert(this->m_mapper,this->m_activeMap.toUtf8().constData(),uc->unicode(),m_prev_char);
  if (cc->processed) {
    event->ignore();
    this->insert(cc->c);
    return;
    QKeyEvent * nevent = new QKeyEvent(QEvent::KeyPress, cc->uc, Qt::NoModifier,cc->c);
    m_prev_char = cc->uc;
    QLineEdit::keyPressEvent(nevent);
    if (m_debug) {
      QString t;
      QTextStream out(&t);
      out << "ImLineEdit out: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << nevent->key();
      out.reset();
      out << nevent->text();
      QLOG_DEBUG() << t;
    }
    return;
  }
  else {
    m_prev_char = pc;
    QLineEdit::keyPressEvent(event);
  }
  */
}
void ImLineEdit::setForceLTR(bool v) {
  QString t = this->text();
  QString ltr(QChar(0x202d));

  disconnect(this,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
  if (! v ) {
      this->setCursorPosition(this->text().size());
      this->setText(t.remove(ltr));
  }
  else {
    if (! t.startsWith(ltr)) {
      t.remove(ltr);
      this->setText(ltr + t);
    }
  }
  m_forceLTR = v;
  this->setFocus();
  this->setCursorPosition(this->text().size());
  connect(this,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
}
void ImLineEdit::onTextChanged(const QString & /* t */) {
  this->setForceLTR(m_forceLTR);
}
void ImLineEdit::focusInEvent(QFocusEvent * event ) {
  emit(gotFocus(event->reason()));
  QLineEdit::focusInEvent(event);
}
