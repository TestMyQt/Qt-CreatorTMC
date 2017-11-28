#ifndef ORGANIZATION_H
#define ORGANIZATION_H

#include "course.h"

#include <QString>
#include <QList>
#include <QSettings>

class Organization
{
public:
    Organization();
    Organization(QString name, QString slug);
    QString getName();
    QString getSlug();
    void addCourse(Course c);
    QList<Course> getCourses();

    static Organization fromQSettings(QSettings *settings);

private:
    QString m_name;
    QString m_slug;
    QList<Course> m_courses;
};

#endif // ORGANIZATION_H
