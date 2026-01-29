// relative includes
#include "RelationManager.h"

// C++ includes
#include <stack>

using employee::RelationManager;

using uuid_t = boost::uuids::uuid;

//! Add subordination relation
bool RelationManager::add_relation(const uuid_t& id_chief, const uuid_t& id) {
    // 1.Validation on self-subordination
    if (id_chief == id) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx_);

    // 2.Validate if we already have a subordinator with `id` identifier
    // Also we can skip reverse check due to consistency of containers
    auto subordinate_it = sub_to_chief_.find(id);
    if (subordinate_it != sub_to_chief_.end()) {
        return false;
    }

    // 3.Validate on hierarchical cycle
    if (has_hierarchical_cycle(id_chief, id)) {
        return false;
    }

    // 4.Add
    sub_to_chief_[id] = id_chief;
    chief_to_subs_.insert({id_chief, id});

    return true;
}

//! Remove subordination relation
bool RelationManager::remove_relation(const uuid_t& id_chief, const uuid_t& id) {
    // 1.Validation on self-subordination
    if (id_chief == id) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx_);

    // 2.Validate if we have a subordinator with `id` identifier with corresponging chief
    auto subordinate_it = sub_to_chief_.find(id);
    if (subordinate_it == sub_to_chief_.end() || subordinate_it->second != id_chief) {
        return false;
    }

    // 3.Remove from containers
    auto [begin, end] = chief_to_subs_.equal_range(id_chief);

    bool found = false;
    for (auto it = begin; it != end; ++it) {
        if (it->second == id) {
            chief_to_subs_.erase(it);
            found = true;
            break;
        }
    }

    if (!found) {
        return false;
    }

    sub_to_chief_.erase(subordinate_it);

    return true;
}

//! Find employee chief
std::optional<uuid_t> RelationManager::get_chief(const uuid_t& id) const {
    std::lock_guard<std::mutex> lock(mtx_);

    auto it = sub_to_chief_.find(id);
    if (it == sub_to_chief_.end()) {
        return std::nullopt;
    }

    return it->second;
}

//! Get employee direct subordinates
std::vector<uuid_t> RelationManager::get_direct_subordinates(const uuid_t& id) const {
    std::lock_guard<std::mutex> lock(mtx_);

    auto it = chief_to_subs_.find(id);
    if (it == chief_to_subs_.end()) {
        return {};
    }

    auto [begin, end] = chief_to_subs_.equal_range(id);

    std::vector<uuid_t> direct_subordinates;
    direct_subordinates.reserve(std::distance(begin, end));

    while (begin != end) {
        direct_subordinates.push_back(begin->second);
        ++begin;
    }

    return direct_subordinates;
}

//! Get employee all subordinates
std::vector<uuid_t> RelationManager::get_all_subordinates(const uuid_t& id) const {
    std::lock_guard<std::mutex> lock(mtx_);

    auto it = chief_to_subs_.find(id);
    if (it == chief_to_subs_.end()) {
        return {};
    }

    std::vector<uuid_t> all_subordinates;

    // dfs
    std::stack<uuid_t> tower;

    // 1.First step - direct subordinates
    auto [begin, end] = chief_to_subs_.equal_range(id);
    for (; begin != end; ++begin) {
        tower.push(begin->second);
        all_subordinates.push_back(begin->second);
    }

    // 2.Then - all subordinates of direct and so on (recursively)
    while (!tower.empty()) {
        const uuid_t& current = tower.top();
        tower.pop();

        auto [first, last] = chief_to_subs_.equal_range(current);
        for (; first != last; ++first) {
            tower.push(first->second);
            all_subordinates.push_back(first->second);
        }
    }

    return all_subordinates;
}

//! Helper function for validate check if ids has hierarchical cycle
bool RelationManager::has_hierarchical_cycle(const uuid_t& id_chief, const uuid_t& id) const {
    uuid_t current = id_chief;

    while (true) {
        if (current == id) {
            return true;
        }

        auto it = sub_to_chief_.find(current);
        if (it == sub_to_chief_.end()) {
            break;
        }

        current = it->second;
    }

    return false;
}