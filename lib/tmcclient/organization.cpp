/*!
    \class Organization
    \inmodule lib/tmcclient
    \inheaderfile organization.h
    \brief Class \l Organization objects represent the educational organizations
        that have TMC courses available. Each \l Organization object contains
        a collection of \l Course objects that correspond to the courses the
        organization is offering online.
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

/*!
    Getter function for the organization name.
*/
QString Organization::getName()
{
    return m_name;
}

QString Organization::getSlug()
{
    return m_slug;
}

/*!
    Adds the \l Course parameter \a c to the collection of the organization's courses.
*/
void Organization::addCourse(Course c)
{
    m_courses.append(c);
}

/*!
    Getter function for the \l Organization object's collection of \l Course objects
    which is implemented as a \l {http://doc.qt.io/qt-5/qlist.html} {QList}.
*/
QList<Course> Organization::getCourses()
{
    return m_courses;
}

Organization Organization::fromQSettings(QSettings *settings)
{
    return Organization(settings->value("orgName", "").toString(),
                        settings->value("orgSlug", "").toString());
}
