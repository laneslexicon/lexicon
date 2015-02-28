#include "entryoptions.h"
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
EntryOptions::EntryOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  m_section = "Entry";

  m_back = new QLineEdit ;
  m_css = new QLineEdit ;
  m_clean = new QLineEdit ;
  m_debug = new QCheckBox ;
  m_find = new QLineEdit ;
  m_findNext = new QLineEdit ;
  m_forward = new QLineEdit ;
  m_home = new QLineEdit ;
  m_margin = new QLineEdit ;
  m_mark = new QLineEdit ;
  m_focusUp = new QLineEdit ;
  m_focusDown = new QLineEdit ;
  m_narrow = new QLineEdit ;
  m_reload = new QLineEdit ;
  m_saveHtml = new QCheckBox ;
  m_saveXml = new QCheckBox ;
  m_saveOutputHtml = new QCheckBox ;
  m_show = new QLineEdit ;
  m_showLinkWarning = new QCheckBox ;
  m_step = new QLineEdit ;
  m_highlightColor = new QLineEdit ;
  m_textWidth = new QLineEdit ;
  m_widen = new QLineEdit ;

  m_zoom = new QDoubleSpinBox ;
  m_zoom->setMaximumWidth(75);
  m_zoom->setDecimals(2);
  m_zoom->setSingleStep(0.01);
  m_zoomIn = new QLineEdit ;
  m_zoomIn->setMaximumWidth(30);
  m_zoomOut = new QLineEdit ;
  m_zoomOut->setMaximumWidth(30);

  QVBoxLayout * vlayout = new QVBoxLayout;
  QFormLayout * layout = new QFormLayout;

  /// Zoom
  layout->addRow(tr("Zoom out"),m_zoomOut);
  layout->addRow(tr("Zoom in"),m_zoomIn);
  layout->addRow(tr("Default zoom"),m_zoom);

  /// Focus
  QGridLayout * focuslayout = new QGridLayout;
  m_home->setMaximumWidth(30);
  m_mark->setMaximumWidth(30);
  m_focusUp->setMaximumWidth(30);
  m_focusDown->setMaximumWidth(30);
  focuslayout->addWidget(new QLabel(tr("Up")),0,0);
  focuslayout->addWidget(m_focusUp,0,1);
  focuslayout->addWidget(new QLabel(tr("Down")),0,2);
  focuslayout->addWidget(m_focusDown,0,3);
  focuslayout->addItem(new QSpacerItem(30,30),0,4);
  focuslayout->addWidget(new QLabel(tr("Home")),1,0);
  focuslayout->addWidget(m_home,1,1);
  focuslayout->addWidget(new QLabel(tr("Mark")),1,2);
  focuslayout->addWidget(m_mark,1,3);
  QHBoxLayout * focuscontainer = new QHBoxLayout;
  focuscontainer->addLayout(focuslayout);
  focuscontainer->addStretch();
  layout->addRow(tr("Page movement"),focuscontainer);

  /// Text size
  m_textWidth->setMaximumWidth(50);
  m_margin->setMaximumWidth(50);
  m_narrow->setMaximumWidth(30);
  m_widen->setMaximumWidth(30);
  m_step->setMaximumWidth(30);
  layout->addRow(tr("Text width"),m_textWidth);
  layout->addRow(tr("Text margin"),m_margin);
  layout->addRow(tr("Widen text"),m_widen);
  layout->addRow(tr("Narrow text"),m_narrow);
  layout->addRow(tr("Step size"),m_step);

  // Lexicon movement
  m_back->setMaximumWidth(30);
  m_forward->setMaximumWidth(30);
  layout->addRow(tr("Lexicon next entry"),m_forward);
  layout->addRow(tr("Lexicon previous entry"),m_back);




  /// Local search
  QHBoxLayout * searchlayout = new QHBoxLayout;
  m_find->setMaximumWidth(30);
  m_findNext->setMaximumWidth(30);
  m_clean->setMaximumWidth(30);
  searchlayout->addWidget(new QLabel(tr("Find")));
  searchlayout->addWidget(m_find);
  searchlayout->addWidget(new QLabel(tr("Find next")));
  searchlayout->addWidget(m_findNext);
  searchlayout->addWidget(new QLabel(tr("Clear highlight")));
  searchlayout->addWidget(m_clean);
  searchlayout->addStretch();
  layout->addRow(tr("Local search"),searchlayout);
  layout->addRow(tr("Show last search results"),m_show);
  /// Assorted

  m_reload->setMaximumWidth(30);
  m_show->setMaximumWidth(30);
  m_showLinkWarning->setMaximumWidth(30);
  m_highlightColor->setMaximumWidth(100);
  layout->addRow(tr("Reload"),m_reload);

  layout->addRow(tr("Show link warning"),m_showLinkWarning);

  QHBoxLayout * colorlayout = new QHBoxLayout;
  colorlayout->addWidget(m_highlightColor);
  QPushButton * colorbutton = new QPushButton(tr("Set colour"));
  connect(colorbutton,SIGNAL(clicked()),this,SLOT(onSetColor()));
  colorlayout->addWidget(colorbutton);
  colorlayout->addStretch();
  layout->addRow(tr("Highlight color"),colorlayout);

  /// Debug
  QHBoxLayout * debugcontainer = new QHBoxLayout;
  QGridLayout * debuglayout = new QGridLayout;
  debuglayout->addWidget(new QLabel(tr("Debug")),0,0);
  debuglayout->addWidget(m_debug,0,1);
  debuglayout->addWidget(new QLabel(tr("Save HTML")),0,2);
  debuglayout->addWidget(m_saveHtml,0,3);
  debuglayout->addWidget(new QLabel(tr("Save inner HTML")),1,0);
  debuglayout->addWidget(m_saveOutputHtml,1,1);
  debuglayout->addWidget(new QLabel(tr("Save XML")),1,2);
  debuglayout->addWidget(m_saveXml,1,3);
  debugcontainer->addLayout(debuglayout);
  debugcontainer->addStretch();
  layout->addRow(tr("Tech"),debugcontainer);

  vlayout->addLayout(layout);
  vlayout->addStretch();
  setLayout(vlayout);
  addButtons();
  readSettings();
  setupConnections();

}

