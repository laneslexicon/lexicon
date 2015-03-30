#include "iconoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
/**
 *
 * @param theme
 * @param parent
 */
IconOptions::IconOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Icons";


  QVBoxLayout * vlayout = new QVBoxLayout;

  QFormLayout * layout = new QFormLayout;


  m_directory = new QLineEdit;
  QPushButton * directorybtn = new QPushButton(tr("Select directory"));
  QHBoxLayout * directorylayout = new QHBoxLayout;
  connect(directorybtn,SIGNAL(clicked()),this,SLOT(onSetDirectory()));
  directorybtn->setProperty("SID",SID_ICON_DIRECTORY);
  m_directory->setObjectName(SID_ICON_DIRECTORY);
  directorylayout->addWidget(m_directory);
  directorylayout->addWidget(directorybtn);

  layout->addRow(tr("Image directory"),directorylayout);


  QHBoxLayout * hlayout = new QHBoxLayout;
  QFormLayout * leftlayout = new QFormLayout;

  m_about = new QLineEdit;
  leftlayout->addRow(tr("About"),addLine(m_about,SID_ICON_ABOUT));
  m_back = new QLineEdit;
  leftlayout->addRow(tr("Back"),addLine(m_back,SID_ICON_BACK));
  m_bookmarks = new QLineEdit;
  leftlayout->addRow(tr("Bookmarks"),addLine(m_bookmarks,SID_ICON_BOOKMARKS));
  m_clear = new QLineEdit;
  leftlayout->addRow(tr("Clear"),addLine(m_clear,SID_ICON_CLEAR));
  m_docs = new QLineEdit;
  leftlayout->addRow(tr("Docs"),addLine(m_docs,SID_ICON_DOCS));
  m_exit = new QLineEdit;
  leftlayout->addRow(tr("Exit"),addLine(m_exit,SID_ICON_EXIT));
  m_first = new QLineEdit;
  leftlayout->addRow(tr("Clear"),addLine(m_clear,SID_ICON_CLEAR));
  m_history = new QLineEdit;
  leftlayout->addRow(tr("Clear"),addLine(m_clear,SID_ICON_CLEAR));
  m_insertLink = new QLineEdit;
  leftlayout->addRow(tr("Inser tLink"),addLine(m_insertLink,SID_ICON_INSERT_LINK));
  m_keymaps = new QLineEdit;
  leftlayout->addRow(tr("Keymaps"),addLine(m_keymaps,SID_ICON_KEYMAPS));
  m_keymapsDisabled = new QLineEdit;
  leftlayout->addRow(tr("Keymaps disabled"),addLine(m_keymapsDisabled,SID_ICON_KEYMAPS_DISABLED));
  m_last = new QLineEdit;
  leftlayout->addRow(tr("Last"),addLine(m_last,SID_ICON_LAST));
  m_link = new QLineEdit;
  leftlayout->addRow(tr("Link"),addLine(m_link,SID_ICON_LINK));


  m_localSearch = new QLineEdit;
  leftlayout->addRow(tr("Local search"),addLine(m_localSearch,SID_ICON_LOCAL_SEARCH));
  m_localSearchNext = new QLineEdit;
  leftlayout->addRow(tr("Local search next"),addLine(m_localSearchNext,SID_ICON_LOCAL_SEARCH_NEXT));


  QFormLayout * rightlayout = new QFormLayout;
  m_logs = new QLineEdit;
  rightlayout->addRow(tr("Logs"),addLine(m_logs,SID_ICON_LOGS));
  m_narrow = new QLineEdit;
  rightlayout->addRow(tr("Narrow"),addLine(m_narrow,SID_ICON_NARROW));
  m_next = new QLineEdit;
  rightlayout->addRow(tr("Next"),addLine(m_next,SID_ICON_NEXT));
  m_notes = new QLineEdit;
  rightlayout->addRow(tr("Notes"),addLine(m_notes,SID_ICON_NOTES));
  m_preferences = new QLineEdit;
  rightlayout->addRow(tr("Preferences"),addLine(m_preferences,SID_ICON_PREFERENCES));
  m_print = new QLineEdit;
  rightlayout->addRow(tr("Print"),addLine(m_print,SID_ICON_PRINT));
  m_search = new QLineEdit;
  rightlayout->addRow(tr("Search"),addLine(m_search,SID_ICON_SEARCH));
  m_syncLeft = new QLineEdit;
  rightlayout->addRow(tr("Sync left"),addLine(m_syncLeft,SID_ICON_SYNC_LEFT));
  m_syncRight = new QLineEdit;
  rightlayout->addRow(tr("Sync right"),addLine(m_syncRight,SID_ICON_SYNC_RIGHT));
  m_unlink = new QLineEdit;
  rightlayout->addRow(tr("Unlink"),addLine(m_unlink,SID_ICON_UNLINK));
  m_widen = new QLineEdit;
  rightlayout->addRow(tr("Widen"),addLine(m_widen,SID_ICON_WIDEN));
  m_zoom = new QLineEdit;
  rightlayout->addRow(tr("Zoom"),addLine(m_zoom,SID_ICON_ZOOM));
  m_zoomIn = new QLineEdit;
  rightlayout->addRow(tr("Zoom in"),addLine(m_zoomIn,SID_ICON_ZOOM_IN));
  m_zoomOut = new QLineEdit;
  rightlayout->addRow(tr("Zoom out"),addLine(m_zoomOut,SID_ICON_ZOOM_OUT));


  hlayout->addLayout(leftlayout);
  hlayout->addLayout(rightlayout);

  vlayout->addLayout(layout);
  vlayout->addLayout(hlayout);
  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();

}
QHBoxLayout * IconOptions::addLine(QLineEdit * edit,const QString & sid) {
  QPushButton * btn = new QPushButton(tr("Select"));
  QHBoxLayout * layout = new QHBoxLayout;
  connect(btn,SIGNAL(clicked()),this,SLOT(onSetFile()));
  QPushButton * icon = new QPushButton;
  btn->setProperty("SID",sid);
  icon->setObjectName(sid);
  edit->setObjectName(sid);
  layout->addWidget(edit);
  layout->addWidget(icon);
  layout->addWidget(btn);
  return layout;
}
void IconOptions::readSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");



  settings.beginGroup("Resources");
  m_directory->setText(settings.value(SID_RESOURCES_IMAGES,"images").toString());
  settings.endGroup();

  settings.beginGroup(m_section);

  m_about->setText(settings.value(SID_ICON_ABOUT,QString()).toString());
  setIconFromField(m_about,SID_ICON_ABOUT);
  m_back->setText(settings.value(SID_ICON_BACK,QString()).toString());


  m_dirty = false;
}
void IconOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  qDebug() << Q_FUNC_INFO << f;

  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup("Resources");
  settings.setValue(SID_RESOURCES_IMAGES,m_directory->text());
  settings.endGroup();


  settings.beginGroup(m_section);

  settings.setValue(SID_ICON_ABOUT,m_about->text());
  settings.setValue(SID_ICON_BACK,m_back->text());

  m_dirty = false;
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool IconOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup("Resources");
  if (compare(&settings,SID_RESOURCES_IMAGES,m_directory)) {
    m_dirty = true;
  }
  settings.endGroup();
  settings.beginGroup(m_section);


  if (compare(&settings,SID_ICON_ABOUT,m_about)) {
    m_dirty = true;
  }
  return m_dirty;
}
void IconOptions::onSetFont() {
  return;
}
void IconOptions::onSetColor() {
}
void IconOptions::onSetDirectory() {
  QString d = m_directory->text();
  QString p = getLexicon()->getResourceFilePath(Lexicon::Image);
  QString imagedirectory = p;
  if (! d.isEmpty()) {
    QFileInfo fi(p,d);
    if (fi.isDir()) {
      imagedirectory = fi.absoluteFilePath();
    }
  }
  QString fileName = QFileDialog::getExistingDirectory(this,
                        tr("Open Image Directory"),imagedirectory);


}
void IconOptions::onSetFile() {
  QPushButton * btn = qobject_cast<QPushButton *>(sender());
  QString sid =  btn->property("SID").toString();
  QLineEdit * edit = this->findChild<QLineEdit *>(sid);
  if (! edit ) {
    return;
  }
  QString d = m_directory->text();
  QString p = getLexicon()->getResourceFilePath(Lexicon::Image);
  QString imagedirectory = p;
  if (! d.isEmpty()) {
    QFileInfo fi(p,d);
    if (fi.isDir()) {
      imagedirectory = fi.absoluteFilePath();
    }
  }
  QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Image"),imagedirectory , tr("Image Files (*.*)"));


  if (! fileName.isEmpty()) {
    if (fileName.startsWith(imagedirectory)) {
      QString s = QString("%1%2").arg(imagedirectory).arg(QDir::separator());;
      edit->setText(fileName.remove(s));
    }
    else {
      edit->setText(fileName);
    }
  }
 setIconFromField(edit,sid);
}
void IconOptions::onSetIcon(const QString & sid,const QString & fileName) {
  qDebug() << Q_FUNC_INFO << sid << fileName;
  QPushButton * btn = this->findChild<QPushButton *>(sid);
  if (btn) {
    btn->setIcon(QIcon(fileName));
  }
  else {
    qDebug() << "Cannot find icon button";
  }
}
void IconOptions::setIconFromField(const QLineEdit * edit, const QString & sid) {
  qDebug() << Q_FUNC_INFO;
  qDebug() << "themeroot" << getLexicon()->getResourceFilePath(Lexicon::ThemeRoot);
  qDebug() << "Image root" << getLexicon()->getResourceFilePath(Lexicon::Image);
  qDebug() << "Theme Directory" << getLexicon()->themeDirectory().absolutePath();
  qDebug() << "Image setting" << m_directory->text();

  QDir themeDirectory = getLexicon()->themeDirectory();
  QDir imagedirectory;


  /// if the image directory is absolute then just use it
  /// otherwise it is a subdirectory of the theme root
  QFileInfo im(m_directory->text());
  if (im.isRelative()) {
    if (themeDirectory.cd(m_directory->text())) {
      /// good to go
      imagedirectory = themeDirectory;
      qDebug() << "cd down to" << themeDirectory.absolutePath();
    }
    else {
      qDebug() << "Could not cd to" << m_directory->text();
    }
  }
  else {
    imagedirectory = im.dir();
  }
  qDebug() << ">>>>>" << imagedirectory.absolutePath();
  QString name = edit->text();
  QFileInfo f(name);
  if (f.isRelative()) {
    qDebug() << "Using relative path";
    QFileInfo fi(imagedirectory,name);
    name = fi.absoluteFilePath();
  }
  onSetIcon(sid,name);
}
