#include "organization.h"

Organization::Organization()
{

}

Organization::Organization(QString name, QString slug) :
    m_name(name),
    m_slug(slug)
{

}

bool Organization::operator==(const Organization &other) const
{
    return other.getName() == m_name &&
            other.getSlug() == m_slug;
}

bool Organization::operator!=(const Organization &other) const
{
    return !(*this == other);
}

QString Organization::getName() const
{
    return m_name;
}

QString Organization::getSlug() const
{
    return m_slug;
}

void Organization::addCourse(Course c)
{
    m_courses.append(c);
}

QList<Course> Organization::getCourses()
{
    return m_courses;
}

Organization Organization::fromQSettings(QSettings *settings)
{
    return Organization(settings->value("orgName", "").toString(),
                        settings->value("orgSlug", "").toString());
}

void Organization::toQSettings(QSettings *settings, Organization org)
{
    settings->setValue("orgName", org.getName());
    settings->setValue("orgSlug", org.getSlug());
}

Organization Organization::fromJson(const QJsonObject jsonOrg)
{
    Organization fromJson = Organization(
                jsonOrg["name"].toString(),
                jsonOrg["slug"].toString());

    return fromJson;
}
