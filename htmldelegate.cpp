#include "htmldelegate.h"
HtmlDelegate::HtmlDelegate(QObject * parent) : QStyledItemDelegate(parent) {

}
HtmlDelegate::~HtmlDelegate() {

}
void HtmlDelegate::setStyleSheet(const QString & str) {
  m_stylesheet = str;
}
void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QTextDocument document;
  if (m_stylesheet.size() > 0) {
    document.setDefaultStyleSheet(m_stylesheet);
  }
  //  document.setDocumentMargin(2);
  QVariant value = index.data(Qt::DisplayRole);
  if (value.isValid() && !value.isNull()) {
    QStyleOptionViewItem myOption= option;
    QTextOption t = document.defaultTextOption();
    t.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    myOption.displayAlignment = Qt::AlignLeft|Qt::AlignVCenter;
    document.setDefaultTextOption(t);
    QString html = "<body>" + value.toString() + "</body>";
    document.setHtml(html);
    //QTableWidget * table = qobject_cast<QTableWidget *>(this->parent());
    QRectF r = myOption.rect;
    r.setX(0);
    r.setY(0);
    painter->translate(myOption.rect.topLeft());
    document.drawContents(painter,r);
    painter->translate(-myOption.rect.topLeft());
  }
}
