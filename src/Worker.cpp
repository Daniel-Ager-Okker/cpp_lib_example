// relative includes
#include "Worker.h"

using namespace employee;

//! Worker object contructor
Worker::Worker(const EmployeeDescr& description) : Employee(description) {
    type_ = EmployeeType::WORKER;
}

//! Get employee type
EmployeeType Worker::get_type() const {
    return EmployeeType::WORKER;
}
