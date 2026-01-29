// relative includes
#include "Foreman.h"

using namespace employee;

//! Foreman object contructor
Foreman::Foreman(const EmployeeDescr& description) : Employee(description) {
    type_ = EmployeeType::FOREMAN;
}

//! Get employee type
EmployeeType Foreman::get_type() const {
    return EmployeeType::FOREMAN;
}