void EntryOptions::readSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);
  m_back->setText(settings.value(SID_ENTRY_BACK).toString());
  m_css->setText(settings.value(SID_ENTRY_CSS).toString());
  m_clean->setText(settings.value(SID_ENTRY_CLEAN).toString());
  m_debug->setChecked(settings.value(SID_ENTRY_DEBUG).toBool());
  m_find->setText(settings.value(SID_ENTRY_FIND).toString());
  m_findNext->setText(settings.value(SID_ENTRY_FIND_NEXT).toString());
  m_forward->setText(settings.value(SID_ENTRY_FORWARD).toString());
  m_home->setText(settings.value(SID_ENTRY_HOME).toString());
  m_margin->setText(settings.value(SID_ENTRY_MARGIN).toString());
  m_mark->setText(settings.value(SID_ENTRY_MARK).toString());
  m_focusUp->setText(settings.value(SID_ENTRY_MOVE_FOCUS_UP).toString());
  m_focusDown->setText(settings.value(SID_ENTRY_MOVE_FOCUS_DOWN).toString());
  m_narrow->setText(settings.value(SID_ENTRY_NARROW).toString());
  m_reload->setText(settings.value(SID_ENTRY_RELOAD).toString());
  m_saveHtml->setChecked(settings.value(SID_ENTRY_DUMP_HTML).toBool());
  m_saveXml->setChecked(settings.value(SID_ENTRY_DUMP_XML).toBool());
  m_saveOutputHtml->setChecked(settings.value(SID_ENTRY_DUMP_OUTPUT_HTML).toBool());
  m_show->setText(settings.value(SID_ENTRY_SHOW).toString());
  m_showLinkWarning->setChecked(settings.value(SID_ENTRY_SHOW_LINK_WARNING).toBool());
  m_step->setText(settings.value(SID_ENTRY_STEP).toString());
  m_highlightColor->setText(settings.value(SID_ENTRY_HIGHLIGHT_COLOR).toString());
  m_textWidth->setText(settings.value(SID_ENTRY_TEXT_WIDTH).toString());
  m_widen->setText(settings.value(SID_ENTRY_WIDEN).toString());
  m_zoom->setValue(settings.value(SID_ENTRY_SCALE).toDouble());
  m_zoomIn->setText(settings.value(SID_ENTRY_ZOOM_IN).toString());
  m_zoomOut->setText(settings.value(SID_ENTRY_ZOOM_OUT).toString());

  m_dirty = false;
}
void EntryOptions::writeSettings() {
  qDebug() << Q_FUNC_INFO << m_settingsFileName;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  settings.setValue(SID_ENTRY_BACK,m_back->text());
  settings.setValue(SID_ENTRY_CSS,m_css->text());
  settings.setValue(SID_ENTRY_CLEAN,m_clean->text());
  settings.setValue(SID_ENTRY_DEBUG,m_debug->isChecked());
  settings.setValue(SID_ENTRY_FIND,m_find->text());
  settings.setValue(SID_ENTRY_FIND_NEXT,m_findNext->text());
  settings.setValue(SID_ENTRY_FORWARD,m_forward->text());
  settings.setValue(SID_ENTRY_HOME,m_home->text());
  settings.setValue(SID_ENTRY_MARGIN,m_margin->text());
  settings.setValue(SID_ENTRY_MARK,m_mark->text());
  settings.setValue(SID_ENTRY_MOVE_FOCUS_UP,m_focusUp->text());
  settings.setValue(SID_ENTRY_MOVE_FOCUS_DOWN,m_focusDown->text());
  settings.setValue(SID_ENTRY_NARROW,m_narrow->text());
  settings.setValue(SID_ENTRY_RELOAD,m_reload->text());
  settings.setValue(SID_ENTRY_DUMP_HTML,m_saveHtml->isChecked());
  settings.setValue(SID_ENTRY_DUMP_XML,m_saveXml->isChecked());
  settings.setValue(SID_ENTRY_DUMP_OUTPUT_HTML,m_saveOutputHtml->isChecked());
  settings.setValue(SID_ENTRY_SHOW,m_show->text());
  settings.setValue(SID_ENTRY_SHOW_LINK_WARNING,m_showLinkWarning->isChecked());
  settings.setValue(SID_ENTRY_STEP,m_step->text());
  settings.setValue(SID_ENTRY_HIGHLIGHT_COLOR,m_highlightColor->text());
  settings.setValue(SID_ENTRY_TEXT_WIDTH,m_textWidth->text());
  settings.setValue(SID_ENTRY_WIDEN,m_widen->text());
  settings.setValue(SID_ENTRY_SCALE,m_zoom->value());
  settings.setValue(SID_ENTRY_ZOOM_IN,m_zoomIn->text());
  settings.setValue(SID_ENTRY_ZOOM_OUT,m_zoomOut->text());
  m_dirty = false;
  emit(modified(false));
}
/**
 * TODO not complete
 *
 *
 * @return
 */
