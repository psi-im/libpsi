/*
 * hunspellchecker.cpp
 *
 * Copyright (C) 2015  Sergey Ilinykh, Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * You can also redistribute and/or modify this program under the
 * terms of the Psi License, specified in the accompanied COPYING
 * file, as published by the Psi Project; either dated January 1st,
 * 2005, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "hunspellchecker.h"

#ifdef Q_OS_WIN
#include "applicationinfo.h"
#endif
#include "config.h"
#include "languagemanager.h"

#include <QCoreApplication>
//#include <QDebug>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QMutableListIterator>
#include <QSet>
#include <QString>
#include <QTextCodec>
#include <hunspell.hxx>

#ifdef H_DEPRECATED
#define NEW_HUNSPELL
#define HS_STRING(text) li.codec->fromUnicode(text).toStdString()
#else
#define HS_STRING(text) li.codec->fromUnicode(text)
#endif

HunspellChecker::HunspellChecker()
{
    getDictPaths();
    getSupportedLanguages();
}

HunspellChecker::~HunspellChecker() { }

void HunspellChecker::getDictPaths()
{
    if (dictPaths_.isEmpty()) {
        QSet<QString> dictPathSet;
        QString       pathFromEnv = QString::fromLocal8Bit(qgetenv("MYSPELL_DICT_DIR"));
        if (!pathFromEnv.isEmpty())
            dictPathSet << pathFromEnv;
#if defined(Q_OS_WIN)
        dictPathSet << QCoreApplication::applicationDirPath() + QLatin1String("/myspell/dicts")
                    << ApplicationInfo::homeDir(ApplicationInfo::DataLocation) + QLatin1String("/myspell/dicts");

#elif defined(Q_OS_MAC)
        dictPathSet << QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/myspell/dicts")    // relative path to dictionaries inside app bundle
                    << QLatin1String("/opt/local/share/myspell"); // MacPorts standard paths
#elif defined(Q_OS_HAIKU)
        dictPathSet << QLatin1String("/system/data/hunspell");
#else
        dictPathSet << QLatin1String("/usr/share/myspell") << QLatin1String("/usr/share/hunspell")
                    << QLatin1String("/usr/local/share/myspell") << QLatin1String("/usr/local/share/hunspell")
                    << QString("%1/.local/share/myspell").arg(QDir::home().absolutePath())
                    << QString("%1/.local/share/hunspell").arg(QDir::home().absolutePath());
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        dictPaths_ = dictPathSet.values();
#else
        dictPaths_ = dictPathSet.toList();
#endif
#if defined(Q_OS_LINUX) && defined(SHARE_SUFF)
    // Special hack for correct work of AppImage, snap and flatpak builds
    static const QString &&additionalPath =
            QDir().absoluteFilePath(qApp->applicationDirPath() + "/../share/" SHARE_SUFF);
    dictPaths_.prepend(additionalPath + "/hunspell");
    dictPaths_.prepend(additionalPath + "/myspell");
#endif
    }
}

bool HunspellChecker::scanDictPaths(const QString &language, QFileInfo &aff, QFileInfo &dic)
{
    for (const QString &dictPath : qAsConst(dictPaths_)) {
        QDir dir(dictPath);
        if (dir.exists()) {
            QFileInfo affInfo(dir.filePath(language + QLatin1String(".aff")));
            QFileInfo dicInfo(dir.filePath(language + QLatin1String(".dic")));
            if (affInfo.isReadable() && dicInfo.isReadable()) {
                aff = affInfo;
                dic = dicInfo;
                return true;
            }
        }
    }
    return false;
}

void HunspellChecker::getSupportedLanguages()
{
    QSet<LanguageManager::LangId> retHash;
    for (const QString &dictPath : qAsConst(dictPaths_)) {
        QDir dir(dictPath);
        if (!dir.exists()) {
            continue;
        }
        const auto fileInfoList = dir.entryInfoList(QStringList() << "*.dic", QDir::Files);
        for (const QFileInfo &fi : fileInfoList) {
            auto id = LanguageManager::fromString(fi.baseName());
            if (id.language) {
                retHash.insert(id);
            }
        }
    }
    supportedLangs_ = retHash;
}

void HunspellChecker::addLanguage(const LanguageManager::LangId &langId)
{
    QString   language = LanguageManager::toString(langId).replace('-', '_');
    QFileInfo aff, dic;
    if (scanDictPaths(language, aff, dic)) {
        LangItem li;
        li.hunspell_
            = HunspellPtr(new Hunspell(aff.absoluteFilePath().toLocal8Bit(), dic.absoluteFilePath().toLocal8Bit()));
        QByteArray codecName(li.hunspell_->get_dic_encoding());
        if (codecName.startsWith("microsoft-cp125")) {
            codecName.replace(0, sizeof("microsoft-cp") - 1, "Windows-");
        } else if (codecName.startsWith("TIS620-2533")) {
            codecName.resize(sizeof("TIS620") - 1);
        }
        li.codec = QTextCodec::codecForName(codecName);
        if (li.codec) {
            li.info.langId   = langId;
            li.info.filename = dic.filePath();
            languages_.append(li);
        } else {
            qDebug("Unsupported myspell dict encoding: \"%s\" for %s", codecName.data(), qPrintable(dic.fileName()));
        }
    }
}

QList<QString> HunspellChecker::suggestions(const QString &word)
{
    QStringList qtResult;
    for (const LangItem &li : qAsConst(languages_)) {
#ifdef NEW_HUNSPELL
        std::vector<std::string> result = li.hunspell_->suggest(HS_STRING(word));
        if (!result.empty()) {
            for (const std::string &item : result) {
                qtResult << QString(li.codec->toUnicode(item.c_str()));
            }
        }
#else
        char **result;
        int    sugNum = li.hunspell_->suggest(&result, HS_STRING(word));
        for (int i = 0; i < sugNum; i++) {
            qtResult << li.codec->toUnicode(result[i]);
        }
        li.hunspell_->free_list(&result, sugNum);
#endif
    }
    return std::move(qtResult);
}

bool HunspellChecker::isCorrect(const QString &word)
{
    for (const LangItem &li : qAsConst(languages_)) {
        if (li.hunspell_->spell(HS_STRING(word)) != 0) {
            return true;
        }
    }
    return false;
}
bool HunspellChecker::add(const QString &word)
{
    if (!word.isEmpty()) {
        QString trimmed_word = word.trimmed();
        for (const LangItem &li : qAsConst(languages_)) {
            if (li.hunspell_->add(HS_STRING(trimmed_word)) != 0) {
                return true;
            }
        }
    }
    return false;
}
bool HunspellChecker::available() const
{
    for (const LangItem &li : languages_) {
        if (li.hunspell_) {
            return true;
        }
    }
    return false;
}
bool HunspellChecker::writable() const { return false; }

void HunspellChecker::unloadLanguage(const LanguageManager::LangId &langId)
{
    QMutableListIterator<LangItem> it(languages_);
    while (it.hasNext()) {
        LangItem item = it.next();
        if (item.info.langId == langId) {
            it.remove();
        }
    }
}

QSet<LanguageManager::LangId> HunspellChecker::getAllLanguages() const { return supportedLangs_; }

void HunspellChecker::setActiveLanguages(const QSet<LanguageManager::LangId> &newLangs)
{
    QSet<LanguageManager::LangId> loadedLangs;
    for (const LangItem &item : qAsConst(languages_)) {
        loadedLangs << item.info.langId;
    }
    QSet<LanguageManager::LangId>         langsToUnload = loadedLangs - newLangs;
    QSet<LanguageManager::LangId>         langsToLoad   = newLangs - loadedLangs;
    QSetIterator<LanguageManager::LangId> it(langsToUnload);
    while (it.hasNext()) {
        unloadLanguage(it.next());
    }
    it = langsToLoad;
    while (it.hasNext()) {
        addLanguage(it.next());
    }
}
