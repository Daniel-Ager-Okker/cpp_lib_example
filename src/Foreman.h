#pragma once

// relative includes
#include "Employee.h"

namespace employee
{

/**
 * @class Foreman
 * @brief Class describes operations for interaction with foreman category
 */
class Foreman : public Employee {
public:
    Foreman() = delete;

    Foreman(const Foreman& other)  = delete;
    Foreman(const Foreman&& other) = delete;

    Foreman& operator=(const Foreman& other)  = delete;
    Foreman& operator=(const Foreman&& other) = delete;

    /**
     * @brief Get employee type
     * @return type
     */
    EmployeeType get_type() const override;

private:
    friend class Employee;

    /**
     *@brief Foreman object contructor
     */
    Foreman(const EmployeeDescr& description);
};

} // namespace employee