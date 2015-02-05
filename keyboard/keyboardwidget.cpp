#include "keyboard.h"
#include "keyboardwidget.h"
/**
 *
 *
 * @param parent
 */
KeyboardWidget::KeyboardWidget(QWidget * parent) : QDialog(parent) {
  m_keyboardConfig = "keyboard.ini";
  m_keyboardDirectory = ".";
  this->setup();
}
KeyboardWidget::KeyboardWidget(const QString & d,const QString & config,QWidget * parent) : QDialog(parent) {
  qDebug() << Q_FUNC_INFO << d << config;
  m_keyboardConfig = config;
  m_keyboardDirectory = d;
  this->setup();
}
void KeyboardWidget::setup() {
  m_target = 0;
  setWindowTitle(tr("Virtual Keyboard"));
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_keyboards = new QComboBox(this);
  m_view = new KeyboardView(this);
  m_view->setStyleSheet("border-style : 2px solid black");

  loadDefinitions();
  m_transform = m_view->transform();
  m_scale = 1.0;

  layout->addWidget(m_keyboards);
  layout->addWidget(m_view);
  setLayout(layout);

  m_transform = m_view->transform();
  //  m_view->loadKeyboard("keyboards/arabic1.ini");
  //  this->autoScale();//  m_view->scale(0.5,0.5);
  /// TODO find default keyboard and load it

  connect(m_view,SIGNAL(virtualKeyPressed(int)),this,SLOT(virtualKeyPressed(int)));
  connect(m_keyboards,SIGNAL(currentIndexChanged(int)),this,SLOT(loadKeyboard(int)));

  if (! m_defaultKeyboard.isEmpty()) {
    int ix = m_keyboards->findText(m_defaultKeyboard);
    qDebug() << Q_FUNC_INFO << "loading default keyboard" << m_defaultKeyboard << ix;
    fopen("/tmp/flag-setCurrentIndex","r");
    m_keyboards->setCurrentIndex(ix);
  }
}
void KeyboardWidget::addShortcut(const QString & keys) {
  QAction * closeAction = new QAction(this);
  closeAction->setShortcut(QKeySequence(keys));
  this->addAction(closeAction);
  connect(closeAction,SIGNAL(triggered()),this,SLOT(onKeyboardShortcut()));
  //  emit(keyboardShortcut());
}
void KeyboardWidget::onKeyboardShortcut() {
  qDebug() << Q_FUNC_INFO;
  QAction * action = qobject_cast<QAction *>(sender());
  if (action) {
    qDebug() << Q_FUNC_INFO << action->shortcut().toString();
    emit(keyboardShortcut(action->shortcut().toString()));
    }
}
bool KeyboardWidget::isAttached() {
  return  (m_target != 0);
}
QSize KeyboardWidget::sizeHint() const {
  if (! m_currentSize.isEmpty())
    return m_currentSize;

  return QSize(800,300);
}
/**
 * Load all the <keyboar name>.ini files in the keyboard directory that support the
 * requested script
 *
 * @param targetScript
 */
void KeyboardWidget::loadDefinitions(const QString & targetScript) {
  bool ok;
  QDir d(m_keyboardDirectory); //QDir::current().absolutePath() + QDir::separator() + "keyboards");
  QStringList files = d.entryList(QDir::Files | QDir::Readable);
  for(int i=0;i < files.size();i++) {
    if (files[i].endsWith(".ini")) {
      if (files[i] != m_keyboardConfig) {
        QString name;
        QString file = d.absolutePath() + QDir::separator() + files[i];
        ok = false;
        QSettings settings(file,QSettings::IniFormat);
        qDebug() << Q_FUNC_INFO << settings.fileName();
        settings.setIniCodec("UTF-8");
        if (settings.childGroups().contains("Header")) {
          settings.beginGroup("Header");
          name = settings.value("name",QString()).toString();
          QString script = settings.value("script",QString()).toString();
          settings.endGroup();
          if ( ! name.isEmpty()) {
            if ( targetScript.isEmpty()) {
              ok = true;
            }
            else  if (script == targetScript) {
              ok = true;
            }
          }
          if (ok) {
            m_keyboards->addItem(name,file);
          }
        }
      }
    }
  }
}
void KeyboardWidget::loadKeyboard(int /*ix */) {
  QString name = m_keyboards->currentData().toString();
  qDebug() << Q_FUNC_INFO << name;
  m_view->loadKeyboard(name);
  this->autoScale();
}
void KeyboardWidget::resizeEvent(QResizeEvent * /* event */) {
  this->autoScale();
  //  m_currentSize = event->size();
  //  qDebug() << "current size" << m_currentSize;
}
void KeyboardWidget::closeEvent(QCloseEvent * /* event */) {
  this->hide();
  emit(closed());
}
/**
 *
 *
 */
void KeyboardWidget::autoScale() {
  if (m_keepAspectRatio)
    m_view->fitInView(m_view->sceneRect(), Qt::KeepAspectRatio);
  else
    m_view->fitInView(m_view->sceneRect(), Qt::IgnoreAspectRatio);
}
/*
void KeyboardWidget::zoomIn() {
  m_view->setTransform(m_transform);
  m_scale += .1;
  m_view->scale(m_scale,m_scale);
}
void KeyboardWidget::zoomOut() {
  m_view->setTransform(m_transform);
  m_scale -= .1;
  m_view->scale(m_scale,m_scale);
}
*/
void KeyboardWidget::showKeyboard() {
  this->show();
  //  this->raise();
  this->activateWindow();
}
/**
 * successive calls to attach(QWidget *) with the same widget
 * attach and then detach (== hide)
 *
 * @param w
 */
void KeyboardWidget::attach(QWidget * w) {
  m_target = w;
  showKeyboard();
  emit(attached());
  /*
  if (m_target == w) {
    m_target = 0;
    this->hide();
    this->lower();
  }
  else {
    m_target = w;
    setFocusProxy(m_target);
    showKeyboard();
  }
  */
}
void KeyboardWidget::detach() {
  m_target = 0;
  this->hide();
  this->lower();
  emit(detached());
}
void KeyboardWidget::virtualKeyPressed(int k) {
  //  qDebug() << Q_FUNC_INFO << k << QChar(k);
  if (m_target == 0) {
    return;
  }
  QKeyEvent * event;
  event = new QKeyEvent(QEvent::KeyPress, k, Qt::NoModifier,QString(QChar(k)));
  QApplication::postEvent(m_target,event);
}
void KeyboardWidget::readSettings() {
  qDebug() << Q_FUNC_INFO << m_keyboardDirectory << m_keyboardConfig;
  QFileInfo fi(m_keyboardDirectory,m_keyboardConfig);
  QSettings settings(fi.absoluteFilePath(),QSettings::IniFormat);
  qDebug() << Q_FUNC_INFO << settings.fileName();
  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");
  m_defaultKeyboard = settings.value("Default",QString()).toString();
  m_keepAspectRatio = settings.value("Keep aspect ratio",false).toBool();
  settings.endGroup();
}
QWidget * KeyboardWidget::target() const {
  return m_target;
}
void KeyboardWidget::hideEvent(QHideEvent * event) {
  m_currentSize = this->size();
  m_currentPosition = this->pos();
  QWidget::hideEvent(event);
}
QPoint KeyboardWidget::currentPosition() const {
  return m_currentPosition;
}
