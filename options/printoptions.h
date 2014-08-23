#ifndef __PRINTOPTIONS_H__
#define __PRINTOPTIONS_H__
#include "optionswidget.h"
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QComboBox>

class PrintOptions : public OptionsWidget {
  Q_OBJECT

 public:
  PrintOptions(QSettings * settings = 0,QWidget * parent = 0);
  bool isModified();
  public slots:
    void writeSettings();
    void readSettings();
    void onPrintDialog();
    void onDirectory();
 private:
    QLineEdit * m_copyCount;
    QLineEdit * m_resolution;
    QLineEdit * m_paperSize;
    QLineEdit * m_orientation;
    QLineEdit * m_printerName;
    QCheckBox * m_fullPage;
    QCheckBox * m_pdfOutput;
    QCheckBox * m_alwaysUse;
    QComboBox * m_pdfName;
    QLineEdit * m_pdfDirectory;
    QPushButton * m_dialogButton;;
    QPushButton * m_directoryButton;
    QPrinter m_printer;
    QCheckBox * m_pdfAutoName;
    QMap<int,QString> m_paper;
    int m_papersz;
    int m_orientationNum;
    void setupPaperSize();
};
#endif
