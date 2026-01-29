#pragma once

// relative includes
#include "Employee.h"

namespace employee
{

/**
 * @class Worker
 * @brief Class describes operations for interaction with worker category
 */
class Worker : public Employee {
public:
    Worker() = delete;

    Worker(const Worker& other)  = delete;
    Worker(const Worker&& other) = delete;

    Worker& operator=(const Worker& other)  = delete;
    Worker& operator=(const Worker&& other) = delete;

    /**
     * @brief Get employee type
     * @return type
     */
    EmployeeType get_type() const override;

private:
    friend class Employee;

    /**
     *@brief Worker object contructor
     */
    Worker(const EmployeeDescr& description);
};

} // namespace employee