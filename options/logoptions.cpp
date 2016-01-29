#include "logoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#include "externs.h"
#ifndef STANDALONE
#include "application.h"
#endif
/**
 *
 * @param theme
 * @param parent
 */
LogOptions::LogOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  setObjectName("logoptions");
  m_section = "Logging";


  QVBoxLayout * vlayout = new QVBoxLayout;

  QFormLayout * layout = new QFormLayout;

  m_file = new QLineEdit ;
  m_maxSize = new QLineEdit ;
  m_fullPath = new QLineEdit;
  m_fullPath->setVisible(false);
  m_maxSize->setValidator(new QIntValidator);
  m_level = new QComboBox ;
  QStringList levels;
  // these have to be in this order for it to work
  levels << tr("Trace") << tr("Debug") << tr("Info") << tr("Warn") << tr("Error");
  levels << tr("Fatal") << tr("Off");
  //  for(int i=0; i < levels.size();i++) {
  //    m_level->addItem(levels[i],i);
  //  }
  m_level->addItems(levels);
  m_archive = new QSpinBox ;
  this->setControlSize(m_archive,MEDIUM_EDIT);
  m_maxLines = new QLineEdit ;
  m_maxLines->setValidator(new QIntValidator);
  m_interval = new QLineEdit ;
  m_interval->setValidator(new QIntValidator);
  m_rotate = new QCheckBox ;

  QPushButton * button = new QPushButton(tr("Show full path"));
  connect(button,SIGNAL(clicked()),this,SLOT(onFullPath()));

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_file);
  hlayout->addSpacing(10);
  hlayout->addWidget(button);
  hlayout->addStretch();
  layout->addRow(tr("File name"),hlayout);
  layout->addRow(tr("Maximum file size (bytes)"),m_maxSize);
  layout->addRow(tr("Log level"),m_level);
  layout->addRow(tr("Number of archives"),m_archive);
  layout->addRow(tr("Rotate logs"),m_rotate);
  layout->addRow(tr("Viewer lines"),m_maxLines);
  layout->addRow(tr("Viewer refresh interval (msecs)"),m_interval);


  vlayout->addLayout(layout);
  vlayout->addWidget(m_fullPath);

  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();
  this->setLineEditSize(VLARGE_EDIT);
  this->setComboSize(VLARGE_EDIT);
}

void LogOptions::readSettings(bool reload) {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_file->setText(settings.value(SID_LOGGING_FILE,"log.txt").toString());
  m_maxSize->setText(settings.value(SID_LOGGING_MAXSIZE,"64000").toString());
  m_level->setCurrentText(settings.value(SID_LOGGING_LEVEL,"Info").toString());
  m_archive->setValue(settings.value(SID_LOGGING_ARCHIVES,4).toInt());
  m_maxLines->setText(settings.value(SID_LOGGING_VIEWER_MAXLINES,"100").toString());
  m_interval->setText(settings.value(SID_LOGGING_VIEWER_INTERVAL,"10000").toString());
  m_rotate->setChecked(settings.value(SID_LOGGING_ROTATE,true).toBool());

  m_dirty = false;
}
void LogOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup(m_section);
  settings.setValue(SID_LOGGING_FILE,m_file->text());
  settings.setValue(SID_LOGGING_MAXSIZE,m_maxSize->text());
  settings.setValue(SID_LOGGING_LEVEL,m_level->currentText());//Data().toInt());
  settings.setValue(SID_LOGGING_ARCHIVES,m_archive->value());
  settings.setValue(SID_LOGGING_VIEWER_MAXLINES,m_maxLines->text());
  settings.setValue(SID_LOGGING_VIEWER_INTERVAL,m_interval->text());
  settings.setValue(SID_LOGGING_ROTATE,m_rotate->isChecked());
  settings.sync();
  m_dirty = false;
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool LogOptions::isModified()  {
  m_dirty = false;
  m_changes.clear();

  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_LOGGING_FILE,m_file)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_LOGGING_MAXSIZE,m_maxSize)) {
    m_dirty = true;
  }
  /// TODO check max/min
  if (compare(&settings,SID_LOGGING_LEVEL,m_level)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_LOGGING_ARCHIVES,m_archive)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_LOGGING_VIEWER_MAXLINES,m_maxLines)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_LOGGING_VIEWER_INTERVAL,m_interval)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_LOGGING_ROTATE,m_rotate)) {
    m_dirty = true;
  }

  return m_dirty;
}
void LogOptions::onSetFont() {
  /*
  QFont f;
  f.fromString(m_font->text());

  QFontDialog * d = new QFontDialog(f);
  QList<QComboBox *> boxes = d->findChildren<QComboBox *>();
  if (boxes.size() == 1) {
    int ix = boxes[0]->findText("Arabic");
    if (ix >= 0) {
      boxes[0]->setCurrentText("Arabic");
      boxes[0]->activated(ix);
    }
  }
  d->setCurrentFont(f);
  if (d->exec() == QDialog::Accepted) {
    QFont font = d->currentFont();
    m_font->setText(font.toString());
  }
  delete d;
  */
  return;
}
void LogOptions::onSetColor() {
  /*
  QColor color;
  color.setNamedColor(m_highlightColor->text());
  QColorDialog d(color);
  if (d.exec() != QDialog::Accepted) {
    return;
  }
  int r,g,b;
  color = d.currentColor();
  color.getRgb(&r,&g,&b);
  QString str = QString("%1,%2,%3").arg(r).arg(g).arg(b);
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Colors");
  QStringList keys = settings.allKeys();
  QStringList v;
  for(int i=0;i < keys.size();i++) {
    v = settings.value(keys[i]).toStringList();
    if (v.join(",") == str) {

      m_highlightColor->setText(keys[i]);
      return;
    }
  }
  m_highlightColor->setText(d.currentColor().name());
  */
}
void LogOptions::onFullPath() {
  QDialog * d = new QDialog;

  //  QLabel * label = new QLabel(tr("Full path"));
  QLineEdit * edit = new QLineEdit;
  edit->setText(getLexicon()->logFilePath());
  edit->setReadOnly(true);
  m_fullPath->setText(getLexicon()->logFilePath());
  SETTINGS

  settings.beginGroup("EditView");
  QString style;
  style = settings.value(SID_EDITVIEW_FILE_STYLESHEET,QString()).toString();
  if (! style.isEmpty()) {
    edit->setStyleSheet(style);
  }
  d->setWindowTitle(tr("Log file path"));
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(edit);
  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Close);
  QPushButton * button = new QPushButton(tr("Copy to clipboard"));
  connect(button,SIGNAL(clicked()),this,SLOT(onCopy()));
  buttons->addButton(button,QDialogButtonBox::ActionRole);
  connect(buttons,SIGNAL(rejected()),d,SLOT(reject()));
  layout->addWidget(buttons);
  layout->addStretch();
  d->setLayout(layout);
  d->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  d->resize(400,60);
  d->exec();
  delete d;
}
void LogOptions::onCopy() {
  QString str = m_fullPath->text();
  m_fullPath->setSelection(0,str.length());
  m_fullPath->copy();

}
