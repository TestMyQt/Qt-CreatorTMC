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
    Organization(const QString &name, const QString &slug);
    bool operator==(const Organization &other) const;
    bool operator!=(const Organization &other) const;
    bool operator!() const;

    QString getName() const;
    QString getSlug() const;
    void addCourse(const Course &c);
    QList<Course> getCourses();

    static Organization fromQSettings(QSettings &settings);
    static Organization fromJson(const QJsonObject &jsonOrg);
    static void toQSettings(QSettings &settings, const Organization &org);

private:
    QString m_name;
    QString m_slug;
    QList<Course> m_courses;
};

Q_DECLARE_METATYPE(Organization)

#endif // ORGANIZATION_H
