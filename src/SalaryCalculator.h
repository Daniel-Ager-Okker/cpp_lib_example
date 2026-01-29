#pragma once

// boost includes
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/unordered_map.hpp>
#include <boost/uuid/uuid.hpp>

namespace employee
{

class Employee;
class RelationManager;

/**
 * @class SalaryCalculator
 * @brief Class for work with salary algorithms calculation
 */
class SalaryCalculator {
public:
    SalaryCalculator() = delete;

    SalaryCalculator(const SalaryCalculator& other)  = delete;
    SalaryCalculator(const SalaryCalculator&& other) = delete;

    SalaryCalculator& operator=(const SalaryCalculator& other)  = delete;
    SalaryCalculator& operator=(const SalaryCalculator&& other) = delete;

    /**
     * Contruct salary calculator entity
     * @param storage storage of Employee objects (const reference)
     * @param relation_mgr relation manager (const reference)
     */
    SalaryCalculator(const boost::unordered_map<boost::uuids::uuid, Employee*>& storage,
                     const RelationManager&                                     relation_mgr);

    /**
     * @brief Calculate month salary of specific employee
     * @param id employee identifier
     * @param date estimated date of salary payment
     * @return calculated salary and success flag
     */
    std::pair<double, bool> calculate_month_salary(const boost::uuids::uuid&     id,
                                                   const boost::gregorian::date& date) const;

private:
    /**
     * @brief Calculate month salary of worker
     * @param p_obj Worker entity
     * @param date estimated date of salary payment
     * @return calculated salary and success flag
     */
    std::pair<double, bool> calculate_worker_salary(const Employee*               p_obj,
                                                    const boost::gregorian::date& date) const;

    /**
     * @brief Calculate month salary of foreman
     * @param p_obj Foreman entity
     * @param date estimated date of salary payment
     * @return calculated salary and success flag
     */
    std::pair<double, bool> calculate_foreman_salary(const Employee*               p_obj,
                                                     const boost::gregorian::date& date) const;

    /**
     * @brief Calculate month salary of manager
     * @param p_obj Manager entity
     * @param date estimated date of salary payment
     * @return calculated salary and success flag
     */
    std::pair<double, bool> calculate_manager_salary(const Employee*               p_obj,
                                                     const boost::gregorian::date& date) const;

private:
    const boost::unordered_map<boost::uuids::uuid, Employee*>& employees_;
    const RelationManager&                                     relation_manager_;
};

} // namespace employee