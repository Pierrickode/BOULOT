#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

class LanguageManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(bool isRightToLeft READ isRightToLeft NOTIFY currentLanguageChanged)
    Q_PROPERTY(QStringList languageNames READ languageNames NOTIFY currentLanguageChanged)
    Q_PROPERTY(int currentLanguageIndex READ currentLanguageIndex NOTIFY currentLanguageChanged)

public:
    explicit LanguageManager(QObject *parent = nullptr);

    QString currentLanguage() const { return m_currentLanguage; }
    Q_INVOKABLE void setCurrentLanguage(const QString &langCode);
    Q_INVOKABLE void setLanguage(const QString &langCode) { setCurrentLanguage(langCode); }

    bool isRightToLeft() const { return m_currentLanguage == "ar"; }
    QStringList languageNames() const { return { "Français", "English", "العربية", "Nederlands" }; }
    int currentLanguageIndex() const;

    Q_INVOKABLE QString tr(const QString &key) const;
    Q_INVOKABLE QString t(const QString &key) const { return tr(key); }

signals:
    void currentLanguageChanged();

private:
    void initTranslations();

    QString m_currentLanguage{"fr"};
    // Map: langCode -> (key -> translatedString)
    QMap<QString, QMap<QString, QString>> m_translations;
};

#endif // LANGUAGEMANAGER_H
