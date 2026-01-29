// relative includes
#include "Manager.h"

using namespace employee;

//! Manager object contructor
Manager::Manager(const EmployeeDescr& description) : Employee(description) {
    type_ = EmployeeType::MANAGER;
}

//! Get employee type
EmployeeType Manager::get_type() const {
    return EmployeeType::MANAGER;
}
