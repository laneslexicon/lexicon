#include "imedit.h"
#include "inputmapper.h"
#include "scripts.h"
ImEdit::ImEdit(QWidget * parent)
  : QTextEdit(parent)
{
  m_debug = false;
  mapper = im_new();
  mapEnabled = true;
  prev_char = 0;
  m_enabled = true;
}
ImEdit::~ImEdit() {
  im_free(mapper);
}
void ImEdit::getMapNames(QStringList & m) {
      mapper->getMapNames(m);
}
QString ImEdit::currentScript() {
  if ( ! mapEnabled ) {
    return QString();
  }
  return mapper->getScript(m_activeMap);
}
QString ImEdit::convertString(const QString & str) {
  bool ok;
  // QString t = im_convert_string(this->mapper,this->mapname,str,&ok);
  /// TODO why is buckwalter hard-coded here?
  QString t = im_convert_string(this->mapper,"Buckwalter",str,&ok);
  if (ok) {
    return t;
  }
  else {
    qDebug() << "error converting string";
    return QString();
  }
}
/**
 *
 *
 * @param fileName
 * @param mapname optional, override mapname in file
 *
 * @return
 */
bool ImEdit::loadMap(const QString & fileName,const QString & mapname) {
  QFile f(fileName);
  if (!f.open(QIODevice::ReadOnly)) {
    emit(logMessage(QString("Error loading file %1: %2 ").arg(fileName).arg(f.errorString())));
    return false;
  }
  if (mapname.isEmpty()) {
    im_load_map_from_json(mapper,fileName.toUtf8().constData(),0);
  }
  else {
    im_load_map_from_json(mapper,fileName.toUtf8().constData(),mapname.toUtf8().constData());
  }
  return true;
}
void ImEdit::activateMap(const QString & name,bool activate) {
  if (! activate ) {
    m_activeMap.clear();
    return;
  }

  if (mapper->hasMap(name) && activate) {
    m_activeMap = name;
    return;
  }
  if (! mapper->hasMap(name)) {
    m_activeMap.clear();
  }
}
void ImEdit::setEnabled(bool v) {
  m_enabled = v;
}
void ImEdit::keyPressEvent(QKeyEvent * event) {
  ushort pc;
  //  qDebug() << "keypress" << mapEnabled << mapname;;
  if (!m_enabled )
    return QTextEdit::keyPressEvent(event);

  if (event->modifiers() & Qt::ControlModifier) {
    return QTextEdit::keyPressEvent(event);
  }
  if (m_debug) {
    QString t;
    QTextStream out(&t);
    out << "ImEdit in: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << event->key() << " " << UcdScripts::getScript(event->key());
    out.reset();
    out << " " << event->text();
    emit(logMessage(t)); //QString("ImEdit got: %1 %2").arg(event->key()).arg(qPrintable(event->text()))));
  }
  if ((! mapEnabled) ||   m_activeMap.isEmpty() ||  m_activeMap.isNull())   {
    return QTextEdit::keyPressEvent(event);
  }

  //event->text().toUtf8().data());
  const QChar * uc = event->text().unicode();
  pc = uc->unicode();
  if (pc == 0) {
    if (m_debug) {
      emit(logMessage("ImEdit unicode value 0 exit"));
    }
    return QTextEdit::keyPressEvent(event);
  }
  im_char * cc = im_convert(this->mapper,this->m_activeMap.toUtf8().constData(),uc->unicode(),prev_char);
  if (cc->processed) {
    event->ignore();
    QKeyEvent * nevent = new QKeyEvent(QEvent::KeyPress, cc->uc, Qt::NoModifier,cc->c);
    prev_char = cc->uc;
    if (cc->consume) {
      this->textCursor().deletePreviousChar();
    }
    QTextEdit::keyPressEvent(nevent);
    if (m_debug) {
      QString t;
      QTextStream out(&t);
      out << "ImEdit out: 0x" << qSetFieldWidth(4) << qSetPadChar(QChar('0')) << hex << nevent->key();
      out.reset();
      out << nevent->text();
      emit(logMessage(t)); //QString("ImEdit out: %1 %2").arg(nevent->key()).arg(qPrintable(nevent->text()))));
    }
    //     QApplication::postEvent(event->target, nevent);
    return;
  }
  else {
    prev_char = pc;
    emit(logMessage("character not processed"));
    QTextEdit::keyPressEvent(event);
  }
}
void ImEdit::setMapname(const QString & name) {
  //  qDebug() << "setting map to:" << name;
  m_activeMap = name;
  bool v = true;
  if (name == "-none-") {
    v = false;
  }
  enableMapping(v);
  qDebug() << Q_FUNC_INFO << "m_activeMap" << name << v << mapper->getScript(name);
  emit(mapChanged(name));
  if (v) {
    QString s = mapper->getScript(name);
    emit(currentScript(s));
  }
}
void ImEdit::enableMapping(bool v) {
  mapEnabled = v;
}
/**
 * changes map via context menu
 *
 */
