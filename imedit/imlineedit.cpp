#include "scripts.h"
#include "inputmapper.h"
#include "imlineedit.h"
ImLineEdit::ImLineEdit(QWidget * parent)
  : QLineEdit(parent)
{
  m_mapper = im_new();
  m_prev_char = 0;
  m_debug = false;
  m_forceLTR = false;
  m_keymapsEnabled = true;
  this->setText("كتب");
  connect(this,SIGNAL(textChanged(const QString &)),this,SLOT(onTextChanged(const QString &)));
  //this->setText("abcd");
}
ImLineEdit::~ImLineEdit() {
  im_free(m_mapper);
}
QStringList ImLineEdit::getMaps() const {
  return m_mapper->getMaps();
}
QString ImLineEdit::getActiveMap() const {
return m_activeMap;
}
QString ImLineEdit::getNullMap() const {
  return m_nullMap;
}
bool ImLineEdit::loadMap(const QString & filename,const QString & mapname) {
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
void ImLineEdit::activateMap(const QString & name,bool activate) {
  qDebug() << Q_FUNC_INFO << name << activate;
  if (! activate ) {
    m_activeMap.clear();
    return;
  }

  if (m_mapper->hasMap(name) && activate) {
    m_activeMap = name;
    return;
  }
  if (! m_mapper->hasMap(name)) {
    m_activeMap.clear();
  }
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
 * NB this deletes settings
 *
 * @param settings
 */
void ImLineEdit::readSettings(QSettings * settings) {
  if (! settings ) {
    settings = new QSettings;
  }
  settings->beginGroup("System");
  m_nullMap = settings->value("Null map","Native").toString();
  m_keymapsEnabled = settings->value("Keymaps",false).toBool();
  settings->endGroup();

  settings->beginGroup("Maps");
  m_debug = settings->value("Debug",false).toBool();
  QStringList groups = settings->childGroups();
  for(int i=0;i < groups.size();i++) {
    settings->beginGroup(groups[i]);
    QString file = settings->value("file",QString()).toString();
    if (! file.isEmpty()) {
      loadMap(file,groups[i]);
    }
    QString scut = settings->value("shortcut",QString()).toString();
    if (! scut.isEmpty()) {
      QKeySequence k = QKeySequence(scut);
      QShortcut * sc = new QShortcut(k,this);
      sc->setProperty("keymap", groups[i]);
      sc->setContext(Qt::WidgetShortcut);
      connect(sc,SIGNAL(activated()),this,SLOT(shortcutActivated()));
    }
    bool  enabled = settings->value("enabled",false).toBool();
    if (enabled) {
      m_activeMap = groups[i];
    }
    settings->endGroup();
  }

  delete settings;
}
void ImLineEdit::setEnabled(bool v) {
  m_keymapsEnabled = v;
}
void ImLineEdit::keyPressEvent(QKeyEvent * event) {
  ushort pc;
  if (event->modifiers() & Qt::ControlModifier) {
    return QLineEdit::keyPressEvent(event);
  }
  if (!m_keymapsEnabled )
    return QLineEdit::keyPressEvent(event);


  if (m_debug) {
    QString t;
    QTextStream out(&t);
    out << "ImLineEdit in: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << event->key() << " " << UcdScripts::getScript(event->key());
    out.reset();
    out << " " << event->text();
    qDebug() << t;
  }
  if ( m_debug && m_activeMap.isEmpty()) {
    qDebug() << Q_FUNC_INFO << "keymaps enabled" << m_keymapsEnabled << "no active map";
    return QLineEdit::keyPressEvent(event);
  }
  if (m_debug)
    qDebug() << this->text() << this->text().size() << this->cursorPosition();

  if ((m_forceLTR) && (this->cursorPosition() == 1)) {
    if (UcdScripts::getScript(event->key()) == "Unknown") {
      event->ignore();
      this->setCursorPosition(1);
      return;
    }
  }
  //event->text().toUtf8().data());
  const QChar * uc = event->text().unicode();
  pc = uc->unicode();
  if (pc == 0) {
    if (m_debug) {
      /// TODO error handling
    }
    return QLineEdit::keyPressEvent(event);
  }
  im_char * cc = im_convert(this->m_mapper,this->m_activeMap.toUtf8().constData(),uc->unicode(),m_prev_char);
  if (cc->processed) {
    event->ignore();
    QKeyEvent * nevent = new QKeyEvent(QEvent::KeyPress, cc->uc, Qt::NoModifier,cc->c);
    m_prev_char = cc->uc;
    QLineEdit::keyPressEvent(nevent);
    if (m_debug) {
      QString t;
      QTextStream out(&t);
      out << "ImLineEdit out: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << nevent->key();
      out.reset();
      out << nevent->text();
      qDebug() << t;
    }
    //     QApplication::postEvent(event->target, nevent);
    return;
  }
  else {
    m_prev_char = pc;
    QLineEdit::keyPressEvent(event);
  }
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
