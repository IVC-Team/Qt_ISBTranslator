#include "isbtranslator.h"
#include "ui_isbtranslator.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QMessageBox>

IsbTranslator::IsbTranslator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IsbTranslator)
{
    // Initial setup.
    ui->setupUi(this);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    this->getSupportedLanguages();

    ui->comboBoxSource->setCurrentText("English");
    ui->comboBoxTarget->setCurrentText("Japanese");
    ui->TargetEdit->setDisabled(true);

    connect(ui->translateButton,SIGNAL(clicked(bool)),this,SLOT(translate()));
}

IsbTranslator::~IsbTranslator()
{
    delete ui;
}

void IsbTranslator::getSupportedLanguages()
{
    // Create a request.
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/getLangs?")
             + QString("key=") + api_key
             + QString("&ui=") + QString("en")
             );

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Send the request.
    QEventLoop loop;
    QNetworkAccessManager manager;
    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    // Receive a reply.
    QNetworkReply* reply = manager.get(request);
    loop.exec();

    // Parse the reply.
    QByteArray bytes = reply->readAll();
    QJsonObject obj = QJsonDocument::fromJson(bytes).object();  // Keys are ("dirs", "langs")

    // Check the response codes.
    if(obj.contains("code") || obj.contains("message"))
    {
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    QJsonObject langs = obj.value("langs").toObject();  // Codes and Languages.
    for(const QString& code : langs.keys())
    {
        QString lang = langs.value(code).toString();
        languages[code] = lang;
        // Fill comboBoxes with initial data.
        ui->comboBoxSource->addItem(lang);
        ui->comboBoxTarget->addItem(lang);
    }
}


void IsbTranslator::translate()
{
    QString source_lang = ui->comboBoxSource->currentText();
    QString target_lang = ui->comboBoxTarget->currentText();

    // Create a request.
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/translate?")
             + QString("key=") + api_key
             + QString("&lang=")
             + languages.key(source_lang) + "-" + languages.key(target_lang)
             + QString("&text=") + ui->SourceEdit->toPlainText()
             );

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Send the request.
    QEventLoop loop;
    QNetworkAccessManager manager;
    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    // Receive a reply.
    QNetworkReply* reply = manager.get(request);
    loop.exec();

    // Parse the reply.
    QByteArray bytes = reply->readAll();
    QJsonObject obj = QJsonDocument::fromJson(bytes).object();

    QJsonArray text = obj.value("text").toArray();
    ui->TargetEdit->clear();
    for(QJsonValue v : text)
    {
        ui->TargetEdit->append(v.toString());
    }
}
