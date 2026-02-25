#ifndef DISTANCE_TRACKER_H
#define DISTANCE_TRACKER_H

#include <QString>
#include <optional>

class DataManager;

class DistanceTracker
{
public:
    explicit DistanceTracker(DataManager* data_manager);

    bool location_changed (const QString& new_location);
    double get_total_distance () const;
    QString get_current_location () const;
private:
    DataManager* m_data_manager;
    QString m_current_location;
    double m_total_distance;
};

#endif // DISTANCE_TRACKER_H
