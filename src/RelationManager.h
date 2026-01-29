#pragma once

// boost includes
#include <boost/unordered_map.hpp>
#include <boost/uuid/uuid.hpp>

// C++ includes
#include <mutex>

namespace employee
{

/**
 * @class RelationManager
 * @brief Class responsible for work hierarchy and subordination relationship
 */
class RelationManager {
public:
    /**
     * @brief Add subordination relation
     * Attention! An employee cannot be his own chief.
     * Attention! What if there is already a subordinate with the identifier `id`?
     * Then you can't add relation, you need to remove old relation first.
     * Reason: one subordinate can have only one chief.
     * Attention! Hierarchical cycles are not allowed.
     * @param id_chief chief unique identifier
     * @param id subordinate unique identifier
     * @return was added or not
     */
    bool add_relation(const boost::uuids::uuid& id_chief, const boost::uuids::uuid& id);

    /**
     * @brief Remove subordination relation
     * @param id_chief chief unique identifier
     * @param id subordinate unique identifier
     * @return was added or not
     */
    bool remove_relation(const boost::uuids::uuid& id_chief, const boost::uuids::uuid& id);

    /**
     * @brief Find employee chief
     * @param id employee unique identifier
     * @return chief unique identifier (optional value)
     */
    std::optional<boost::uuids::uuid> get_chief(const boost::uuids::uuid& id) const;

    /**
     * @brief Get employee direct subordinates
     * @param id employee unique identifier
     * @return direct subordinates
     */
    std::vector<boost::uuids::uuid> get_direct_subordinates(const boost::uuids::uuid& id) const;

    /**
     * @brief Get employee all subordinates
     * @param id employee unique identifier
     * @return direct subordinates
     */
    std::vector<boost::uuids::uuid> get_all_subordinates(const boost::uuids::uuid& id) const;

private:
    /**
     * @brief Helper function for validate check if ids has hierarchical cycle
     * @return has cycle or not
     */
    bool has_hierarchical_cycle(const boost::uuids::uuid& id_chief,
                                const boost::uuids::uuid& id) const;

private:
    //!< Mutex for threads sync
    mutable std::mutex mtx_;

    //!< Relation "subordinate-->chief" (one to one)
    boost::unordered_map<boost::uuids::uuid, boost::uuids::uuid> sub_to_chief_;

    //!< Relation "chief--subordinates>" (one to many)
    boost::unordered_multimap<boost::uuids::uuid, boost::uuids::uuid> chief_to_subs_;
};

} // namespace employee