void ImEdit::actionChangeMap() {
  qDebug() << Q_FUNC_INFO;
  QAction * action = (QAction *)QObject::sender();
  QString m = action->data().toString();
  setMapname(m);
}
/**
 * changes font via context menu
 *
 */
void ImEdit::actionSetFont() {

  bool ok;
  QFont font = QFontDialog::getFont(
                                    &ok, QFont("", 10), this);
  if (ok) {
    m_docFont = font;
    setFont(m_docFont);
    emit(fontChanged());
  } else {
    // the user canceled the dialog; font is set to the initial
    // value, in this case Helvetica [Cronyx], 10
  }


}
void ImEdit::setDocFont(const QFont & f) {
  m_docFont = f;
  setFont(m_docFont);
  emit(fontChanged());
}
void ImEdit::actionInsertUnicode() {
  /*
    QAction * action = (QAction *)QObject::sender();
    QString m = action->data().toString();
    if (! m_unicode.contains(m)) {
    qDebug() << "ERROR: insert undefined unicode entry" << m;
    return;
    }
    UnicodeEntry * u = m_unicode.value(m);
    // get the hex value and insert it
    qDebug() << "insert unicode" << u->m_name << u->m_value;
    //  QKeyEvent * nevent = new QKeyEvent(QEvent::KeyPress, cc->uc, Qt::NoModifier,cc->c);
    //  QTextEdit::keyPressEvent(nevent);
    QTextCursor cursor = textCursor();
    cursor.insertText(QString(QChar(u->m_value)));
    setTextCursor(cursor);
  */
}
void ImEdit::actionDeleteUnicode() {
  /*
    QAction * action = (QAction *)QObject::sender();
    qDebug() << "delete at" << m_hudPos << m_hudChar;
    QTextCursor cstart = cursorForPosition(QPoint(0,0));
    cstart.setPosition(m_hudPos);
    cstart.deleteChar();
  */
}
void ImEdit::dragEnterEvent(QDragEnterEvent * event) {
  event->acceptProposedAction();
}
void ImEdit::dropEvent(QDropEvent * event) {
  emit(logMessage("ImEdit drop event"));
}
void ImEdit::scrollContentsBy(int dx,int dy) {
  QTextEdit::scrollContentsBy(dx,dy);
  emit(scrollby());
}
void ImEdit::getScriptNames(QStringList & m) {
  QStringList s;
  mapper->getScripts(s);
  for(int i=0;i < s.size();i++) {
    QStringList  p = s[i].split("",QString::SkipEmptyParts);
    if ( p.size() > 0) {
      QString x = p[0].toUpper();
      p.removeFirst();
      m.append(x + p.join(""));
    }
  }
}
void ImEdit::focusOutEvent(QFocusEvent * event ) {
  qDebug() << Q_FUNC_INFO << event->reason();

  //  if (event->reason() == Qt::TabFocusReason) {
  //    emit tabPressed();
  //  }
  if (event->reason() != Qt::PopupFocusReason) {
    setCursorWidth(0);
    emit editingFinished();
  }
  QTextEdit::focusOutEvent(event);
}
void ImEdit::focusInEvent(QFocusEvent * event ) {
  setCursorWidth(1);
  QTextEdit::focusInEvent(event);
}
#ifdef WITH_WRAPPED_EDIT
/**
 *
 *
 * @param parent
 */

