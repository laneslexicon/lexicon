#include "imeditoroptions.h"
#include "scripts.h"
#include "inputmapper.h"
#ifdef LANE
#include "application.h"
#include "externs.h"
#endif
/**
 *
 *
 * @param parent
 */
EditMapDialog::EditMapDialog(QWidget * parent) : QDialog(parent) {
  m_file = new QLineEdit;
  setObjectName("editmapdialog");
  setWindowTitle(tr("Map Settings"));
  QRegExpValidator * rxv = new QRegExpValidator();
  QRegExp rx("\\S+");
  rxv->setRegExp(rx);            // one or more non-whitespace characters
  m_file->setValidator(rxv);


  m_help = new QLineEdit;
  m_shortcut = new QKeySequenceEdit;
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  QVBoxLayout * layout = new QVBoxLayout;
  QFormLayout * formlayout = new QFormLayout;

  QPushButton * mapButton = new QPushButton("...");
  QHBoxLayout * maplayout = new QHBoxLayout;
  maplayout->addWidget(m_file);
  maplayout->addWidget(mapButton);
  formlayout->addRow(tr("File"),maplayout);

  QPushButton * helpButton = new QPushButton("...");
  QHBoxLayout * helplayout = new QHBoxLayout;
  helplayout->addWidget(m_help);
  helplayout->addWidget(helpButton);

  formlayout->addRow(tr("Help ile"),helplayout);
  formlayout->addRow(tr("Shortcut"),m_shortcut);

  layout->addLayout(formlayout);
  layout->addWidget(buttonBox);
  layout->addStretch();
  setLayout(layout);
  connect(mapButton,SIGNAL(clicked()),this,SLOT(onMapFile()));
  connect(helpButton,SIGNAL(clicked()),this,SLOT(onHelpFile()));
}
QString EditMapDialog::mapfile() const {
  return m_file->text();
}
QString EditMapDialog::helpfile() const {
  return m_help->text();
}
QString EditMapDialog::keySequence() const {
  return m_shortcut->keySequence().toString();
}
QSize EditMapDialog::sizeHint() const {
  return QSize(400,100);
}
void EditMapDialog::onMapFile() {
  QFileInfo fi(m_file->text());

  QString fileName = QFileDialog::getOpenFileName(this,tr("Select map file"),
                               fi.absolutePath(),tr("All files (*.*)"));

  if (! fileName.isEmpty()) {
    m_file->setText(fileName);
  }
}
void EditMapDialog::onHelpFile() {
  QFileInfo fi(m_help->text());

  QString fileName = QFileDialog::getOpenFileName(this,tr("Select help file"),
                               fi.absolutePath(),tr("HTML files (*.html)"));

  if (! fileName.isEmpty()) {
    m_help->setText(fileName);
  }
}
void EditMapDialog::setFile(const QString & t) {
  m_file->setText(t);
}
void EditMapDialog::setHelp(const QString & t) {
  m_help->setText(t);
}
void EditMapDialog::setMapShortcut(const QString & t) {
  m_shortcut->setKeySequence(QKeySequence(t));
}
EditScriptDialog::EditScriptDialog(QWidget  * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
  m_scripts = new QComboBox;

  QStringList scripts;
  UcdScripts::getScripts(scripts);
  m_scripts->addItems(scripts);
  m_font = new QLineEdit;
  QHBoxLayout * hlayout = new QHBoxLayout;
  QPushButton * fontButton = new QPushButton(tr("Set font"));
  hlayout->addWidget(m_font);
  hlayout->addWidget(fontButton);
  QFormLayout * flayout = new QFormLayout;
  flayout->addRow(tr("Script"),m_scripts);
  flayout->addRow(tr("Font"),hlayout);

  layout->addLayout(flayout);
  layout->addWidget(buttonBox);
  layout->addStretch();
  setLayout(layout);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(fontButton,SIGNAL(clicked()),this,SLOT(onFont()));
}
void EditScriptDialog::onFont() {
  QFont f;
  if (!m_font->text().isEmpty()) {
    f.fromString(m_font->text());
  }
  QFontDialog d(this);
  d.setCurrentFont(f);
  if (d.exec() == QDialog::Accepted) {
    f = d.selectedFont();
  }
  else {
    return;
  }
  m_font->setText(f.toString());
}
QString EditScriptDialog::fontString() const {
  return m_font->text();
}
QString EditScriptDialog::script() const {
  return m_scripts->currentText();
}
void EditScriptDialog::setValues(const QString & script,const QString & font) {
  m_scripts->setCurrentText(script);
  m_font->setText(font);
}
QSize EditScriptDialog::sizeHint() const {
  return QSize(400,100);
}
/**
 *
 *
 * @param parent
 */
