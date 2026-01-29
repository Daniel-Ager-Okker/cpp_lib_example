#pragma once

// lib includes
#include <employee_lib/EmployeeManager.h>

namespace employee
{

/**
 * @class Employee
 * @brief Class describes operations for interaction with employee
 */
class Employee {
public:
    virtual ~Employee() = default;

    /**
     * @brief Create an Employee object
     * @param description employee description
     * @return Employee entity
     */
    static Employee* create(const EmployeeDescr& description);

    /**
     * @brief Get employee unique identifier
     */
    const uuid_t& get_id() const;

    /**
     * @brief Get hire date
     * @return hire date
     */
    const date_t& get_hire_date() const;

    /**
     * @brief Get base salary
     * @return base salary
     */
    double get_base_salary() const;

    /**
     * @brief Get employee type
     * @return type
     */
    virtual EmployeeType get_type() const = 0;

protected:
    /**
     *@brief Employee object contructor
     */
    Employee(const EmployeeDescr& description);

protected:
    date_t       hire_date_;   //!< Date of employment
    double       base_salary_; //!< Base salary at the moment of employment
    EmployeeType type_;        //!< Employee category

private:
    uuid_t id_; //!< Unique identifier
};

} // namespace employee