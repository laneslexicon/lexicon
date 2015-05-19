#include "imeditor.h"
#include "imedit.h"
#include "scripts.h"
#include "showmapwidget.h"
#include "imeditoroptions.h"
#ifdef LANE
#include "application.h"
#include "externs.h"
#endif
ImEditor::ImEditor(QWidget * parent) : QWidget(parent) {
  m_mapSignals = new QSignalMapper(this);
  m_edit = new ImEdit;

  m_printer = new QPrinter();
  m_mapBox = new QComboBox;


  m_helpBox = new QComboBox;

  QList<QByteArray> ba =  QTextDocumentWriter::supportedDocumentFormats();
  QStringList docFormats;
  for(int i=0;i < ba.size();i++) {
    m_formats << QString(ba[i].data());
  }
  m_formats.clear();
  m_formats << "html" << "odf" << "txt";
  createActions();
  readSettings();
  createToolbar();
  m_toolbar->setVisible( m_toolbarVisible);
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_toolbar);
  layout->addWidget(m_edit);
  setLayout(layout);

  connect(m_edit,SIGNAL(charInserted(int,int)),this,SLOT(charInserted(int,int)));
  connect(m_edit,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPositionChanged()));
  connect(m_edit,SIGNAL(logMessage(const QString &)),this,SLOT(logMessage(const QString &)));
  connect(m_edit,SIGNAL(gotFocus(int)),this,SIGNAL(editFocus(int)));
  QLocale l;
  QString script = QLocale::scriptToString(l.script());
  if (! m_scriptFonts.contains(script)) {
    /// Prompt for font and save settings
  }
  setCharFormat(script);
  m_edit->setFocusPolicy(Qt::StrongFocus);
  m_edit->setFocus(Qt::OtherFocusReason);
  m_mapBox->setVisible(m_mapBox->count() != 0);
  m_helpBox->setVisible(m_helpBox->count() != 0);

  m_mapBox->setCurrentText(m_mapOffValue);
  m_helpBox->setCurrentIndex(-1);
  connect(m_mapBox,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onChangeMap(const QString &)));
  connect(m_helpBox,SIGNAL(currentTextChanged(const QString &)),this,SLOT(onShowHelp(const QString &)));
  this->cursorPositionChanged();
}
ImEditor::~ImEditor() {
  qDebug() << Q_FUNC_INFO;
  QString settingsFileName("editor.ini");
#ifdef LANE
  settingsFileName = getLexicon()->editorSettingsFileName();
#endif
  QSettings settings(settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Editor");
  settings.setValue("Size",this->size());
  settings.setValue("Position",this->pos());
}
void ImEditor::hideHelp() {
  qDebug() << Q_FUNC_INFO;
  QWidgetList widgets = QApplication::topLevelWidgets();

  for(int i=0;i < widgets.size();i++) {
    ShowMapWidget * w = qobject_cast<ShowMapWidget *>(widgets[i]);
    if (w)  {
      w->hide();
    }
  }
}
void ImEditor::createActions() {
  m_openAction = new QAction(tr("Open"),this);
  m_openAction->setIcon(QPixmap(":/png/open16.png"));
  connect(m_openAction,SIGNAL(triggered()),this,SLOT(onOpen()));

  m_importAction = new QAction(tr("Import"),this);
  m_importAction->setIcon(QPixmap(":/png/document-import16.png"));
  connect(m_importAction,SIGNAL(triggered()),this,SLOT(onImport()));

  m_printAction = new QAction(tr("Print"),this);
  m_printAction->setIcon(QPixmap(":/png/print16.png"));
  connect(m_printAction,SIGNAL(triggered()),this,SLOT(onPrint()));

  m_printOptionsAction = new QAction(tr("Print Preferences"),this);
  m_printOptionsAction->setIcon(QPixmap(":/png/printers16.png"));
  connect(m_printOptionsAction,SIGNAL(triggered()),this,SLOT(onPrinters()));

  m_saveAction = new QAction(tr("Save"),this);
  m_saveAction->setIcon(QPixmap(":/png/save16.png"));
  connect(m_saveAction,SIGNAL(triggered()),this,SLOT(onSave()));

  m_saveAsAction = new QAction(tr("Save as"),this);
  m_saveAsAction->setIcon(QPixmap(":/png/saveas16.png"));
  connect(m_saveAsAction,SIGNAL(triggered()),this,SLOT(onSaveAs()));

  m_fontAction = new QAction(tr("Font"),this);
  m_fontAction->setIcon(QPixmap(":/png/font16.png"));
  connect(m_fontAction,SIGNAL(triggered()),this,SLOT(onFont()));

  m_insertBlockAction = new QAction(tr("Insert block"),this);
  m_insertBlockAction->setIcon(QPixmap(":/png/insert-block16.png"));
  connect(m_insertBlockAction,SIGNAL(triggered()),this,SLOT(onInsertBlock()));

  m_insertCharAction = new QAction(tr("Insert char"),this);
  m_insertCharAction->setIcon(QPixmap(":/png/insert-text16.png"));
  connect(m_insertCharAction,SIGNAL(triggered()),this,SLOT(onInsertChar()));

  m_optionsAction = new QAction(tr("Options"),this);
  m_optionsAction->setIcon(QPixmap(":/png/preferences16.png"));
  connect(m_optionsAction,SIGNAL(triggered()),this,SLOT(onOptions()));
}
void ImEditor::createToolbar() {
  m_toolbar = new QToolBar(this);

  m_toolbar->addAction(m_openAction);
  m_toolbar->addAction(m_importAction);
  m_toolbar->addAction(m_saveAction);
  m_toolbar->addAction(m_saveAsAction);
  m_toolbar->addAction(m_printAction);
  m_toolbar->addAction(m_printOptionsAction);
  m_toolbar->addAction(m_fontAction);
  m_toolbar->addAction(m_insertCharAction);
  m_toolbar->addAction(m_insertBlockAction);
  m_toolbar->addAction(m_optionsAction);
  m_toolbar->addSeparator();
  QWidget * w = new QWidget;
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr(" Maps  ")));
  hlayout->addWidget(m_mapBox);
  hlayout->addWidget(new QLabel(tr("  Help  ")));
  hlayout->addWidget(m_helpBox);
  w->setLayout(hlayout);
  m_mapBoxAction = m_toolbar->addWidget(w);
}
void ImEditor::readSettings() {
  QString str;
  QShortcut * cut;
  QStringList keys;
  QString settingsFileName("editor.ini");
#ifdef LANE
  settingsFileName = getLexicon()->editorSettingsFileName();
#endif
  qDebug() << Q_FUNC_INFO << settingsFileName;
  QSettings settings(settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");
  m_edit->setDebug(settings.value("Debug",false).toBool());
  m_mapOffValue = settings.value("Map off","-None-").toString();
  m_mapBox->addItem(m_mapOffValue);
  m_autoSaveSettings = settings.value("Autosave settings",true).toBool();
  m_interactiveFont = settings.value("Prompt font",false).toBool();
  m_toolbarVisible = settings.value("Toolbar",true).toBool();
  m_pdfPrint = settings.value("PDF print",false).toBool();
  str =  settings.value("Keymap off","Ctrl+K,D").toString();
  cut = new QShortcut(QKeySequence(str),this);
  connect(cut, SIGNAL(activated()), m_mapSignals, SLOT(map()));
  m_mapSignals->setMapping(cut, m_mapOffValue);

  str = settings.value("Keymap help").toString();
  if (! str.isEmpty()) {
    cut = new QShortcut(QKeySequence(str),this);
    connect(cut, SIGNAL(activated()), this, SLOT(onMapHelp()));
  }
  str = settings.value("Insert block","Ctrl+J,B").toString();
  cut = new QShortcut(QKeySequence(str),this);
  connect(cut,SIGNAL(activated()),this,SLOT(onInsertBlock()));

  m_insertBlockAction->setShortcut(QKeySequence(str));
  connect(m_insertBlockAction,SIGNAL(triggered()),this,SLOT(onInsertBlock()));


  str = settings.value("Insert char","Ctrl+J,I").toString();
  cut = new QShortcut(QKeySequence(str),this);
  connect(cut,SIGNAL(activated()),this,SLOT(onInsertChar()));

  //  m_insertCharAction->setShortcut(QKeySequence(str));
  //  connect(m_insertCharAction,SIGNAL(triggered()),this,SLOT(onInsertChar()));
  settings.endGroup();

  settings.beginGroup("Maps");
  QStringList maps = settings.childGroups();
  for(int i=0;i < maps.size();i++) {
    settings.beginGroup(maps[i]);
    str = settings.value("File").toString();
    QFileInfo fi(str);
    if (fi.exists()) {
      m_edit->loadMap(str);
      m_mapBox->addItem(maps[i]);
      str = settings.value("Shortcut").toString();
      if (! str.isEmpty()) {
        QShortcut * cut = new QShortcut(QKeySequence(str),this);
        connect(cut, SIGNAL(activated()), m_mapSignals, SLOT(map()));
        m_mapSignals->setMapping(cut, maps[i]);
        m_mapToScript.insert(maps[i],m_edit->getScript(maps[i]));
      }
      str = settings.value("Help").toString();
      if (! str.isEmpty()) {
        m_mapHelp.insert(maps[i],str);
        m_helpBox->addItem(maps[i]);
      }
    }
    settings.endGroup();
  }
  settings.endGroup();

  settings.beginGroup("Scripts");
  QStringList scripts = settings.childKeys();
  for(int i=0;i < scripts.size();i++) {
    m_scriptFonts.insert(scripts[i],settings.value(scripts[i]).toString());
  }
  settings.endGroup();

  connect(m_mapSignals, SIGNAL(mapped(const QString &)),
          this, SLOT(onMap(const QString &)));

  /*
  QFont f;
  f.fromString(m_scriptFonts.value("Latin"));
  m_edit->setDocFont(f);
  */
  if (m_mapBox->count() == 0) {
    m_helpBox->hide();
  }
  /*
  qDebug() << m_scriptFonts;
  qDebug() << m_mapToScript;
  qDebug() << m_mapHelp;
  */
}
void ImEditor::hideMaps(bool show) {
  m_mapBoxAction->setVisible(show);
}
void ImEditor::hidePrint(bool show) {
  m_printAction->setVisible(show);
  m_printOptionsAction->setVisible(show);
}
void ImEditor::onMap(const QString & m) {
  qDebug() << Q_FUNC_INFO << QString("Current script %1, requested map %2").arg(m_currentScript).arg(m);
  if (m == m_mapOffValue) {
    m_edit->enableMapping(false);
    /// this needs to come from somewhere
    this->setCharFormat("Latin");
  }
  else {
    m_edit->setCurrentMap(m);
    this->setCharFormat(m);
  }
  m_mapBox->blockSignals(true);
  m_mapBox->setCurrentText(m);
  m_mapBox->blockSignals(false);
}
void ImEditor::setCharFormat(const QString & mapname) {
  QString script;
  if (m_mapToScript.contains(mapname)) {
    script = m_mapToScript.value(mapname);
  }
  QLocale l = QLocale::system();
  if (script.isEmpty()) {
    script = "Latin";
  }
  if (m_scriptFonts.contains(script)) {
    QFont f;
    f.fromString(m_scriptFonts.value(script));
    QTextCursor cursor = m_edit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setFont(f);
    /*
    int pos = cursor.position();
    if (pos > 0) {

    }
    */
    cursor.setCharFormat(fmt);
    m_edit->setTextCursor(cursor);
  }
  m_currentScript = script;
}
void ImEditor::onFont() {
  qDebug() << Q_FUNC_INFO;
  QTextCursor c = m_edit->textCursor();
  QFont charFont;
  if (c.hasSelection()) {
    charFont = c.charFormat().font();
    qDebug() << "Selection font" << c.charFormat().fontFamily();
    qDebug() << "Start/End/Pos" << c.selectionStart() << c.selectionEnd() << c.position();
    /*
    QTextCursor cursor = m_edit->textCursor();
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    int currPos = cursor.position();
    QStringList f;
    for(int i=startPos;i <= endPos;i++) {
      cursor.setPosition(i);
      //      m_edit->setTextCursor(cursor);
      if (! f.contains(cursor.charFormat().fontFamily())) {
          f << cursor.charFormat().fontFamily();
      }
    }
    qDebug() << "families" << f;
    */
    qDebug() << "charfont" << charFont.toString();
    qDebug() << QString("[%1][Bold=%2][Italic=%3]").arg(charFont.family())
      .arg(charFont.bold()).arg(charFont.italic());
  }
  QFont font;

  QFontDialog d(this);
  d.setCurrentFont(charFont);
  if (d.exec() == QDialog::Accepted) {
    font = d.selectedFont();
  }
  else {
    return;
  }

  if (m_autoSaveSettings) {
    QSettings settings("editor.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("Edit");
    settings.setValue("Font",font.toString());
  }

  if (! c.hasSelection()) {
    m_edit->setDocFont(font);
    return;
  }
  QTextCharFormat fmt = c.charFormat();
  fmt.setFont(font);
  c.setCharFormat(fmt);

}
/**
 * save as html
 *
 */
void ImEditor::onSave() {
  qDebug() << Q_FUNC_INFO << m_formats << sender();
  QString formats = QString(tr("Documents (%1)")).arg(m_formats.join(" "));

  if (m_documentFileName.isEmpty()) {
    QString fileName = QFileDialog::getSaveFileName(this,
                         tr("Save Document"), ".", formats);
    if (fileName.isEmpty()) {
      return;
    }
    m_documentFileName = fileName;
  }
  QFileInfo fi(m_documentFileName);
  QString suffix = fi.suffix();
  if (! m_formats.contains(suffix)) {
    suffix = suffix.toLower();
    if (! m_formats.contains(suffix)) {
      qDebug() << "Unsupported format";
      suffix = "html";
    }

  }
  QTextDocumentWriter w;
  w.setFormat(suffix.toLatin1());
  w.setFileName(m_documentFileName);
  w.write(m_edit->document());
}

void ImEditor::onSaveAs() {
  QString formats = QString(tr("Documents (%1)")).arg(m_formats.join(" "));
  QString fileName = QFileDialog::getSaveFileName(this,
     tr("Save Document"), ".", formats);

  QFileInfo fi(fileName);
  if (fi.exists()) {
    /// overwrite
  }
  QString suffix = fi.suffix();
  if (! m_formats.contains(suffix)) {
    suffix = suffix.toLower();
    if (! m_formats.contains(suffix)) {
      qDebug() << "Unsupported format";
      suffix = "html";
    }

  }
  QTextDocumentWriter w;
  w.setFormat(suffix.toLatin1());
  w.setFileName(fileName);
  w.write(m_edit->document());
}
void ImEditor::onImport() {
 QString fileName = QFileDialog::getOpenFileName(this,
     tr("Import Document"), ".", tr("Documents (*.odf *.html *.txt)"));

 if (fileName.isEmpty()) {
   return;
 }
 QFile file(fileName);
 if (! file.open(QIODevice::ReadOnly)) {
   return;
 }
 QTextStream in(&file);
 QString str = in.readAll();
 file.close();
 QFileInfo fi(fileName);
 QTextCursor cursor = m_edit->textCursor();
 if (fi.suffix().toLower() == "html") {
   cursor.insertHtml(str);
 }
 else {
   cursor.insertText(str);
 }
 if (m_documentFileName.isEmpty()) {
   m_documentFileName = fileName;
 }
 this->scan();
 setWindowTitle(fileName);
 qDebug() << "Curent map" << m_edit->currentMap();
}
void ImEditor::onOpen() {
 QString fileName = QFileDialog::getOpenFileName(this,
     tr("Open Document"), ".", tr("Documents (*.odf *.html *.txt)"));

 if (fileName.isEmpty()) {
   return;
 }
 QFile file(fileName);
 if (! file.open(QIODevice::ReadOnly)) {
   return;
 }
 QTextStream in(&file);
 QString str = in.readAll();
 file.close();
 QFileInfo fi(fileName);
 if (fi.suffix().toLower() == "html") {
   m_edit->setHtml(str);
 }
 else {
   m_edit->setPlainText(str);
 }
 m_documentFileName = fileName;
 this->scan();
 setWindowTitle(fileName);
 qDebug() << "Curent map" << m_edit->currentMap();
}
void ImEditor::onPrint() {
  qDebug() << Q_FUNC_INFO << m_printer->printerName();
  if (!m_printer->isValid()) {
    this->onPrinters();
  }
  if (!m_printer->isValid()) {
    return;
  }
  if (m_pdfPrint) {
    m_printer->setOutputFormat(QPrinter::PdfFormat);
  }
  if (m_printer->outputFormat() == QPrinter::NativeFormat) {
    m_edit->document()->print(m_printer);
    return;
  }
  if (m_documentFileName.isEmpty()) {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Document name for PDF print"),
                                                QDir::current().absoluteFilePath("."),
                           tr("Documents (*.odf *.html *.txt)"));
    if (fileName.isEmpty()) {
      return;
    }
    m_documentFileName  = fileName;
  }
  QFileInfo fi(m_documentFileName);

  QString outputFile = fi.canonicalPath() + QDir::separator() + fi.baseName();
  if (m_printer->outputFormat() == QPrinter::PdfFormat) {
    outputFile += ".pdf";
  }
  QFile f(outputFile);
  if (! f.exists()) {
    m_printer->setOutputFileName(outputFile);
  }
  m_edit->document()->print(m_printer);
}
void ImEditor::onPrinters() {
  QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
  if (printers.size() == 0) {
    QMessageBox msgBox;
    msgBox.setText(tr("No printers are available."));
    msgBox.setInformativeText(tr("All printing will be to PDF"));
    msgBox.exec();
    return;
  }

  QPrinter * printer = new QPrinter();
  QPrintDialog * printDialog = new QPrintDialog(printer, this);
  printDialog->setWindowModality(Qt::WindowModal);
  printDialog->setWindowTitle("Set and Configure Printer");
  printDialog->open(this,SLOT(onPrinterSetup()));
}
void ImEditor::charInserted(int unicode,int position) {
  //  qDebug() << Q_FUNC_INFO << unicode << QChar(unicode) << UcdScripts::getScript(unicode) << position;
  QString family = m_edit->textCursor().charFormat().fontFamily();
  QString script = UcdScripts::getScript(unicode);
  //  qDebug() << Q_FUNC_INFO << QString("[%1][%2][%3][%4]").arg(m_currentScript).arg(script).arg(QChar(unicode)).arg(family);
  //  qDebug() << Q_FUNC_INFO << QString("Cursor font [%1]").arg(family);
  if (!m_scriptFonts.contains(script)) {
    return;
  }

  if (script != m_currentScript) {
    qDebug() << "Font mismatch" << m_currentScript << script;
    //    m_currentScript = script;
  }
  /*
    if (m_scriptFonts.contains(script)) {
      QFont f;
      f.fromString(m_scriptFonts.value(script));
      QTextCursor cursor = m_edit->textCursor();
      QTextCharFormat fmt = cursor.charFormat();
      fmt.setFont(f);
      int pos = cursor.position();
      if (pos > 0) {
        //        cursor.setPosition(pos - 1,QTextCursor::MoveAnchor);
      }
      cursor.setCharFormat(fmt);
      cursor.movePosition(QTextCursor::PreviousCharacter);
      //      cursor.setPosition(pos+1,QTextCursor::KeepAnchor);

      //      cursor.movePosition(QTextCursor::NextCharacter);
      //      cursor.setCharFormat(fmt);
      m_edit->setTextCursor(cursor);
      qDebug() << "Switch font to" << f.family() << pos << script;
      m_currentScript = script;
    }
  }
  */
}
void ImEditor::cursorPositionChanged() {
  QString family = m_edit->textCursor().charFormat().fontFamily();
  QTextCursor cursor = m_edit->textCursor();


  if (!m_scriptFonts.contains(m_currentScript)) {
    return;
  }
  QFont f;
  f.fromString(m_scriptFonts.value(m_currentScript));
  if (f.family() != family) {
    QTextCursor cursor = m_edit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setFont(f);
    cursor.setCharFormat(fmt);
    m_edit->setTextCursor(cursor);
    qDebug() << Q_FUNC_INFO << "changed format";
  }

}
void ImEditor::scan() {
  QTextCharFormat fmt;
  QFont font;
  QTextCursor cursor = m_edit->textCursor();
  int sz = m_edit->document()->characterCount();
  QString script;
  QString currentScript;
  QList<QPair<int,QString> > m;
  for(int i=0;i < sz;i++) {
    script = UcdScripts::getScript(m_edit->document()->characterAt(i).unicode());
    if (m_scriptFonts.contains(script) && (script != currentScript)) {
      currentScript = script;
      m << qMakePair(i,script);
    }
  }
  qDebug() << m;
  if (m.size() == 0) {
    return;
  }

  int endPos;
  int startPos;

  for(int i=0;i <= (m.size() - 1);i++) {
    startPos = m[i].first;
    if (i == (m.size() - 1)) {
      endPos = sz;
    }
    else {
      endPos = m[i+1].first;
    }
    endPos--;
    cursor.setPosition(startPos,QTextCursor::MoveAnchor);
    fmt = cursor.charFormat();
    font.fromString(m_scriptFonts.value(m[i].second));
    fmt.setFont(font);
    cursor.setPosition(endPos,QTextCursor::KeepAnchor);
    QString text = cursor.selectedText();
    m_currentScript = m[i].second;
    if (m_interactiveFont) {
      QMessageBox msgBox;
      msgBox.setText(QString("Change font to %1.").arg(font.family()));
      if (text.size() > 20) {
        msgBox.setInformativeText(text.left(20));
        msgBox.setDetailedText(text);
      }
      else {
        msgBox.setInformativeText(text);
      }
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Yes);

      int ret = msgBox.exec();
      if (ret == QMessageBox::Yes) {
        cursor.setCharFormat(fmt);
      }
    }
    else {
      cursor.setCharFormat(fmt);
    }
  }
}
void ImEditor::onMapHelp() {
  qDebug() << Q_FUNC_INFO;
  QString map = m_edit->currentMap();
  this->onShowHelp(map);
}
void ImEditor::onShowHelp(const QString & mapname) {
  qDebug() << Q_FUNC_INFO << mapname;
  QString map = mapname;
  if (mapname.isEmpty()) {
    map = m_edit->currentMap();
  }
  QWidgetList widgets = QApplication::topLevelWidgets();

  for(int i=0;i < widgets.size();i++) {
    ShowMapWidget * w = qobject_cast<ShowMapWidget *>(widgets[i]);
    if (w && (w->map() == map))  {
      w->show();
      w->raise();
      return;
    }
  }


  if (m_mapHelp.contains(map)) {
    QString fileName = m_mapHelp.value(map);
    ShowMapWidget * w = new ShowMapWidget();
    w->setMapName(map);
    w->loadHtml(fileName);
    w->show();
  }

}
void ImEditor::onPrinterSetup() {
  QPrintDialog * dlg = qobject_cast<QPrintDialog *>(sender());
  if (! dlg ) {
    return;
  }
  QPrinter * printer = dlg->printer();
  if (dlg->result() == QDialog::Accepted) {
    delete m_printer;
    m_printer = printer;
  }
  else {
    delete printer;
  }
}
void ImEditor::onInsertBlock() {
  QTextCursor cursor =  m_edit->textCursor();
  QTextCharFormat fmt = cursor.charFormat();
  qDebug() << "Char format" << fmt.fontFamily();
  cursor.insertBlock();
  QTextBlock block = cursor.block();
  //  block.clearLayout();
  /// for this to work, the script named in the map file
  /// has to match the script
  QLocale m(m_edit->currentScript());
  //  qDebug() << "Locale text direction" << m.textDirection();
  //  qDebug() << "Block text directiont" << block.textDirection();
  //  qDebug() << "Left to right" << (block.textDirection() == Qt::LeftToRight);
  QTextBlockFormat blockfmt = block.blockFormat();
  blockfmt.setLayoutDirection(m.textDirection());
  cursor.setBlockFormat(blockfmt);
}
void ImEditor::logMessage(const QString & t) {
  qDebug() << t;
}
void ImEditor::onInsertChar() {
  bool ok;
  QString text = QInputDialog::getText(this, tr("Insert Unicode Character"),
                                       tr("Enter decimal or hex (prefixed by 0x) code point:"), QLineEdit::Normal,"", &ok);
  if (!ok) {
    return;
  }
  ok = true;
  int c = text.toUInt(&ok,0);
  if (ok) {
    wchar_t wc(c);
    QTextCursor cursor = m_edit->textCursor();
    cursor.insertText(QString::fromWCharArray(&wc,1));
  }
  else {
    qDebug() << "Conversion error";
  }
}
void ImEditor::onOptions() {
  ImEditorOptions * options = new ImEditorOptions();
  //  options->setWindowModality(Qt::WindowModal);
  if (options->exec() == QDialog::Accepted) {
    options->writeSettings();
  }
  delete options;
}
void ImEditor::onChangeMap(const QString & map) {
  this->onMap(map);
  m_edit->setFocus();
}
