#include "findoptions.h"
#include "definedsettings.h"
#include "QsLog.h"
#ifndef STANDALONE
#include "application.h"
#endif
/**
 * Not done:
 *
 * @param theme
 * @param parent
 */
FindOptions::FindOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Entry";
  QVBoxLayout * layout = new QVBoxLayout;
  QGroupBox * fullbox = new QGroupBox(tr("Full text search"));

  m_fullDebug = new QCheckBox;
  m_fullFragmentSize = new QSpinBox;
  m_fullIncludeHeads = new QCheckBox;
  m_fullOneRow = new QCheckBox;
  m_fullStep = new QSpinBox;
  m_fullViewerWidth = new QLineEdit;
  m_fullViewerHeight = new QLineEdit;
  m_fullWholeWord = new QCheckBox;
  m_fullXslt = new QLineEdit;
  m_fullHeadColor = new QLineEdit;
  m_fullHeadColor->setText("wheat");
  QFormLayout * fulllayout = new QFormLayout;
  fulllayout->addRow(tr("Debug"),m_fullDebug);

  QHBoxLayout * colorlayout = new QHBoxLayout;
  colorlayout->addWidget(m_fullHeadColor);
  QPushButton * colorbutton = new QPushButton(tr("Set colour"));
  connect(colorbutton,SIGNAL(clicked()),this,SLOT(onSetColor()));
  colorlayout->addWidget(colorbutton);
  colorlayout->addStretch();

  fulllayout->addRow(tr("Head word colour"),colorlayout);

  fullbox->setLayout(fulllayout);

  layout->addWidget(fullbox);
  setLayout(layout);
  addButtons();
  readSettings();
  setupConnections();

}

void FindOptions::readSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_dirty = false;
}
void FindOptions::writeSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);


  settings.sync();
  settings.endGroup();
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool FindOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);


  return m_dirty;
}
void FindOptions::onSetFont() {
  /*
  bool arabic = false;
  QPushButton * button = qobject_cast<QPushButton *>(QObject::sender());
  QFont f;
  if (button == m_standardButton) {
    f.fromString(m_standardFont->text());
  }
  else {
    arabic = true;
    f.fromString(m_arabicFont->text());
  }
  if (arabic) {
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
      m_arabicFont->setText(font.toString());
    }
    delete d;
    return;
  }
  bool ok;
  QFont font = QFontDialog::getFont(&ok, f,  this);
  if (!ok) {
    return;
  }
  m_standardFont->setText(font.toString());
  */
}
void FindOptions::onSetColor() {
  QColor color;
  color.setNamedColor(m_fullHeadColor->text());
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
      m_fullHeadColor->setText(keys[i]);
      return;
    }
  }
  m_fullHeadColor->setText(d.currentColor().name());
}