ImEditorOptions::ImEditorOptions(QWidget * parent) : QDialog(parent) {
  setWindowTitle(tr("Editor Options"));
  QVBoxLayout * layout = new QVBoxLayout;

  m_maps = new QTableWidget(this);

  m_addMapButton = new QPushButton(tr("Add"));
  m_editMapButton = new QPushButton(tr("Edit"));
  m_deleteMapButton = new QPushButton(tr("Delete"));
  m_addScriptButton = new QPushButton(tr("Add"));
  m_editScriptButton = new QPushButton(tr("Edit"));
  m_deleteScriptButton = new QPushButton(tr("Delete"));

  m_scripts = new QTableWidget(this);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_addMapButton,SIGNAL(clicked()),this,SLOT(onAddMap()));
  connect(m_editMapButton,SIGNAL(clicked()),this,SLOT(onEditMap()));
  connect(m_deleteMapButton,SIGNAL(clicked()),this,SLOT(onDeleteMap()));
  connect(m_addScriptButton,SIGNAL(clicked()),this,SLOT(onAddScript()));
  connect(m_editScriptButton,SIGNAL(clicked()),this,SLOT(onEditScript()));
  connect(m_deleteScriptButton,SIGNAL(clicked()),this,SLOT(onDeleteScript()));

  m_insertChar = new QKeySequenceEdit;
  m_keymapOff = new QKeySequenceEdit;
  m_keymapHelp = new QKeySequenceEdit;
  m_insertBlock = new QKeySequenceEdit;
  m_showToolbar = new QCheckBox;
  m_promptFont = new QCheckBox;
  m_pdfPrint = new QCheckBox;
  readSettings();

  QGroupBox * mapgroup = new QGroupBox(tr("Keymaps"),this);
  QVBoxLayout * maplayout = new QVBoxLayout;
  QVBoxLayout * mapbtnlayout = new QVBoxLayout;
  QHBoxLayout * maphlayout = new QHBoxLayout;
  mapbtnlayout->addWidget(m_addMapButton);
  mapbtnlayout->addWidget(m_editMapButton);
  mapbtnlayout->addWidget(m_deleteMapButton);
  mapbtnlayout->addStretch();

  maphlayout->addWidget(m_maps);
  maphlayout->addLayout(mapbtnlayout);
  maplayout->addLayout(maphlayout);
  maplayout->addStretch();
  mapgroup->setLayout(maplayout);

  layout->addWidget(mapgroup);

  QGroupBox * scriptgroup = new QGroupBox(tr("Script fonts"),this);
  QVBoxLayout * scriptlayout = new QVBoxLayout;
  QVBoxLayout * scriptbtnlayout = new QVBoxLayout;
  QHBoxLayout * scripthlayout = new QHBoxLayout;
  scriptbtnlayout->addWidget(m_addScriptButton);
  scriptbtnlayout->addWidget(m_editScriptButton);
  scriptbtnlayout->addWidget(m_deleteScriptButton);
  scriptbtnlayout->addStretch();
  scripthlayout->addWidget(m_scripts);
  scripthlayout->addLayout(scriptbtnlayout);

  scriptlayout->addLayout(scripthlayout);
  scriptlayout->addStretch();
  scriptgroup->setLayout(scriptlayout);

  layout->addWidget(scriptgroup);

  QGroupBox * othergroup = new QGroupBox(tr("Other"),this);
  QFormLayout * otherlayout = new QFormLayout;

  otherlayout->addRow(tr("Print to pdf"),m_pdfPrint);
  otherlayout->addRow(tr("Keymap off"),m_keymapOff);
  otherlayout->addRow(tr("Keymap help"),m_keymapHelp);
  otherlayout->addRow(tr("Insert character"),m_insertChar);
  otherlayout->addRow(tr("Insert block"),m_insertBlock);
  otherlayout->addRow(tr("Show toolbar"),m_showToolbar);
  otherlayout->addRow(tr("Prompt for font"),m_promptFont);
  othergroup->setLayout(otherlayout);

  layout->addWidget(othergroup);
  layout->addWidget(buttonBox);
  //  layout->addStretch();
  setLayout(layout);
  m_maps->resizeColumnsToContents();
  m_scripts->resizeColumnsToContents();
  connect(m_maps,SIGNAL(itemSelectionChanged()),this,SLOT(mapSelectionChanged()));
}
void ImEditorOptions::readSettings() {
  QString str;
  QStringList keys;
  QString settingsFileName("editor.ini");
#ifdef LANE
  settingsFileName = getLexicon()->editorSettingsFileName();
#endif
  QSettings settings(settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("Options");
  QSize sz = settings.value("Size",QSize()).toSize();
  if (! sz.isNull()) {
    this->resize(sz);
  }
  QPoint p = settings.value("Position",QPoint()).toPoint();
  if (! p.isNull()) {
    this->move(p);
  }
  settings.endGroup();
  settings.beginGroup("System");

  m_promptFont->setChecked(settings.value("Prompt font",false).toBool());
  m_showToolbar->setChecked(settings.value("Toolbar",true).toBool());
  m_pdfPrint->setChecked(settings.value("PDF print",false).toBool());
  m_keymapOff->setKeySequence(QKeySequence(settings.value("Keymap off","Ctrl+K,D").toString()));
  m_keymapHelp->setKeySequence(QKeySequence(settings.value("Keymap help","Ctrl+K,H").toString()));
  m_insertChar->setKeySequence(QKeySequence(settings.value("Insert char","Ctrl+U,I").toString()));
  m_insertBlock->setKeySequence(QKeySequence(settings.value("Insert block","Ctrl+J,B").toString()));

  settings.endGroup();

  settings.beginGroup("Maps");
  QStringList maps = settings.childGroups();
  QString mapfile;
  QString shortcut;
  QString helpfile;
  m_maps->setRowCount(maps.size());
  m_maps->setColumnCount(3);
  m_maps->setHorizontalHeaderLabels(QStringList() << tr("File") << tr("Help File") << tr("Shorcut"));
  m_maps->horizontalHeader()->setStretchLastSection(true);
  m_maps->setSelectionMode(QAbstractItemView::SingleSelection);
  m_maps->setSelectionBehavior(QAbstractItemView::SelectRows);
  for(int i=0;i < maps.size();i++) {
    settings.beginGroup(maps[i]);
    mapfile = settings.value("File",QString()).toString();
    shortcut = settings.value("Shortcut",QString()).toString();
    helpfile = settings.value("Help",QString()).toString();
    m_maps->setItem(i,0,new QTableWidgetItem(mapfile));
    m_maps->setItem(i,1,new QTableWidgetItem(helpfile));
    m_maps->setItem(i,2,new QTableWidgetItem(shortcut));
    settings.endGroup();
  }
  settings.endGroup();

  settings.beginGroup("Scripts");
  QStringList scripts = settings.childKeys();
  m_scripts->setRowCount(scripts.size());
  m_scripts->setColumnCount(2);
  m_scripts->setHorizontalHeaderLabels(QStringList() << tr("Script") << tr("Font"));
  m_scripts->horizontalHeader()->setStretchLastSection(true);
  m_scripts->setSelectionMode(QAbstractItemView::SingleSelection);
  m_scripts->setSelectionBehavior(QAbstractItemView::SelectRows);
  for(int i=0;i < scripts.size();i++) {
    m_scripts->setItem(i,0,new QTableWidgetItem(scripts[i]));
    m_scripts->setItem(i,1,new QTableWidgetItem(settings.value(scripts[i],QString()).toString()));
  }
  settings.endGroup();

}
void ImEditorOptions::writeSettings() {
  QString str;
  QStringList keys;

  QString settingsFileName("editor.ini");
#ifdef LANE
  settingsFileName = getLexicon()->editorSettingsFileName();
#endif
  QSettings settings(settingsFileName,QSettings::IniFormat);



  settings.setIniCodec("UTF-8");
  settings.beginGroup("System");

  settings.setValue("Prompt font",m_promptFont->isChecked());
  settings.setValue("Toolbar",m_showToolbar->isChecked());
  settings.setValue("PDF print",m_pdfPrint->isChecked());
  settings.setValue("Keymap off",m_keymapOff->keySequence().toString());
  settings.setValue("Keymap help",m_keymapHelp->keySequence().toString());
  settings.setValue("Insert char",m_insertChar->keySequence().toString());
  settings.setValue("Insert block",m_insertBlock->keySequence().toString());



  settings.endGroup();
  settings.beginGroup("Options");
  settings.setValue("Size",this->size());
  settings.setValue("Position",this->pos());
  settings.endGroup();

  settings.beginGroup("Maps");
  settings.remove("");

  QString mapfile;
  QString shortcut;
  QString helpfile;
  InputMapper * im = im_new();
  for(int i=0;i < m_maps->rowCount();i++) {
    mapfile = m_maps->item(i,0)->text();
    int ix = im_load_map_from_json(im,mapfile.toLocal8Bit().constData());
    KeyMap * map = im_get_map(im,ix);
    QString mapname = map->getName();
    settings.beginGroup(mapname);
    settings.setValue("File",m_maps->item(i,0)->text());
    settings.setValue("Help",m_maps->item(i,1)->text());
    settings.setValue("Shortcut",m_maps->item(i,2)->text());
    settings.endGroup();
  }
  settings.endGroup();

  settings.beginGroup("Scripts");
  settings.remove("");

  for(int i=0;i < m_scripts->rowCount();i++) {
    settings.setValue(m_scripts->item(i,0)->text(),m_scripts->item(i,1)->text());
  }
  settings.endGroup();

  im_free(im);
}
QSize ImEditorOptions::sizeHint() const {
  return QSize(500,600);
}
void ImEditorOptions::onAddMap() {
  EditMapDialog w;
  if (w.exec() != QDialog::Accepted) {
    return;
  }


  if (w.mapfile().isEmpty()) {
    QMessageBox::critical(this, tr("Add Map"),
                         tr("No map file specified"),
                         QMessageBox::Cancel);
    return;
  }
  int row = m_maps->rowCount();
  m_maps->setRowCount(row+1);
  QDir dir = QDir::current();
  QFileInfo mfi(w.mapfile());
  QString str  = dir.relativeFilePath(mfi.absoluteFilePath());
  QTableWidgetItem * item0 = new QTableWidgetItem(str);
  QFileInfo hfi(w.helpfile());

  str  = dir.relativeFilePath(hfi.absoluteFilePath());
  QTableWidgetItem * item1 = new QTableWidgetItem(str);

  QTableWidgetItem * item2 = new QTableWidgetItem(w.keySequence());

  m_maps->setItem(row,0,item0);
  m_maps->setItem(row,1,item1);
  m_maps->setItem(row,2,item2);
}
void ImEditorOptions::onEditMap() {
  QList<QTableWidgetItem *> items = m_maps->selectedItems();
  if (items.size() != 3) {
    return;
  }
  EditMapDialog w;
  w.setFile(items[0]->text());
  w.setHelp(items[1]->text());
  w.setMapShortcut(items[2]->text());
  if (w.exec() != QDialog::Accepted) {
    return;
  }
  QDir dir = QDir::current();
  QFileInfo mfi(w.mapfile());
  QString str  = dir.relativeFilePath(mfi.absoluteFilePath());
  items[0]->setText(str);

  QFileInfo hfi(w.helpfile());
  str  = dir.relativeFilePath(hfi.absoluteFilePath());
  items[1]->setText(str);

  items[2]->setText(w.keySequence());
}
void ImEditorOptions::onDeleteMap() {

  QList<QTableWidgetItem *> items = m_maps->selectedItems();
  if (items.size() != 3) {
    return;
  }
  m_maps->removeRow(items[0]->row());
}
void ImEditorOptions::mapSelectionChanged() {
  //  QList<QTableWidgetItem *> items = m_maps->selectedItems();
}
void ImEditorOptions::scriptSelectionChanged() {
}
void ImEditorOptions::onAddScript() {
  EditScriptDialog w;
  if (w.exec() != QDialog::Accepted) {
    return;
  }
  if (w.script().isEmpty()) {
    QMessageBox::critical(this, tr("Add Script"),
                         tr("No script specified"),
                         QMessageBox::Cancel);
    return;
  }
  int row = m_scripts->rowCount();
  m_scripts->setRowCount(row+1);
  m_scripts->setItem(row,0,new QTableWidgetItem(w.script()));
  m_scripts->setItem(row,1,new QTableWidgetItem(w.fontString()));

}
void ImEditorOptions::onEditScript() {
  QList<QTableWidgetItem *> items = m_scripts->selectedItems();
  if (items.size() != 2) {
    return;
  }
  EditScriptDialog w;
  w.setValues(items[0]->text(),items[1]->text());
  if (w.exec() != QDialog::Accepted) {
    return;
  }
  items[0]->setText(w.script());
  items[1]->setText(w.fontString());
}
void ImEditorOptions::onDeleteScript() {
  QList<QTableWidgetItem *> items = m_scripts->selectedItems();
  if (items.size() != 2) {
    return;
  }
  m_scripts->removeRow(items[0]->row());
}
