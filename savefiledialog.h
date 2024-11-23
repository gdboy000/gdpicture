//
// Created by gao on 24-11-23.
//

#ifndef SAVEFILEDIALOG_H
#define SAVEFILEDIALOG_H

#include <QDialog>

class QFileDialog;


QT_BEGIN_NAMESPACE
namespace Ui { class SaveFileDialog; }
QT_END_NAMESPACE

class SaveFileDialog : public QDialog {
Q_OBJECT

public:
    explicit SaveFileDialog(QWidget *parent = nullptr);
    ~SaveFileDialog() override;
    void setDefaultPath(const QString&,const QString&,const QString&);
    void setDefaultPath(const QString&);
    void setResultMessage(const QString& text){_updateLine(text);}
private:
    Ui::SaveFileDialog *_ui;
    QFileDialog* _fileDialog;
    QString _path;
    QString _fileName;
    QString _postfix;

    void _initConnect();
    void _updateLine(const QString&);
signals:
    void pathChanged(const QString &);
    void typeChanged(const QString &);
    void savePathChanged(const QString &);
};


#endif //SAVEFILEDIALOG_H