bool EntryOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (compare(&settings,SID_ENTRY_BACK,m_back)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ENTRY_CSS,m_css)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_CLEAN,m_clean)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ENTRY_DEBUG,m_debug)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_FIND,m_find)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_FIND_NEXT,m_findNext)) {
    m_dirty = true;
  }
  if (compare(&settings,SID_ENTRY_FORWARD,m_forward)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_HOME,m_home)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_MARGIN,m_margin)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_MARK,m_mark)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_MOVE_FOCUS_UP,m_focusUp)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_MOVE_FOCUS_DOWN,m_focusDown)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_NARROW,m_narrow)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_RELOAD,m_reload)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_DUMP_HTML,m_saveHtml)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_DUMP_XML,m_saveXml)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_DUMP_OUTPUT_HTML,m_saveOutputHtml)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_SHOW,m_show)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_SHOW_LINK_WARNING,m_showLinkWarning)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_STEP,m_step)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_HIGHLIGHT_COLOR,m_highlightColor)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_TEXT_WIDTH,m_textWidth)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_WIDEN,m_widen)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_SCALE,m_zoom)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_ZOOM_IN,m_zoomIn)) {
    m_dirty = true;
  }

  if (compare(&settings,SID_ENTRY_ZOOM_OUT,m_zoomOut)) {
    m_dirty = true;
  }

  return m_dirty;
}
void EntryOptions::onSetFont() {
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
void EntryOptions::onSetColor() {
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
}
