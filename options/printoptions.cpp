#include "printoptions.h"
#include "definedsettings.h"
#ifndef STANDALONE
#include "application.h"
#include "externs.h"
#endif
PrintOptions::PrintOptions(const QString & theme,QWidget * parent) : OptionsWidget(theme,parent) {
  /*
#ifdef STANDALONE
  m_settings = new QSettings("default.ini",QSettings::IniFormat);
  m_settings->setIniCodec("UTF-8");
#else
  m_settings = (qobject_cast<Lexicon *>(qApp))->getSettings();
#endif
  */
  m_section = "Printer";

  m_dialogButton  = new QPushButton(tr("Click to change print options"),this);
  connect(m_dialogButton,SIGNAL(clicked()),this,SLOT(onPrintDialog()));

  QHBoxLayout * printbtnlayout = new QHBoxLayout;
  printbtnlayout->addWidget(m_dialogButton);
  printbtnlayout->addStretch();

  m_directoryButton = new QPushButton(tr("Select directory"),this);
  connect(m_directoryButton,SIGNAL(clicked()),this,SLOT(onDirectory()));

  m_alwaysUse = new QCheckBox(this);
  m_fullPage = new QCheckBox(this);
  m_pdfOutput = new QCheckBox(this);


  m_pdfName = new QComboBox(this);

  m_namingMethods << "arabic";
  m_namingMethods << "node";;
  m_namingMethods << "date";

  m_pdfName->addItem(tr("Arabic word"),m_namingMethods[0]);
  m_pdfName->addItem(tr("Node name"),m_namingMethods[1]);
  m_pdfName->addItem(tr("Date/time"),m_namingMethods[2]);

  m_pdfAutoName= new QCheckBox(this);

  m_copyCount = new QLineEdit(this);
  m_printerName = new QLineEdit(this);
  m_resolution = new QLineEdit(this);
  m_paperSize = new QLineEdit(this);
  m_orientation = new QLineEdit(this);
  m_pdfDirectory = new QLineEdit(this);

  /// disable since they are set via the PrintDialog
  m_copyCount->setReadOnly(true);
  m_printerName->setReadOnly(true);
  m_resolution->setReadOnly(true);
  m_paperSize->setReadOnly(true);
  m_fullPage->setEnabled(false);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_pdfDirectory);
  hlayout->addWidget(m_directoryButton);
  hlayout->addStretch();

  QVBoxLayout * vlayout = new QVBoxLayout;


  QGroupBox * printerbox = new QGroupBox(tr("Printer properties"));
  QFormLayout * printerlayout = new QFormLayout;

  printerlayout->addRow("",printbtnlayout);
  printerlayout->addRow(tr("Name"),m_printerName);
  printerlayout->addRow(tr("Orientation"),m_orientation);
  printerlayout->addRow(tr("Paper size"),m_paperSize);
  printerlayout->addRow(tr("Resolution"),m_resolution);
  printerlayout->addRow(tr("Copies"),m_copyCount);
  printerlayout->addRow(tr("Full page"),m_fullPage);
  printerbox->setLayout(printerlayout);

  QGroupBox * usagebox = new QGroupBox(tr("PDF options"));
  QFormLayout * formlayout = new QFormLayout;

  formlayout->addRow(tr("PDF output"),m_pdfOutput);
  formlayout->addRow(tr("PDF directory"),hlayout);
  formlayout->addRow(tr("Auto name PDF"),m_pdfAutoName);
  formlayout->addRow(tr("Naming method"),m_pdfName);

  usagebox->setLayout(formlayout);

  QFormLayout * uselayout = new QFormLayout;
  uselayout->addRow(tr("Use these settings and do not prompt for printer settings"),m_alwaysUse);
  vlayout->addWidget(printerbox);
  vlayout->addWidget(usagebox);
  vlayout->addLayout(uselayout);
  setLayout(vlayout);
  addButtons();
  setupPaperSize();
  readSettings();
  setupConnections();
}
void PrintOptions::onPrintDialog() {
  QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
  if (printers.size() == 0) {
    QMessageBox msgBox;
    msgBox.setText(tr("No printers are available."));
    msgBox.setInformativeText(tr("All printing will be to PDF"));
    msgBox.exec();
    m_pdfOutput->setChecked(true);
    return;
  }
  QPrinter * p = new QPrinter();
  QPrintDialog * d = new QPrintDialog(p,this);
  d->setWindowModality(Qt::WindowModal);
  d->setWindowTitle("Set and Configure Printer");
  d->open(this,SLOT(onPrinterSetup()));


}
void PrintOptions::onPrinterSetup() {
  QString t;
  QPrintDialog * d = qobject_cast<QPrintDialog *>(sender());
  if (! d ) {
    return;
  }
  if (d->result() == QDialog::Rejected) {
    return;
  }
  QPrinter * printer = d->printer();
    if (printer->outputFormat() != QPrinter::PdfFormat) {
    m_copyCount->setText(QString("%1").arg(printer->copyCount()));
    m_resolution->setText(QString("%1").arg(printer->resolution()));
    if (m_paper.contains(printer->paperSize())) {
      t = m_paper.value(printer->paperSize());
      m_paperSize->setText(t);
      m_papersz = printer->paperSize();
    }
    else {
      m_paperSize->setText(QString("%1").arg(printer->paperSize()));
    }
    m_orientationNum = printer->orientation();
    if (m_orientationNum == QPrinter::Portrait) {
      m_orientation->setText("Portrait");
    }
    else {
      m_orientation->setText("Landscape");
    }
    m_printerName->setText(QString("%1").arg(printer->printerName()));
    m_fullPage->setChecked(printer->fullPage());
    }
    if (printer->outputFormat() == QPrinter::PdfFormat) {
      m_pdfOutput->setChecked(true);
    }
    else {
      m_pdfOutput->setChecked(false);
    }
    delete printer;
}
void PrintOptions::readSettings() {
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  m_printerName->setText(settings.value(SID_PRINTER_NAME).toString());
  m_orientation->setText(settings.value(SID_PRINTER_ORIENTATION).toString());
  m_resolution->setText(settings.value(SID_PRINTER_RESOLUTION).toString());
  m_paperSize->setText(settings.value(SID_PRINTER_PAPER_SIZE).toString());
  m_copyCount->setText(settings.value(SID_PRINTER_COPIES).toString());
  m_fullPage->setChecked(settings.value(SID_PRINTER_FULL_PAGE).toBool());
  m_alwaysUse->setChecked(settings.value(SID_PRINTER_USE).toBool());
  m_pdfOutput->setChecked(settings.value(SID_PRINTER_OUTPUT_PDF).toBool());
  m_pdfDirectory->setText(settings.value(SID_PRINTER_PDF_DIRECTORY).toString());
  m_pdfAutoName->setChecked(settings.value(SID_PRINTER_AUTONAME_PDF).toBool());
  int ix = m_pdfName->findData(settings.value(SID_PRINTER_AUTONAME_METHOD).toString());
  if (ix == -1 ) {
    ix = 1;
  }
  m_pdfName->setCurrentIndex(ix);


  settings.endGroup();
}
void PrintOptions::writeSettings(const QString & fileName) {
  QString f = m_settingsFileName;
  if (!fileName.isEmpty()) {
    f = fileName;
  }
  QSettings settings(f,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  settings.setValue(SID_PRINTER_NAME,m_printerName->text());
  settings.setValue(SID_PRINTER_ORIENTATION,m_orientation->text());
  settings.setValue(SID_PRINTER_RESOLUTION,m_resolution->text());
  settings.setValue(SID_PRINTER_PAPER_SIZE,m_paperSize->text());
  settings.setValue(SID_PRINTER_COPIES,m_copyCount->text());
  settings.setValue(SID_PRINTER_FULL_PAGE,m_fullPage->isChecked());
  settings.setValue(SID_PRINTER_USE,m_alwaysUse->isChecked());
  settings.setValue(SID_PRINTER_OUTPUT_PDF,m_pdfOutput->isChecked());
  settings.setValue(SID_PRINTER_PDF_DIRECTORY,m_pdfDirectory->text());
  settings.setValue(SID_PRINTER_AUTONAME_PDF,m_pdfAutoName->isChecked());

  settings.setValue(SID_PRINTER_AUTONAME_METHOD,m_pdfName->currentData().toString());
  settings.sync();

  settings.endGroup();
  emit(modified(false));
}
bool PrintOptions::isModified()  {
  m_dirty = false;
  QSettings settings(m_settingsFileName,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(m_section);

  if (m_printerName->text() != settings.value(SID_PRINTER_NAME).toString()) {
    m_dirty = true;
  }
  if (m_orientation->text() != settings.value(SID_PRINTER_ORIENTATION).toString()) {
    m_dirty = true;
  };
  if (m_resolution->text() != settings.value(SID_PRINTER_RESOLUTION).toString()) {
    m_dirty = true;
  }
  if (m_paperSize->text() != settings.value(SID_PRINTER_PAPER_SIZE).toString()) {
    m_dirty = true;
  }
  if (m_copyCount->text() != settings.value(SID_PRINTER_COPIES).toString()) {
    m_dirty = true;
  }
  if (m_fullPage->isChecked() != settings.value(SID_PRINTER_FULL_PAGE).toBool()) {
    m_dirty = true;
  }
  if (m_alwaysUse->isChecked() != settings.value(SID_PRINTER_USE).toBool()) {
    m_dirty = true;
  }
  if (m_pdfOutput->isChecked() != settings.value(SID_PRINTER_OUTPUT_PDF).toBool()) {
    m_dirty = true;
  }
  if (m_pdfDirectory->text() != settings.value(SID_PRINTER_PDF_DIRECTORY).toString()) {
    m_dirty = true;
  }
  if (m_pdfAutoName->isChecked() != settings.value(SID_PRINTER_AUTONAME_PDF).toBool()) {
    m_dirty = true;
  }

  if (m_pdfName->currentData().toString() != settings.value(SID_PRINTER_AUTONAME_METHOD).toString()) {
    m_dirty = true;
  }
  settings.endGroup();
  return m_dirty;
}
void PrintOptions::onDirectory() {
  QString directoryName = QFileDialog::getExistingDirectory();
  m_pdfDirectory->setText(directoryName);
}
void PrintOptions::setupPaperSize() {
  m_paper.insert(0,"A4");
  m_paper.insert(1,"B5");
  m_paper.insert(2,"Letter");
  m_paper.insert(3,"Legal");
  m_paper.insert(4,"Executive");
  m_paper.insert(5,"A0");
  m_paper.insert(6,"A1");
  m_paper.insert(7,"A2");
  m_paper.insert(8,"A3");
  m_paper.insert(9,"A5");
  m_paper.insert(10,"A6");
  m_paper.insert(11,"A7");
  m_paper.insert(12,"A8");
  m_paper.insert(13,"A9");
  m_paper.insert(14,"B0");
  m_paper.insert(15,"B1");
  m_paper.insert(16,"B10");
  m_paper.insert(17,"B2");
  m_paper.insert(18,"B3");
  m_paper.insert(19,"B4");
  m_paper.insert(20,"B6");
  m_paper.insert(21,"B7");
  m_paper.insert(22,"B8");
  m_paper.insert(23,"B9");
  m_paper.insert(24,"C5E");
  m_paper.insert(25,"Comm10E");
  m_paper.insert(26,"DLE");
  m_paper.insert(27,"Folio");
  m_paper.insert(28,"Ledger");
  m_paper.insert(29,"Tabloid");
  m_paper.insert(30,"Custom");
  m_paper.insert(31,"A10");
  m_paper.insert(32,"A3Extra");
  m_paper.insert(33,"A4Extra");
  m_paper.insert(34,"A4Plus");
  m_paper.insert(35,"A4Small");
  m_paper.insert(36,"A5Extra");
  m_paper.insert(37,"B5Extra");
  m_paper.insert(38,"JisB0");
  m_paper.insert(39,"JisB1");
  m_paper.insert(40,"JisB2");
  m_paper.insert(41,"JisB3");
  m_paper.insert(42,"JisB4");
  m_paper.insert(43,"JisB5");
  m_paper.insert(44,"JisB6");
  m_paper.insert(45,"JisB7");
  m_paper.insert(46,"JisB8");
  m_paper.insert(47,"JisB9");
  m_paper.insert(48,"JisB10");
  m_paper.insert(49,"AnsiC");
  m_paper.insert(50,"AnsiD");
  m_paper.insert(51,"AnsiE");
  m_paper.insert(52,"LegalExtra");
  m_paper.insert(53,"LetterExtra");
  m_paper.insert(54,"LetterPlus");
  m_paper.insert(55,"LetterSmall");
  m_paper.insert(56,"TabloidExtra");
  m_paper.insert(57,"ArchA");
  m_paper.insert(58,"ArchB");
  m_paper.insert(59,"ArchC");
  m_paper.insert(60,"ArchD");
  m_paper.insert(61,"ArchE");
  m_paper.insert(62,"Imperial7x9");
  m_paper.insert(63,"Imperial8x10");
  m_paper.insert(64,"Imperial9x11");
  m_paper.insert(65,"Imperial9x12");
  m_paper.insert(66,"Imperial10x11");
  m_paper.insert(67,"Imperial10x13");
  m_paper.insert(68,"Imperial10x14");
  m_paper.insert(69,"Imperial12x11");
  m_paper.insert(70,"Imperial15x11");
  m_paper.insert(71,"ExecutiveStandard");
  m_paper.insert(72,"Note");
  m_paper.insert(73,"Quarto");
  m_paper.insert(74,"Statement");
  m_paper.insert(75,"SuperA");
  m_paper.insert(76,"SuperB");
  m_paper.insert(77,"Postcard");
  m_paper.insert(78,"DoublePostcard");
  m_paper.insert(79,"Prc16K");
  m_paper.insert(80,"Prc32K");
  m_paper.insert(81,"Prc32KBig");
  m_paper.insert(82,"FanFoldUS");
  m_paper.insert(83,"FanFoldGerman");
  m_paper.insert(84,"FanFoldGermanLegal");
  m_paper.insert(85,"EnvelopeB4");
  m_paper.insert(86,"EnvelopeB5");
  m_paper.insert(87,"EnvelopeB6");
  m_paper.insert(88,"EnvelopeC0");
  m_paper.insert(89,"EnvelopeC1");
  m_paper.insert(90,"EnvelopeC2");
  m_paper.insert(91,"EnvelopeC3");
  m_paper.insert(92,"EnvelopeC4");
  m_paper.insert(93,"EnvelopeC6");
  m_paper.insert(94,"EnvelopeC65");
  m_paper.insert(95,"EnvelopeC7");
  m_paper.insert(96,"Envelope9");
  m_paper.insert(97,"Envelope11");
  m_paper.insert(98,"Envelope12");
  m_paper.insert(99,"Envelope14");
  m_paper.insert(100,"EnvelopeMonarch");
  m_paper.insert(101,"EnvelopePersonal");
  m_paper.insert(102,"EnvelopeChou3");
  m_paper.insert(103,"EnvelopeChou4");
  m_paper.insert(104,"EnvelopeInvite");
  m_paper.insert(105,"EnvelopeItalian");
  m_paper.insert(106,"EnvelopeKaku2");
  m_paper.insert(107,"EnvelopeKaku3");
  m_paper.insert(108,"EnvelopePrc1");
  m_paper.insert(109,"EnvelopePrc2");
  m_paper.insert(110,"EnvelopePrc3");
  m_paper.insert(111,"EnvelopePrc4");
  m_paper.insert(112,"EnvelopePrc5");
  m_paper.insert(113,"EnvelopePrc6");
  m_paper.insert(114,"EnvelopePrc7");
  m_paper.insert(115,"EnvelopePrc8");
  m_paper.insert(116,"EnvelopePrc9");
  m_paper.insert(117,"EnvelopePrc10");
  m_paper.insert(118,"EnvelopeYou4");
  m_paper.insert(118,"LastPageSize");
}