WrappedEdit::WrappedEdit(QWidget * parent) : QWidget(parent) {
  bool ok;
  m_currentMap = "-none-";
  m_edit = new ImEdit(this);
  m_edit->setDebug(true);
  m_edit->setTabChangesFocus(true);
  m_edit->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_edit,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));
  connect(m_edit,SIGNAL(editingFinished()),this,SIGNAL(editingFinished()));
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_edit);
  setLayout(layout);
  m_options.readSettings();
  MapSettings maps = m_options.getMapSettings();
  QStringList keys = maps.keys();
  for(int i=0;i < keys.size();i++) {
    ScriptMap * v = maps.value(keys[i]);
    // need to have two settings
    // (1) load map on startup
    // (2) shortcut enabled
    if (v->getEnabled()) {
      ok = m_edit->loadMap(v->getFileName(),keys[i]);
      if (ok) {
        //        emit(logMessage(QString("Loaded map %1").arg(keys[i])));
      }
      else {
        //        emit(logMessage(QString("Failed to loaded map %1").arg(keys[i])));
      }
      QKeySequence k = QKeySequence(v->getShortCut());
      QShortcut * sc = new QShortcut(k,m_edit);
      sc->setContext(Qt::WidgetShortcut);
      connect(sc,SIGNAL(activated()),this,SLOT(shortcutActivated()));
    }
  }
  connect(m_edit,SIGNAL(textChanged()),this,SIGNAL(dataChanged()));
}
WrappedEdit::~WrappedEdit() {
  qDebug() << Q_FUNC_INFO;
}
void WrappedEdit::toggleMap(const QString & map) {
  //  qDebug() << Q_FUNC_INFO << m_name << "current map" << m_currentMap;
  if (m_currentMap == "-none-") {
    m_currentMap = map;
  }
  else {
    m_currentMap = "-none-";
  }
  //  qDebug() << Q_FUNC_INFO << "setting map to" << m_currentMap;
  m_edit->setMapname(m_currentMap);
}
void WrappedEdit::shortcutActivated() {
  QShortcut * s = (QShortcut *)QObject::sender();

  QString map = m_options.isShortcut(s->key());
  //  qDebug() << Q_FUNC_INFO << s->key().toString() << "got map" << map;
  if (! map.isEmpty()) {
    toggleMap(map);
  }
}
void WrappedEdit::setScriptFont(const QString & script) {
  QString fontString = m_options.getFontString(script);
  if (!fontString.isEmpty()) {
    QFont f;
    f.fromString(fontString);
    m_edit->setDocFont(f);
  }
}
void WrappedEdit::actionChangeMap() {
  QAction * mapAction = (QAction *)QObject::sender();
  QString mapname = mapAction->data().toString();
  if (mapname == "None") {
    mapname = "-none-";
  }
  toggleMap(mapname);
}
void WrappedEdit::showContextMenu(const QPoint & pt) {
  qDebug() << Q_FUNC_INFO;
  QMenu * menu =  m_edit->createStandardContextMenu(pt);
  buildContextMenu(menu);
  menu->exec(mapToGlobal(pt));
  delete menu;
}
void WrappedEdit::buildContextMenu(QMenu * menu) {
  QAction * actionFont = new QAction("Set font",menu);
  menu->addAction(actionFont);
  connect(actionFont,SIGNAL(triggered()),this,SLOT(actionChangeFont()));
  MapSettings maps = m_options.getMapSettings();
  QStringList keys = maps.keys();
  QAction * section = menu->addSection("Language Maps");
  keys << "None";
  for(int i=0;i < keys.size();i++) {
    QAction * actionMap = new QAction(keys[i],menu);
    ScriptMap * map = maps.value(keys[i]);
    if (map) {
      actionMap->setShortcut(QKeySequence(map->getShortCut()));
    }
    actionMap->setData(keys[i]);

    connect(actionMap,SIGNAL(triggered()),this,SLOT(actionChangeMap()));
    menu->addAction(actionMap);
  }
}
void WrappedEdit::actionChangeFont() {

  bool ok;
  QFont font = QFontDialog::getFont(
                                    &ok, editor()->getDocFont(), this);
  if (ok) {
    //     m_docFont = font;
    m_docFontString = font.toString();
    m_edit->setDocFont(font);
    qDebug() << Q_FUNC_INFO << m_docFontString;
    emit(fontChanged());
  } else {
    // the user canceled the dialog; font is set to the initial
    // value, in this case Helvetica [Cronyx], 10
  }


}
QString WrappedEdit::getText() {
  QString t = this->editor()->toPlainText();
  if (! UcdScripts::isScript(t,"Arabic")) {
    t = m_edit->convertString(t);
  }
  return t;
}
#endif
#ifdef WITH_HUD
/**
 *
 *
 * HUD stuff, not used here
 *
 *
 */

