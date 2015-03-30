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
  connect(directorybtn,SIGNAL(clicked()),this,SLOT(onSetFile()));
  directorybtn->setProperty("SID",SID_ICON_DIRECTORY);
  m_directory->setObjectName(SID_ICON_DIRECTORY);
  directorylayout->addWidget(m_directory);
  directorylayout->addWidget(directorybtn);

  layout->addRow(tr("Image directory"),directorylayout);

  m_about = new QLineEdit;
  QPushButton * aboutbtn = new QPushButton(tr("Select icon"));
  QHBoxLayout * aboutlayout = new QHBoxLayout;
  connect(aboutbtn,SIGNAL(clicked()),this,SLOT(onSetFile()));
  QPushButton * abouticon = new QPushButton;
  aboutbtn->setProperty("SID",SID_ICON_ABOUT);
  abouticon->setObjectName(SID_ICON_ABOUT);
  m_about->setObjectName(SID_ICON_ABOUT);
  aboutlayout->addWidget(m_about);
  aboutlayout->addWidget(abouticon);
  aboutlayout->addWidget(aboutbtn);
  layout->addRow(tr("About"),aboutlayout);


  m_back = new QLineEdit;
  QPushButton * backbtn = new QPushButton(tr("Select icon"));
  QHBoxLayout * backlayout = new QHBoxLayout;
  connect(backbtn,SIGNAL(clicked()),this,SLOT(onSetFile()));

  backbtn->setProperty("SID",SID_ICON_BACK);
  m_back->setObjectName(SID_ICON_BACK);
  backlayout->addWidget(m_back);
  backlayout->addWidget(backbtn);
  layout->addRow(tr("Back"),backlayout);


  vlayout->addLayout(layout);
  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();

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
  settings.beginGroup(m_section);


  return m_dirty;
}
void IconOptions::onSetFont() {
  return;
}
void IconOptions::onSetColor() {
}
void IconOptions::onSetFile() {
  QPushButton * btn = qobject_cast<QPushButton *>(sender());
  QString sid =  btn->property("SID").toString();
  QLineEdit * edit = this->findChild<QLineEdit *>(sid);
  if (! edit ) {
    return;
  }
  if (sid == SID_ICON_DIRECTORY) {
    /// TODO QFileDialog dir only
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
