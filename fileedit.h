#ifndef FILEEDIT_H
#define FILEEDIT_H
#include <QTextEdit>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
//a custom File edit class to allow Drag&drop functionality
class FileEdit : public QTextEdit
{
    Q_OBJECT
public:
    FileEdit(QWidget* parent = nullptr);


    void dragEnterEvent(QDragEnterEvent * event);
    void dropEvent(QDropEvent * event);

};

#endif // FILEEDIT_H
