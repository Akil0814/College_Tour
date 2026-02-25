#include "distance_tracker.h"
#include "data_manager.h"

DistanceTracker::DistanceTracker(DataManager* data_manager)
    : m_data_manager(data_manager),
      m_current_location(""),
      m_total_distance(0.0)

{
}

bool DistanceTracker::location_changed(const QString& new_location){

    if (m_current_location.isEmpty())
    {
        m_current_location = new_location;
        return true;
    }

    const std::optional<double> miles =

        m_data_manager->get_distance_between_college(m_current_location, new_location);

    if (!miles.has_value()){

        //fails, still need to get the college ID
        qDebug() << "Failed to find distance";
        return false;
    }

    m_total_distance += miles.value();
    m_current_location = new_location;
    return true;
}

double DistanceTracker::get_total_distance() const {

    return m_total_distance;
}

QString DistanceTracker::get_current_location() const {
    return m_current_location;
}
