#ifndef ZIPHELPER_H
#define ZIPHELPER_H

#include <QBuffer>
#include <QDir>

#include <utils/fileutils.h>

using Utils::FileName;
using Utils::FileNameList;

class ZipHelper
{
    public:
        static bool createZip(QDir projectDir, FileNameList files, QBuffer *zipBuffer);
        static QStringList extractZip(QByteArray *zipData, QString saveDir);
};

#endif // ZIPHELPER_H
