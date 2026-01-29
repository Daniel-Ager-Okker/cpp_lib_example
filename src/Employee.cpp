// relative includes
#include "Foreman.h"
#include "Manager.h"
#include "Worker.h"

// boost includes
#include <boost/uuid/uuid_generators.hpp>

using namespace employee;

Employee* Employee::create(const EmployeeDescr& description) {
    switch (description.type) {
        case EmployeeType::WORKER:
            return new Worker{description};

        case EmployeeType::FOREMAN:
            return new Foreman{description};

        case EmployeeType::MANAGER:
            return new Manager{description};

        default:
            return nullptr;
    }

    return nullptr;
}

//! Get employee unique identifier
const uuid_t& Employee::get_id() const {
    return id_;
}

//!  Get hire date
const date_t& Employee::get_hire_date() const {
    return hire_date_;
}

//! Get base salary
double Employee::get_base_salary() const {
    return base_salary_;
}

//! Employee object contructor
Employee::Employee(const EmployeeDescr& description) :
    hire_date_(description.hire_date), base_salary_(description.base_salary) {

    boost::uuids::random_generator gen;
    id_ = gen();
}