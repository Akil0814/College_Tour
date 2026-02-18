#include "data_manager.h"
#include <stdexcept>
#include <string>

/*
This function recursively visits the nearest unvisited campus by linearly searching remaining campuses to visit,
choosing the closest one from the current stop, removing it from remaining, and recursing until no campuses remain.

If no edges exist between current campus and all remaining a runtime error is thrown.
*/

void visit_next(
    int current_college_id,
    QVector<int> remaining_colleges,
    QVector<int> visit_order,
    double &total_distance,
    const DataManager &data_manager)
{
    // Base case if there are no more remaining colleges to visit
    if (remaining_colleges.isEmpty())
    {
        return;
    }

    //
    int closest_id = -1;
    int closest_index = -1;

    // Init min distance to largest int so all valid distances are smaller
    int min_distance = std::numeric_limits<int>::max();

    // For all remaining unviseted colleges
    for (int i = 0; i < remaining_colleges.size(); i++)
    {
        int next_id = remaining_colleges[i];

        // Get distance between current college and next canidate | nullptr if not found
        auto distance = data_manager.get_distance_between_college(current_college_id, next_id);

        // Check if distance is valid and check if its the current smallest
        if (distance && *distance < min_distance)
        {
            // Update new shortest
            min_distance = *distance;

            // Store id and store index for removal
            closest_id = next_id;
            closest_index = i;
        }
    }

    // Check that a next stop is found
    if (closest_id == -1)
    {
        throw std::runtime_error("No reachable college was found from current campus id:" + std::to_string(current_college_id));
    }

    // Update state | total distance | visit order | remove from remaining
    total_distance += min_distance;
    visit_order.push_back(closest_id);
    remaining_colleges.removeAt(closest_index);

    // Recurse
    visit_next(closest_id, remaining_colleges, visit_order, total_distance, data_manager);
}
