#include "fileedit.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
FileEdit::FileEdit(QWidget* parent) : QTextEdit(parent)
{

}

void FileEdit::dragEnterEvent(QDragEnterEvent * event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}
void FileEdit::dropEvent(QDropEvent * event)
{
    const QMimeData* mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        QString filePath = urlList.first().toLocalFile();
        setText(filePath);
    }
}
