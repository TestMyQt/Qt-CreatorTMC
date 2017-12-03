/*!
    \class Organization
    \inmodule lib/tmcclient
    \inheaderfile organization.h
    \brief Class \l Course says, "Hello, world!"
*/

#include "organization.h"

Organization::Organization()
{

}

Organization::Organization(QString name, QString slug) :
    m_name(name),
    m_slug(slug)
{

}

QString Organization::getName()
{
    return m_name;
}

QString Organization::getSlug()
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
