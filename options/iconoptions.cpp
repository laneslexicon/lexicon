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
  leftlayout->addRow(tr("First"),addLine(m_first,SID_ICON_FIRST));
  m_history = new QLineEdit;
  leftlayout->addRow(tr("History"),addLine(m_history,SID_ICON_HISTORY));
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

  leftlayout->setVerticalSpacing(5);
  rightlayout->setVerticalSpacing(5);
  hlayout->addLayout(leftlayout);
  hlayout->addSpacing(30);
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
  QHBoxLayout * layout = new QHBoxLayout;
  QPushButton * icon = new QPushButton;
  connect(icon,SIGNAL(clicked()),this,SLOT(onSetFile()));
  icon->setProperty("SID",sid);
  icon->setObjectName(sid);
  edit->setObjectName(sid);
  layout->addWidget(edit);
  layout->addWidget(icon);
  return layout;
}
void IconOptions::readSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");



  settings.beginGroup("Resources");
  m_directory->setText(settings.value(SID_RESOURCES_IMAGES,"images").toString());
  settings.endGroup();

  settings.beginGroup(m_section);

  m_about->setText(settings.value(SID_ICON_ABOUT,QString()).toString());
  setIconFromField(m_about,SID_ICON_ABOUT);

  m_back->setText(settings.value(SID_ICON_BACK,QString()).toString());
  setIconFromField(m_back,SID_ICON_BACK);
  m_bookmarks->setText(settings.value(SID_ICON_BOOKMARKS,QString()).toString());
  setIconFromField(m_bookmarks,SID_ICON_BOOKMARKS);
  m_clear->setText(settings.value(SID_ICON_CLEAR,QString()).toString());
  setIconFromField(m_clear,SID_ICON_CLEAR);
  m_docs->setText(settings.value(SID_ICON_DOCS,QString()).toString());
  setIconFromField(m_docs,SID_ICON_DOCS);
  m_exit->setText(settings.value(SID_ICON_EXIT,QString()).toString());
  setIconFromField(m_exit,SID_ICON_EXIT);
  m_first->setText(settings.value(SID_ICON_FIRST,QString()).toString());
  setIconFromField(m_first,SID_ICON_FIRST);
  m_history->setText(settings.value(SID_ICON_HISTORY,QString()).toString());
  setIconFromField(m_history,SID_ICON_HISTORY);

  m_insertLink->setText(settings.value(SID_ICON_INSERT_LINK,QString()).toString());
  setIconFromField(m_insertLink,SID_ICON_INSERT_LINK);

  m_keymaps->setText(settings.value(SID_ICON_KEYMAPS,QString()).toString());
  setIconFromField(m_keymaps,SID_ICON_KEYMAPS);

  m_keymapsDisabled->setText(settings.value(SID_ICON_KEYMAPS_DISABLED,QString()).toString());
  setIconFromField(m_keymapsDisabled,SID_ICON_KEYMAPS_DISABLED);

  m_last->setText(settings.value(SID_ICON_LAST,QString()).toString());
  setIconFromField(m_last,SID_ICON_LAST);
  m_link->setText(settings.value(SID_ICON_LINK,QString()).toString());
  setIconFromField(m_link,SID_ICON_LINK);

  m_localSearch->setText(settings.value(SID_ICON_LOCAL_SEARCH,QString()).toString());
  setIconFromField(m_localSearch,SID_ICON_LOCAL_SEARCH);
  m_localSearchNext->setText(settings.value(SID_ICON_LOCAL_SEARCH_NEXT,QString()).toString());
  setIconFromField(m_localSearchNext,SID_ICON_LOCAL_SEARCH_NEXT);
  m_logs->setText(settings.value(SID_ICON_LOGS,QString()).toString());
  setIconFromField(m_logs,SID_ICON_LOGS);
  m_narrow->setText(settings.value(SID_ICON_NARROW,QString()).toString());
  setIconFromField(m_narrow,SID_ICON_NARROW);
  m_next->setText(settings.value(SID_ICON_NEXT,QString()).toString());
  setIconFromField(m_next,SID_ICON_NEXT);
  m_preferences->setText(settings.value(SID_ICON_PREFERENCES,QString()).toString());
  setIconFromField(m_preferences,SID_ICON_PREFERENCES);
  m_print->setText(settings.value(SID_ICON_PRINT,QString()).toString());
  setIconFromField(m_print,SID_ICON_PRINT);
  m_search->setText(settings.value(SID_ICON_SEARCH,QString()).toString());
  setIconFromField(m_search,SID_ICON_SEARCH);
  m_syncLeft->setText(settings.value(SID_ICON_SYNC_LEFT,QString()).toString());
  setIconFromField(m_syncLeft,SID_ICON_SYNC_LEFT);
  m_syncRight->setText(settings.value(SID_ICON_SYNC_RIGHT,QString()).toString());
  setIconFromField(m_syncRight,SID_ICON_SYNC_RIGHT);
  m_unlink->setText(settings.value(SID_ICON_UNLINK,QString()).toString());
  setIconFromField(m_unlink,SID_ICON_UNLINK);
  m_widen->setText(settings.value(SID_ICON_WIDEN,QString()).toString());
  setIconFromField(m_widen,SID_ICON_WIDEN);
  m_zoom->setText(settings.value(SID_ICON_ZOOM,QString()).toString());
  setIconFromField(m_zoom,SID_ICON_ZOOM);
  m_zoomIn->setText(settings.value(SID_ICON_ZOOM_IN,QString()).toString());
  setIconFromField(m_zoomIn,SID_ICON_ZOOM_IN);
  m_zoomOut->setText(settings.value(SID_ICON_ZOOM_OUT,QString()).toString());
  setIconFromField(m_zoomOut,SID_ICON_ZOOM_OUT);


  m_dirty = false;
}
void IconOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }

  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  qDebug() << Q_FUNC_INFO << f;

  settings.beginGroup("Resources");
  settings.setValue(SID_RESOURCES_IMAGES,m_directory->text());
  settings.endGroup();

  settings.beginGroup(m_section);

  settings.setValue(SID_ICON_ABOUT,m_about->text());

  settings.setValue(SID_ICON_BACK,m_back->text());
  settings.setValue(SID_ICON_BOOKMARKS,m_bookmarks->text());
  settings.setValue(SID_ICON_CLEAR,m_clear->text());
  settings.setValue(SID_ICON_DOCS,m_docs->text());
  settings.setValue(SID_ICON_EXIT,m_exit->text());
  settings.setValue(SID_ICON_FIRST,m_first->text());
  settings.setValue(SID_ICON_HISTORY,m_history->text());

  settings.setValue(SID_ICON_INSERT_LINK,m_insertLink->text());

  settings.setValue(SID_ICON_KEYMAPS,m_keymaps->text());

  settings.setValue(SID_ICON_KEYMAPS_DISABLED,m_keymapsDisabled->text());

  settings.setValue(SID_ICON_LAST,m_last->text());
  settings.setValue(SID_ICON_LINK,m_link->text());

  settings.setValue(SID_ICON_LOCAL_SEARCH,m_localSearch->text());
  settings.setValue(SID_ICON_LOCAL_SEARCH_NEXT,m_localSearchNext->text());
  settings.setValue(SID_ICON_LOGS,m_logs->text());
  settings.setValue(SID_ICON_NARROW,m_narrow->text());
  settings.setValue(SID_ICON_NEXT,m_next->text());
  settings.setValue(SID_ICON_PREFERENCES,m_preferences->text());
  settings.setValue(SID_ICON_PRINT,m_print->text());
  settings.setValue(SID_ICON_SEARCH,m_search->text());
  settings.setValue(SID_ICON_SYNC_LEFT,m_syncLeft->text());
  settings.setValue(SID_ICON_SYNC_RIGHT,m_syncRight->text());
  settings.setValue(SID_ICON_UNLINK,m_unlink->text());
  settings.setValue(SID_ICON_WIDEN,m_widen->text());
  settings.setValue(SID_ICON_ZOOM,m_zoom->text());
  settings.setValue(SID_ICON_ZOOM_IN,m_zoomIn->text());
  settings.setValue(SID_ICON_ZOOM_OUT,m_zoomOut->text());


  settings.sync();
  qDebug() << Q_FUNC_INFO << "Synched";

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

  if (compare(&settings,SID_ICON_ABOUT,m_about)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ICON_BACK,m_back)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_BOOKMARKS,m_bookmarks)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_CLEAR,m_clear)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_DOCS,m_docs)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_EXIT,m_exit)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_FIRST,m_first)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_HISTORY,m_history)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ICON_INSERT_LINK,m_insertLink)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ICON_KEYMAPS,m_keymaps)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ICON_KEYMAPS_DISABLED,m_keymapsDisabled)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ICON_LAST,m_last)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_LINK,m_link)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ICON_LOCAL_SEARCH,m_localSearch)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_LOCAL_SEARCH_NEXT,m_localSearchNext)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_LOGS,m_logs)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_NARROW,m_narrow)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_NEXT,m_next)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_PREFERENCES,m_preferences)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_PRINT,m_print)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_SEARCH,m_search)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_SYNC_LEFT,m_syncLeft)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_SYNC_RIGHT,m_syncRight)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_UNLINK,m_unlink)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_WIDEN,m_widen)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_ZOOM,m_zoom)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_ZOOM_IN,m_zoomIn)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ICON_ZOOM_OUT,m_zoomOut)) {
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


  QDir id(imagedirectory);

  if (! fileName.isEmpty()) {
    edit->setText(id.relativeFilePath(fileName));
  }
 setIconFromField(edit,sid);
}
void IconOptions::onSetIcon(const QString & sid,const QString & fileName) {
  QPushButton * btn = this->findChild<QPushButton *>(sid);
  if (btn) {
    btn->setIcon(QIcon(fileName));
  }
  else {
  }
}
void IconOptions::setIconFromField(const QLineEdit * edit, const QString & sid) {
  QDir themeDirectory = getLexicon()->themeDirectory();
  QDir imagedirectory;


  /// if the image directory is absolute then just use it
  /// otherwise it is a subdirectory of the theme root
  QFileInfo im(m_directory->text());
  if (im.isRelative()) {
    if (themeDirectory.cd(m_directory->text())) {
      /// good to go
      imagedirectory = themeDirectory;
    }
    else {
    }
  }
  else {
    imagedirectory = im.dir();
  }
  QString name = edit->text();
  QFileInfo f(name);
  if (f.isRelative()) {
    QFileInfo fi(imagedirectory,name);
    name = fi.absoluteFilePath();
  }
  onSetIcon(sid,name);
}
