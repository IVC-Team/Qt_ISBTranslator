#ifndef IsbTranslator_H
#define IsbTranslator_H

#include <QMainWindow>
#include <QString>
#include <QMap>

namespace Ui {
class IsbTranslator;
}

class IsbTranslator : public QMainWindow
{
    Q_OBJECT

public:
    explicit IsbTranslator(QWidget *parent = 0);
    ~IsbTranslator();

    void getSupportedLanguages();

public slots:
    void translate();
private:
    Ui::IsbTranslator *ui;

    QString api_key = "trnsl.1.1.20170414T012838Z.fbc883185b4d934b.07d0c0e98f5ad49fdbac6ce95bf652c079460002";
    QMap<QString, QString> languages;
};

#endif // IsbTranslator_H
