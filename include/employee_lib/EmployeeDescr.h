#pragma once

// boost includes
#include <boost/date_time/gregorian/gregorian.hpp>

namespace employee
{

using date_t = boost::gregorian::date;

/**
 * @class EmployeeType
 * @brief Class that enumerates possbile employee categories
 */
enum class EmployeeType { WORKER = 0, FOREMAN, MANAGER };

/**
 * @class EmployeeDescr
 * @brief Class that describes employee main attributes
 */
struct EmployeeDescr {
    EmployeeType type;        //!< Employee's category
    double       base_salary; //!< Employee's base salary at the time of employment
    date_t       hire_date;   //!< Date of employment (attention: day value always be ignored)
};

} // namespace employee