#include "ziphelper.h"

#include <quazip/JlCompress.h>
#include <QBuffer>
#include <QDebug>
#include <QMutableListIterator>

bool ZipHelper::createZip(QDir projectDir, FileNameList files, QBuffer *zipBuffer)
{
    QuaZip zip(zipBuffer);

    if (!zip.open(QuaZip::mdCreate)) {
        return false;
    }

    QuaZipFile zipFile(&zip);
    QFile inFile;

    foreach(FileName file, files) {
        if (!file.toFileInfo().isFile())
            continue;

        QString filePath = file.toFileInfo().filePath();
        QString relativePath = projectDir.relativeFilePath(filePath);

        inFile.setFileName(filePath);
        if (!inFile.open(QIODevice::ReadOnly)) {
            return false;
        }

        QuaZipNewInfo info = QuaZipNewInfo(relativePath, filePath);
        if (!zipFile.open(QIODevice::WriteOnly, info)) {
            return false;
        }

        // Copy data to zip
        zipFile.write(inFile.readAll());

        // Was writing ok?
        if (zipFile.getZipError() != 0) {
            return false;
        }

        zipFile.close();

        // Was close ok?
        if (zipFile.getZipError() != 0) {
            return false;
        }

        inFile.close();
    }

    return true;
}

QStringList ZipHelper::extractZip(QByteArray *zipData, QString saveDir)
{
    QBuffer zipBuffer(zipData);
    zipBuffer.open(QIODevice::ReadOnly);

    QStringList fileList = JlCompress::getFileList(&zipBuffer);

    // Remove student files in "project_dir/src/"
    QStringList studentFiles = fileList.filter("/src/");

    QStringListIterator i(studentFiles);
    while(i.hasNext()){
        fileList.removeAll(i.next());
    }

    QStringList extracted = JlCompress::extractFiles(&zipBuffer, fileList, saveDir);

    return extracted;
}
