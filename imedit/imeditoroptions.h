#ifndef _IMEDITOROPTIONS_H_
#define _IMEDITOROPTIONS_H_
#include <QtCore>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QTableWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QSettings>
#include <QDialog>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QFont>
#include <QFontDialog>
#include <QComboBox>
#include <QCheckBox>
class EditMapDialog : public QDialog {
  Q_OBJECT

 public:
  EditMapDialog(QWidget * parent = 0);
  QString mapfile() const;
  QString helpfile() const;
  QString keySequence() const;
  void setFile(const QString &);
  void setHelp(const QString &);
  void setMapShortcut(const QString &);

  public slots:
    void onMapFile();
    void onHelpFile();
 protected:
  QSize sizeHint() const;
 private:
  QLineEdit * m_file;
  QLineEdit * m_help;
  QKeySequenceEdit * m_shortcut;

};
class EditScriptDialog : public QDialog {
  Q_OBJECT

 public:
  EditScriptDialog(QWidget * parent = 0);
  QString script() const;
  QString fontString() const;
  void setValues(const QString &,const QString &);
  public slots:
    void onFont();
 protected:
  QSize sizeHint() const;

 private:
  QComboBox * m_scripts;
  QLineEdit * m_font;
};
class ImEditorOptions : public QDialog {
  Q_OBJECT

 public:
  ImEditorOptions(QWidget * parent = 0);
  void writeSettings();
  public slots:
    void onAddMap();
    void onEditMap();
    void onDeleteMap();
    void onAddScript();
    void onEditScript();
    void onDeleteScript();
    void scriptSelectionChanged();
    void mapSelectionChanged();
 protected:
  QSize sizeHint() const;

 private:
  QTableWidget * m_maps;
  QTableWidget * m_scripts;
  QKeySequenceEdit * m_insertChar;
  QKeySequenceEdit * m_keymapOff;
  QKeySequenceEdit * m_keymapHelp;
  QKeySequenceEdit * m_insertBlock;
  QCheckBox * m_showToolbar;
  QCheckBox * m_promptFont;
  QCheckBox * m_pdfPrint;
  QPushButton * m_addMapButton;
  QPushButton * m_editMapButton;
  QPushButton * m_deleteMapButton;
  QPushButton * m_addScriptButton;
  QPushButton * m_editScriptButton;
  QPushButton * m_deleteScriptButton;
  void readSettings();
};
#endif
