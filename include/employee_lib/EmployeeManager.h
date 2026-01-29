#pragma once

// boost includes
#include <boost/uuid/uuid.hpp>

// C++ includes
#include <memory>
#include <optional>

// relative includes
#include "EmployeeDescr.h"

namespace employee
{

using uuid_t = boost::uuids::uuid;

/**
 * @class EmployeeManager
 * @brief Class that provides an API for work with employee logic
 */
class EmployeeManager {
public:
    EmployeeManager(const EmployeeManager& other)  = delete;
    EmployeeManager(const EmployeeManager&& other) = delete;

    EmployeeManager& operator=(const EmployeeManager& other)  = delete;
    EmployeeManager& operator=(const EmployeeManager&& other) = delete;

    /**
     * @brief Construct an EmployeeManager object
     */
    EmployeeManager();

    /**
     * @brief Destruct an EmployeeManager object
     */
    ~EmployeeManager();

    /**
     * @brief Registrate new employee
     * @param description employee description
     * @return unique employee identifier and success status
     */
    std::pair<uuid_t, bool> add_employee(const EmployeeDescr& description);

    /**
     * @brief Remove employee from registration list
     * @param id unique employee identifier
     * @return success status
     */
    bool remove_employee(const uuid_t& id);

    /**
     * @brief Find employee by it unique identifier
     * @param id unique employee identifier
     * @return optional value of employee description
     */
    std::optional<EmployeeDescr> find_employee(const uuid_t& id) const;

    /**
     * @brief Add relation between chief and subordinate
     * @param chief chief unique id
     * @param subordinate subordinate unique id
     * @return was added or not
     */
    bool add_subordination(const uuid_t& chief, const uuid_t& subordinate);

    /**
     * @brief Remove subordination relation between chief and subordinate
     * @param chief сhief unique identifier
     * @param subordinate subordinate unique identifier
     * @return true if relation existed and was removed, false otherwise
     */
    bool remove_subordination(const uuid_t& chief, const uuid_t& subordinate);

    /**
     * @brief Get employee chief
     * @param id employee unique identifier
     * @return chief unique identifier (optional value)
     */
    std::optional<uuid_t> get_chief(const uuid_t& id) const;

    /**
     * @brief Get employee direct subordinates
     * @param id employee unique identifier
     * @return direct subordinates
     */
    std::vector<uuid_t> get_direct_subordinates(const uuid_t& id) const;

    /**
     * @brief Get employee all subordinates
     * @param id employee unique identifier
     * @return direct subordinates
     */
    std::vector<uuid_t> get_all_subordinates(const uuid_t& id) const;

    /**
     * @brief Calculate employee salary
     * @param id employee unique identifier
     * @param date date
     * @return month salary and success flag
     */
    std::pair<double, bool> calculate_employee_salary(const uuid_t& id, const date_t& date) const;

private:
    class PrivateData;
    std::unique_ptr<PrivateData> p_data_;
};

} // namespace employee
