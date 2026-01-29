#pragma once

// relative includes
#include "Employee.h"

namespace employee
{

/**
 * @class Manager
 * @brief Class describes operations for interaction with Manager category
 */
class Manager : public Employee {
public:
    Manager() = delete;

    Manager(const Manager& other)  = delete;
    Manager(const Manager&& other) = delete;

    Manager& operator=(const Manager& other)  = delete;
    Manager& operator=(const Manager&& other) = delete;

    /**
     * @brief Get employee type
     * @return type
     */
    EmployeeType get_type() const override;

private:
    friend class Employee;

    /**
     *@brief Manager object contructor
     */
    Manager(const EmployeeDescr& description);
};

} // namespace employee