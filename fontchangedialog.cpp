#include "fontchangedialog.h"
#include "arabicfontdialog.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
#define FONT_SIZE_WIDTH 40
#define LEAD_SPACING 50
#define SPACING 20
FontChangeDialog::FontChangeDialog(QWidget * parent) : QDialog(parent) {
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;

  QFormLayout * formlayout = new QFormLayout;
  formlayout->setHorizontalSpacing(30);
  formlayout->addRow(new QLabel(tr("Lexicon font name and size")));


  m_lexiconArabicFontName = new QLineEdit;

  m_lexiconArabicFontSize = new QLineEdit;
  m_lexiconArabicFontSize->setMaximumWidth(FONT_SIZE_WIDTH);

  m_lexiconEnglishFontName = new QLineEdit;

  m_lexiconEnglishFontSize = new QLineEdit;
  m_lexiconEnglishFontSize->setMaximumWidth(FONT_SIZE_WIDTH);
  m_appArabicFontName = new QLineEdit;

  m_appArabicFontSize = new QLineEdit;
  m_appArabicFontSize->setMaximumWidth(FONT_SIZE_WIDTH);

  m_appNativeFontName = new QLineEdit;

  m_appNativeFontSize = new QLineEdit;

  m_appNativeFontSize->setMaximumWidth(FONT_SIZE_WIDTH);

  QHBoxLayout * hlayout1 = new QHBoxLayout;
  hlayout1->addSpacing(LEAD_SPACING);
  hlayout1->addWidget(m_lexiconArabicFontName);
  hlayout1->addSpacing(SPACING);
  hlayout1->addWidget(m_lexiconArabicFontSize);
  hlayout1->addSpacing(SPACING);
  QPushButton * button1 = new QPushButton(tr("Choose font"));
  hlayout1->addWidget(button1);

  QHBoxLayout * hlayout2 = new QHBoxLayout;
  hlayout2->addSpacing(LEAD_SPACING);
  hlayout2->addWidget(m_lexiconEnglishFontName);
  hlayout2->addSpacing(SPACING);
  hlayout2->addWidget(m_lexiconEnglishFontSize);
  hlayout2->addSpacing(SPACING);
  QPushButton * button2 = new QPushButton(tr("Choose font"));
  hlayout2->addWidget(button2);



  formlayout->addRow(tr("Arabic"),hlayout1);
  formlayout->addRow(tr("English"),hlayout2);
  connect(button1,SIGNAL(clicked()),this,SLOT(lexiconArabicFontDialog()));
  connect(button2,SIGNAL(clicked()),this,SLOT(lexiconEnglishFontDialog()));



  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

  layout->addLayout(formlayout);
  layout->addWidget(buttonBox);
  setLayout(layout);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QFontDatabase fdb;
  m_arFonts = fdb.families(QFontDatabase::Arabic);
}
void FontChangeDialog::lexiconArabicFontDialog() {
   QFont f;
  ArabicFontDialog * d = new ArabicFontDialog(QFont("Amiri", 14), this);
  if (d->exec()) {
    f = d->selectedFont();
  }
  delete d;
  int sz = f.pixelSize();
  if (sz == -1)
    sz = f.pointSize();

  m_lexiconArabicFontName->setText(f.family());
  m_lexiconArabicFontSize->setText(QString("%1").arg(sz));

}
void FontChangeDialog::lexiconEnglishFontDialog() {
   QFont f;
  QFontDialog * d = new QFontDialog(QFont("Courier", 14), this);
  if (d->exec()) {
    f = d->selectedFont();
  }
  delete d;
  int sz = f.pixelSize();
  if (sz == -1)
    sz = f.pointSize();

  m_lexiconEnglishFontName->setText(f.family());
  m_lexiconEnglishFontSize->setText(QString("%1").arg(sz));

}
void FontChangeDialog::appArabicFontDialog() {
}
void FontChangeDialog::appNativeFontDialog(){
}

QSize FontChangeDialog::sizeHint() const {
  return QSize(600,400);
}
void FontChangeDialog::readSettings() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  QStringList r;
  SETTINGS
  QStringList keys = settings.allKeys();
  qDebug() << keys;
  QString v;
  QRegularExpression re("amiri", QRegularExpression::CaseInsensitiveOption);
  for(int i=0;i < keys.size();i++) {
    v = settings.value(keys[i],QString()).toString();
    if (!v.isEmpty()) {
      if (re.match(v).hasMatch()) {
        qDebug() << keys[i] << v;
      }
    }
  }
  settings.beginGroup("Entry");
  //  bool debug = settings.value(SID_ENTRY_DEBUG,false).toBool();
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
  css = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,css);
  r = readFile(css);

  css = settings.value(SID_ENTRY_PRINT_CSS,QString("entry_print.css")).toString();
  css = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,css);
  r = readFile(css);
  settings.endGroup();
  settings.beginGroup("XSLT");
  //  m_entryXslt = settings.value(SID_XSLT_ENTRY,QString("entry.xslt")).toString();
}
void FontChangeDialog::writeSettings() {
}
QStringList FontChangeDialog::readFile(const QString & name) {
  QStringList lines;
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QLOG_WARN()  << QString(tr("Cannot open file %1:  %2")).arg(name).arg(f.errorString());
    return lines;
  }
  QTextStream in(&f);
  while(! in.atEnd()) {
    lines << in.readLine();
  }
  f.close();
  return lines;
}
