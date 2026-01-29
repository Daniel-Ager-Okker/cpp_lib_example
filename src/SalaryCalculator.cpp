// relative includes
#include "SalaryCalculator.h"
#include "Employee.h"
#include "RelationManager.h"

using namespace employee;

//! Contruct salary calculator entity
SalaryCalculator::SalaryCalculator(const boost::unordered_map<uuid_t, Employee*>& storage,
                                   const RelationManager&                         relation_mgr) :
    employees_(storage), relation_manager_(relation_mgr) {}

//! Calculate month salary of specific employee
std::pair<double, bool> SalaryCalculator::calculate_month_salary(const uuid_t& id,
                                                                 const date_t& date) const {
    // Be calm and sure: employees_ container locked by the calling party (one level above)
    auto employee_it = employees_.find(id);
    assert(employee_it != employees_.end());

    const Employee* p_employee = employee_it->second;
    assert(p_employee != nullptr);

    switch (p_employee->get_type()) {
        case EmployeeType::WORKER:
            return calculate_worker_salary(p_employee, date);

        case EmployeeType::FOREMAN:
            return calculate_foreman_salary(p_employee, date);

        case EmployeeType::MANAGER:
            return calculate_manager_salary(p_employee, date);

        default:
            break;
    }

    return {0.0, false};
}

//! Calculate month salary of worker and success flag
std::pair<double, bool> SalaryCalculator::calculate_worker_salary(const Employee* p_obj,
                                                                  const date_t&   date) const {
    const date_t& hire_date   = p_obj->get_hire_date();
    const double  base_salary = p_obj->get_base_salary();

    // 1.Simple validation
    const date_t hire_date_normalized{hire_date.year(), hire_date.month(), 1};
    const date_t date_normalized{date.year(), date.month(), 1};

    if (date_normalized < hire_date_normalized) {
        return {0.0, false};
    }

    // 2.Bonus calculation
    const int months_diff = 12 * (date_normalized.year() - hire_date_normalized.year()) +
                            (date_normalized.month() - hire_date_normalized.month());

    const int full_years = months_diff / 12;

    const double bonus = std::min(base_salary, 0.1 * full_years * base_salary);

    return {base_salary + bonus, true};
}

//! Calculate month salary of foreman
std::pair<double, bool> SalaryCalculator::calculate_foreman_salary(const Employee* p_obj,
                                                                   const date_t&   date) const {
    const date_t& hire_date   = p_obj->get_hire_date();
    const double  base_salary = p_obj->get_base_salary();

    // 1.Simple validation
    const date_t hire_date_normalized{hire_date.year(), hire_date.month(), 1};
    const date_t date_normalized{date.year(), date.month(), 1};

    if (date_normalized < hire_date_normalized) {
        return {0.0, false};
    }

    // 2.Bonus calculation (for long service)
    const int months_diff = 12 * (date_normalized.year() - hire_date_normalized.year()) +
                            (date_normalized.month() - hire_date_normalized.month());

    const int full_years = months_diff / 12;

    const double bonus_years = std::min(base_salary * 0.4, 0.05 * full_years * base_salary);

    // 3.Bonus calculation (for direct subordinates)
    const std::vector<uuid_t> direct_subordinates =
        relation_manager_.get_direct_subordinates(p_obj->get_id());

    double subordinates_total_salary = 0.0;
    for (const uuid_t& subordinate_id : direct_subordinates) {
        const auto [salary, ok] = calculate_month_salary(subordinate_id, date);
        if (!ok) {
            return {0.0, false};
        }
        subordinates_total_salary += salary;
    }

    subordinates_total_salary *= 0.07;

    return {base_salary + bonus_years + subordinates_total_salary, true};
}

//! Calculate month salary of manager
std::pair<double, bool> SalaryCalculator::calculate_manager_salary(const Employee* p_obj,
                                                                   const date_t&   date) const {
    const date_t& hire_date   = p_obj->get_hire_date();
    const double  base_salary = p_obj->get_base_salary();

    // 1.Simple validation
    const date_t hire_date_normalized{hire_date.year(), hire_date.month(), 1};
    const date_t date_normalized{date.year(), date.month(), 1};

    if (date_normalized < hire_date_normalized) {
        return {0.0, false};
    }

    // 2.Bonus calcultaion
    const std::vector<uuid_t> all_subordinates =
        relation_manager_.get_all_subordinates(p_obj->get_id());

    double subordinates_total_salary = 0.0;
    for (const uuid_t& subordinate_id : all_subordinates) {
        const auto [salary, ok] = calculate_month_salary(subordinate_id, date);
        if (!ok) {
            return {0.0, false};
        }
        subordinates_total_salary += salary;
    }

    subordinates_total_salary *= 0.03;

    return {base_salary + subordinates_total_salary, true};
}