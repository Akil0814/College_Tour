#include "data_manager.h"
#include <stdexcept>
#include <string>

/*
This function recursively visits the nearest unvisited campus by linearly searching remaining campuses to visit,
choosing the closest one from the current stop, removing it from remaining, and recursing until no campuses remain.

If no edges exist between current campus and all remaining a runtime error is thrown. Otherwise
visit order will store college ids in the order to be visited.

Total distance returned as a vector of each leg distance.
*/

void visit_next(
    int current_college_id,
    QVector<int> &remaining_colleges,
    QVector<int> &visit_order,
    QVector<int> &total_distance,
    const DataManager &data_manager)
{
    // Base case | there are no more remaining colleges to visit
    if (remaining_colleges.isEmpty())
    {
        return;
    }

    int nearest_campus_id = -1;
    int nearest_campus_index = -1;

    // Ensures first valid distance will always be set to the min distance
    int nearest_campus_dist = std::numeric_limits<int>::max();

    // Search through unvisited campuss
    for (int i = 0; i < remaining_colleges.size(); i++)
    {
        int next_campus_id = remaining_colleges[i];

        // Get distance between current college and next canidate | nullptr if no valid edge
        auto distance = data_manager.get_distance_between_college(current_college_id, next_campus_id);

        // Check if distance is valid and check if its the current smallest | always true on first iteration
        if (distance.has_value() && *distance < nearest_campus_dist)
        {
            // Update new shortest distance and record campus id and location in remaining
            nearest_campus_dist = *distance;
            nearest_campus_id = next_campus_id;
            nearest_campus_index = i;
        }
    }

    // Throw error if next stop is not found | nearest is still sentinal
    if (nearest_campus_id == -1)
    {
        throw std::runtime_error("No reachable college was found from current campus id:" + std::to_string(current_college_id));
    }

    // Update state | distance to next campus | visit order | remove from remaining
    total_distance.push_back(nearest_campus_dist);
    visit_order.push_back(nearest_campus_id);
    remaining_colleges.removeAt(nearest_campus_index);

    visit_next(nearest_campus_id, remaining_colleges, visit_order, total_distance, data_manager);
}

QVector<int> route_optimize(int start_id, QVector<int> destinations) {
    QVector<int> visit_order;
    QVector<int> leg_distances;

    // The algorithm starts at the fixed starting college
    visit_order.push_back(start_id);

    // Logic from  helper function
    visit_next(start_id, destinations, visit_order, leg_distances, *DataManager::instance());

    return visit_order;
}
