// lib includes
#include <employee_lib/EmployeeManager.h>

// boost includes
#include <boost/unordered_map.hpp>

// relative includes
#include "Employee.h"
#include "RelationManager.h"
#include "SalaryCalculator.h"

// С++ includes
#include <mutex>

using namespace employee;

class EmployeeManager::PrivateData {
public:
    PrivateData() :
        employees(boost::unordered_map<uuid_t, Employee*>{}), relation_manager(RelationManager{}),
        salary_calculator(SalaryCalculator{employees, relation_manager}) {}

    template<typename... Args>
    std::array<Employee*, sizeof...(Args)> find_employees_by_ids(Args... args) {
        static_assert((std::is_same_v<Args, uuid_t> && ...),
                      "All arguments must be boost::uuids::uuid");

        std::array<Employee*, sizeof...(Args)> result{};

        size_t i = 0;
        ((result[i++] = (employees.find(args) != employees.end()) ? employees.at(args) : nullptr),
         ...);

        return result;
    }

public:
    std::mutex                              mtx;
    boost::unordered_map<uuid_t, Employee*> employees;

    RelationManager relation_manager;

    SalaryCalculator salary_calculator;
};

//! Construct an EmployeeManager object
EmployeeManager::EmployeeManager() : p_data_(std::make_unique<PrivateData>()) {}

//! Destruct an EmployeeManager object
EmployeeManager::~EmployeeManager() {
    for (auto it = p_data_->employees.begin(); it != p_data_->employees.end(); ++it) {
        delete it->second;
    }
    p_data_->employees.clear();
}

//! Registrate new employee
std::pair<uuid_t, bool> EmployeeManager::add_employee(const EmployeeDescr& description) {
    Employee* employee = Employee::create(description);
    if (employee == nullptr) {
        return {uuid_t{}, false};
    }

    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);
        p_data_->employees[employee->get_id()] = employee;
    }

    return {employee->get_id(), true};
}

//! Remove employee from registration list
bool EmployeeManager::remove_employee(const uuid_t& id) {
    auto it = p_data_->employees.find(id);
    if (it == p_data_->employees.end()) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);
        delete it->second;
        p_data_->employees.erase(it);
    }

    return true;
}

//! Find employee by it unique identifier
std::optional<EmployeeDescr> EmployeeManager::find_employee(const uuid_t& id) const {
    const Employee* p_employee = nullptr;

    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);

        auto it = p_data_->employees.find(id);
        if (it == p_data_->employees.end()) {
            return std::nullopt;
        }

        p_employee = it->second;
    }

    if (p_employee != nullptr) {
        EmployeeDescr descr{
            p_employee->get_type(), p_employee->get_base_salary(), p_employee->get_hire_date()};
        return descr;
    }
    return std::nullopt;
}

//! Add relation between chief and subordinate
bool EmployeeManager::add_subordination(const uuid_t& chief, const uuid_t& subordinate) {
    // 1.Validate on having such employees and employee category
    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);

        const auto employees = p_data_->find_employees_by_ids(chief, subordinate);
        if (std::any_of(employees.begin(), employees.end(), [&chief](const Employee* emp) -> bool {
                return emp == nullptr ||
                       (emp->get_id() == chief && emp->get_type() == EmployeeType::WORKER);
            })) {
            return false;
        }
    }

    // 2.Add
    return p_data_->relation_manager.add_relation(chief, subordinate);
}

//! Remove subordination relation between chief and subordinate
bool EmployeeManager::remove_subordination(const uuid_t& chief, const uuid_t& subordinate) {
    // 1.Validate on having such employees and employee category
    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);

        const auto employees = p_data_->find_employees_by_ids(chief, subordinate);
        if (std::any_of(employees.begin(), employees.end(), [&chief](const Employee* emp) -> bool {
                return emp == nullptr ||
                       (emp->get_id() == chief && emp->get_type() == EmployeeType::WORKER);
            })) {
            return false;
        }
    }

    // 2.Remove
    return p_data_->relation_manager.remove_relation(chief, subordinate);
}

//! Get employee chief
std::optional<uuid_t> EmployeeManager::get_chief(const uuid_t& id) const {
    // 1.Validate on having such employee
    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);

        const auto employees = p_data_->find_employees_by_ids(id);
        if (std::any_of(employees.begin(), employees.end(), [](const Employee* emp) -> bool {
                return emp == nullptr;
            })) {
            return std::nullopt;
        }
    }

    // 2.Find it chief
    return p_data_->relation_manager.get_chief(id);
}

//! Get employee direct subordinates
std::vector<uuid_t> EmployeeManager::get_direct_subordinates(const uuid_t& id) const {
    // 1.Validate on having such employee
    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);

        const auto employees = p_data_->find_employees_by_ids(id);
        if (std::any_of(employees.begin(), employees.end(), [](const Employee* emp) -> bool {
                return emp == nullptr;
            })) {
            return {};
        }
    }

    // 2.Find it direct subordinates
    return p_data_->relation_manager.get_direct_subordinates(id);
}

//! Get employee all subordinates
std::vector<uuid_t> EmployeeManager::get_all_subordinates(const uuid_t& id) const {
    // 1.Validate on having such employee
    {
        std::lock_guard<std::mutex> lock(p_data_->mtx);

        const auto employees = p_data_->find_employees_by_ids(id);
        if (std::any_of(employees.begin(), employees.end(), [](const Employee* emp) -> bool {
                return emp == nullptr;
            })) {
            return {};
        }
    }

    // 2.Find it all subordinates
    return p_data_->relation_manager.get_all_subordinates(id);
}

//! Calculate employee salary
std::pair<double, bool> EmployeeManager::calculate_employee_salary(const uuid_t& id,
                                                                   const date_t& date) const {
    // Pay attention: use lock for all function space due to lock employees storage for all
    // calculation time
    // TODO: but someone can change relation hierarchy (think about lock logic)

    // 1.Validate on having such employee
    std::lock_guard<std::mutex> lock(p_data_->mtx);

    const auto employees = p_data_->find_employees_by_ids(id);
    if (std::any_of(employees.begin(), employees.end(), [](const Employee* emp) -> bool {
            return emp == nullptr;
        })) {
        return {};
    }

    // 2.Calculate
    return p_data_->salary_calculator.calculate_month_salary(id, date);
}