HudEdit::HudEdit(QWidget * parent) : WrappedEdit(parent) {
  m_hudCharCount = 0;
  m_unicode = m_options.getUnicodeSettings();
}
void HudEdit::buildContextMenu(QMenu * menu) {
  WrappedEdit::buildContextMenu(menu);

  QStringList k = m_unicode.keys();
  if (k.size() > 0 ) {
    menu->addSeparator();
  }
  for(int i=0;i < k.size();i++) {
    UnicodeEntry * u = m_unicode.value(k[i]);
    QString t;
    if (! u->m_uname.isEmpty()) {
      t = u->m_uname;
    }
    if (! u->m_name.isEmpty()) {
      t = u->m_name;
    }
    QAction * action = new QAction(t,menu);
    action->setData(QVariant(k[i]));
    connect(action,SIGNAL(triggered()),m_edit,SLOT(actionInsertUnicode()));
    menu->addAction(action);
  }

  if (m_hudCharCount > 0) {
    qDebug() << "hudchar count" << m_hudCharCount;
    QAction * delAll = new QAction("Remove all HUD chars",menu);
    menu->addAction(delAll);
  }
  qDebug() << "hud char" << m_isHudChar << m_hudChar;
  if (m_isHudChar) {
    UnicodeEntry * u = m_unicode.value(QString(m_hudChar));
    if (u) {
      QString t;
      if (! u->m_uname.isEmpty()) {
        t = u->m_uname;
      }
      if (! u->m_name.isEmpty()) {
        t = u->m_name;
      }
      menu->addSeparator();
      QAction * delAction = new QAction("Delete " + t,menu);
      connect(delAction,SIGNAL(triggered()),m_edit,SLOT(actionDeleteUnicode()));

      menu->addAction(delAction);
    }
    else {
      qDebug() << "no unicode entry for" << m_hudChar;
    }
  }
}

HudEdit::~HudEdit() {
  qDebug() << Q_FUNC_INFO;
  /*
    QStringList k = m_unicode.keys();
    for(int i=0;i < k.size();i++) {
    UnicodeEntry * e = m_unicode.take(k[i]);
    delete e;
    }
  */
}
void HudEdit::setUnicode(const UniSettings & opts) {
  QStringList k = opts.keys();
  for(int i=0;i < k.size();i++) {
    UnicodeEntry * e = new UnicodeEntry(*opts.value(k[i]));
    m_unicode.insert(k[i],e);
  }
}
#endif
