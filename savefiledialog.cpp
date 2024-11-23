//
// Created by gao on 24-11-23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SaveFileDialog.h" resolved

#include "savefiledialog.h"
#include "ui_savefiledialog.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QStandardPaths>


QString toggleCase(const QString &input) {
    QString result;
    for (QChar ch : input) {
        // 如果字符是大写字母，则转换为小写；如果是小写字母，则转换为大写
        if (ch.isUpper()) {
            result.append(ch.toLower());
        } else if (ch.isLower()) {
            result.append(ch.toUpper());
        } else {
            // 如果字符既不是大写也不是小写，保持不变
            result.append(ch);
        }
    }
    return result;
}

SaveFileDialog::SaveFileDialog(QWidget *parent) :
    QDialog(parent), _ui(new Ui::SaveFileDialog) {
    _ui->setupUi(this);
    //JPG JPEG PNG
    QStringList lists;
    lists<<"JPG"<<"JPEG"<<"PNG"<<"BMP";
    _ui->typeBox->addItems(lists);
    _path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);;
    _fileDialog = new QFileDialog(this);
    _fileDialog->setFileMode(QFileDialog::Directory);
    _initConnect();
}

void SaveFileDialog::setDefaultPath(const QString &path) {
    QFileInfo fileInfo(path);
    setDefaultPath(fileInfo.absoluteDir().path(),fileInfo.completeBaseName(),fileInfo.suffix());
}


void SaveFileDialog::setDefaultPath(const QString &path, const QString &fileName, const QString &postfix) {
    _path = path;
    _ui->saveFold->setText(_path);
    _fileName = fileName;
    _ui->filename->setText(_fileName);
    _postfix = postfix;
    _ui->typeBox->setCurrentText(toggleCase(postfix));
    _updateLine(QString("文件地址：%1/%2.%3").arg(_path).arg(_fileName).arg(_postfix));
}


SaveFileDialog::~SaveFileDialog() {
    delete _ui;
}

void SaveFileDialog::_initConnect() {
    connect(_ui->foldBtn,&QPushButton::clicked,[this] {
        _fileDialog->setDirectory(_path);
        if (_fileDialog->exec()) {
            _path = _fileDialog->selectedFiles()[0];
            _ui->saveFold->setText(_path);
            _updateLine(QString("文件地址：%1/%2.%3").arg(_path).arg(_fileName).arg(_postfix));
        }
    });
    connect(_ui->filename,&QLineEdit::textChanged,[this](const QString &text) {
        _fileName = text;
        _updateLine(QString("文件地址：%1/%2.%3").arg(_path).arg(_fileName).arg(_postfix));
    });
    connect(_ui->typeBox,&QComboBox::currentTextChanged,[this](const QString &text) {
        _postfix = toggleCase(text);
        _updateLine(QString("文件地址：%1/%2.%3").arg(_path).arg(_fileName).arg(_postfix));
    });
    connect(_ui->btn1,&QPushButton::clicked,[this]{savePathChanged(QString("%1/%2.%3").arg(_path).arg(_fileName).arg(_postfix));});
    connect(_ui->btn2,&QPushButton::clicked,[this]{this->close();});
}

void SaveFileDialog::_updateLine(const QString &text) {
    _ui->label->setText(text);
    QFontMetrics fm(_ui->label->font());
    _ui->label->setText(fm.elidedText(text, Qt::ElideMiddle, this->width()));//

}


