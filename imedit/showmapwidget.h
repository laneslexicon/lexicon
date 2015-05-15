#ifndef _SHOWMAPWIDGET_H__
#define _SHOWMAPWIDGET_H__
#include <QtCore>
#include <QDialog>
#include <QVBoxLayout>
#include <QSettings>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
class ShowMapWidget : public QDialog {
  Q_OBJECT

 public:
  ShowMapWidget(QWidget * parent = 0);
  ~ShowMapWidget();
  QString map() const;
  void setMapName(const QString &);
 protected:
  void focusOutEvent(QFocusEvent */* event */);

  public slots:
    void loadHtml(const QString & fileName);
 private:
  QTextEdit * m_html;
  QString m_map;
  QDialogButtonBox * m_buttons;
};
#endif
