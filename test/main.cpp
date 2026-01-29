// google test includes
#include <gtest/gtest.h>

// lib includes
#include <employee_lib/EmployeeManager.h>

// boost includes
#include <boost/uuid/uuid_generators.hpp>

// C++ includes
#include <array>
#include <optional>
#include <tuple>

using employee::date_t;
using employee::EmployeeDescr;
using employee::EmployeeManager;
using employee::EmployeeType;
using employee::uuid_t;

namespace bg = boost::gregorian;

//! Common employee descriptions for tests
static EmployeeDescr WORKER_DESCR{EmployeeType::WORKER, 100000.0, date_t{2026, 1, 1}};
static EmployeeDescr FOREMAN_DESCR{EmployeeType::FOREMAN, 200000.0, date_t{2026, 1, 1}};
static EmployeeDescr MANAGER_DESCR{EmployeeType::MANAGER, 300000.0, date_t{2026, 1, 1}};

//! Helper for generate some uuid
static uuid_t __generate_uuid();

//! Helper for add few emplyees
template<size_t N>
auto __add_few_employees(EmployeeManager& m, const EmployeeDescr& descr) {
    std::array<uuid_t, N> uuids;

    for (size_t i = 0; i < N; ++i) {
        auto [uid, ok] = m.add_employee(descr);
        EXPECT_TRUE(ok);

        uuids[i] = uid;
    }

    return [&uuids]<size_t... I>(std::index_sequence<I...>) {
        return std::tuple{uuids[I]...};
    }(std::make_index_sequence<N>{});
}

TEST(main_suite, add_employee) {
    EmployeeManager manager{};

    auto [id1, id2] = __add_few_employees<2>(manager, FOREMAN_DESCR);
    EXPECT_TRUE(id1 != id2);
}

TEST(main_suite, remove_employee) {
    EmployeeManager manager{};

    // 1.Remove non-existen
    const uuid_t some_id = __generate_uuid();

    bool removed = manager.remove_employee(some_id);
    EXPECT_FALSE(removed);

    // 2.Registrate some employee and then remove it
    auto [id] = __add_few_employees<1>(manager, FOREMAN_DESCR);

    removed = manager.remove_employee(id);
    EXPECT_TRUE(removed);
}

TEST(main_suite, find_employee) {
    EmployeeManager manager{};

    // 1.Find non-existen
    const uuid_t some_id = __generate_uuid();

    std::optional<EmployeeDescr> found = manager.find_employee(some_id);
    EXPECT_EQ(std::nullopt, found);

    // 2.Registrate some employee and then find it
    auto [id] = __add_few_employees<1>(manager, FOREMAN_DESCR);

    found = manager.find_employee(id);
    EXPECT_TRUE(found != std::nullopt);

    EmployeeDescr found_descr = found.value();
    EXPECT_EQ(FOREMAN_DESCR.type, found_descr.type);
    EXPECT_EQ(FOREMAN_DESCR.base_salary, found_descr.base_salary);
    EXPECT_EQ(FOREMAN_DESCR.hire_date, found_descr.hire_date);

    // 3.Remove it and try to find again
    const bool removed = manager.remove_employee(id);
    EXPECT_TRUE(removed);

    found = manager.find_employee(id);
    EXPECT_EQ(std::nullopt, found);
}

TEST(main_suite, add_subordination) {
    EmployeeManager manager{};

    // 1.Registrate some employees
    auto [chief, sub] = __add_few_employees<2>(manager, FOREMAN_DESCR);

    // 2.Test cases
    const uuid_t some_id = __generate_uuid();
    EXPECT_FALSE(manager.add_subordination(some_id, sub));
    EXPECT_FALSE(manager.add_subordination(chief, some_id));
    EXPECT_FALSE(manager.add_subordination(chief, chief));
    EXPECT_TRUE(manager.add_subordination(chief, sub));
    EXPECT_FALSE(manager.add_subordination(chief, sub));

    // 3.Cycle case
    auto [uid_a, uid_b, uid_c] = __add_few_employees<3>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(uid_a, uid_b));
    EXPECT_TRUE(manager.add_subordination(uid_b, uid_c));
    EXPECT_FALSE(manager.add_subordination(uid_c, uid_a));

    // 4.Case on worker category (can't have subordinates)
    EmployeeDescr descr = FOREMAN_DESCR;
    descr.type          = EmployeeType::WORKER;

    uuid_t wroker_chief, worker_sub;
    bool   _;

    std::tie(wroker_chief, _) = manager.add_employee(descr);
    std::tie(worker_sub, _)   = manager.add_employee(descr);

    EXPECT_FALSE(manager.add_subordination(wroker_chief, worker_sub));
}

TEST(main_suite, remove_subordination) {
    EmployeeManager manager{};

    // 1.Case OK
    auto [chief, sub] = __add_few_employees<2>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(chief, sub));
    EXPECT_TRUE(manager.remove_subordination(chief, sub));

    // 2.Case non-existen
    uuid_t non_existen_chief = __generate_uuid();
    EXPECT_FALSE(manager.remove_subordination(non_existen_chief, sub));

    uuid_t non_existen_sub = __generate_uuid();
    EXPECT_FALSE(manager.remove_subordination(chief, non_existen_sub));

    // 3.Case wrong chief
    auto [wrong_chief] = __add_few_employees<1>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(chief, sub));
    EXPECT_FALSE(manager.remove_subordination(wrong_chief, sub));

    // 4.Case wrong subordinate
    auto [wrong_sub] = __add_few_employees<1>(manager, FOREMAN_DESCR);

    EXPECT_FALSE(manager.remove_subordination(chief, wrong_sub));

    // 5.Case chain (a->b->c)
    auto [a, b, c] = __add_few_employees<3>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(a, b));
    EXPECT_TRUE(manager.add_subordination(b, c));

    EXPECT_FALSE(manager.remove_subordination(a, c)); // 'c' can have only one chief!

    EXPECT_TRUE(
        manager.remove_subordination(b, c)); // at this time 'c' does not have 'b' as a chief
    EXPECT_FALSE(manager.remove_subordination(a, c)); // now 'c' can have chief 'a'

    EXPECT_FALSE(manager.remove_subordination(c, b)); // cycle validation again

    // 6.Case self-removal
    EXPECT_FALSE(manager.add_subordination(a, a));
}

TEST(main_suite, get_chief) {
    EmployeeManager manager{};

    // 1.Case OK
    auto [chief, sub] = __add_few_employees<2>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(chief, sub));
    EXPECT_EQ(chief, manager.get_chief(sub).value());

    // 2.Case non-existen
    uuid_t non_existen_sub = __generate_uuid();
    EXPECT_EQ(std::nullopt, manager.get_chief(non_existen_sub));

    // 3.Case existen employee without chief
    std::tie(sub) = __add_few_employees<1>(manager, FOREMAN_DESCR);
    EXPECT_EQ(std::nullopt, manager.get_chief(sub));
}

TEST(main_suite, get_direct_subordinates) {
    EmployeeManager manager{};

    // 1.Case non-existen
    uuid_t non_existen_sub = __generate_uuid();
    EXPECT_TRUE(manager.get_direct_subordinates(non_existen_sub).empty());

    // 2.Case has direct subordinates
    auto [chief, sub1, sub2, sub3] = __add_few_employees<4>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(chief, sub1));
    EXPECT_EQ(manager.get_direct_subordinates(chief).size(), 1);

    EXPECT_TRUE(manager.add_subordination(chief, sub2));
    EXPECT_EQ(manager.get_direct_subordinates(chief).size(), 2);

    EXPECT_TRUE(manager.add_subordination(chief, sub3));
    EXPECT_EQ(manager.get_direct_subordinates(chief).size(), 3);

    EXPECT_TRUE(manager.remove_subordination(chief, sub3));
    EXPECT_EQ(manager.get_direct_subordinates(chief).size(), 2);

    // 3.Case only one direct subordinate, but few indirect
    EXPECT_TRUE(manager.remove_subordination(chief, sub2));

    auto [sub4, sub5, sub6, sub7] = __add_few_employees<4>(manager, FOREMAN_DESCR);
    for (const uuid_t& s : {sub4, sub5, sub6, sub7}) {
        EXPECT_TRUE(manager.add_subordination(sub1, s));
    }

    EXPECT_TRUE(manager.get_direct_subordinates(chief).size() == 1);
}

TEST(main_suite, get_all_subordinates) {
    EmployeeManager manager{};

    // 1.Case non-existen
    uuid_t non_existen_sub = __generate_uuid();
    EXPECT_TRUE(manager.get_all_subordinates(non_existen_sub).empty());

    // 2.Case has only direct subordinates
    auto [chief, sub1, sub2, sub3] = __add_few_employees<4>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(chief, sub1));
    EXPECT_TRUE(manager.add_subordination(chief, sub2));
    EXPECT_TRUE(manager.add_subordination(chief, sub3));

    EXPECT_EQ(manager.get_all_subordinates(chief).size(), 3);

    // 3.Case a->b->c->d
    auto [a, b, c, d] = __add_few_employees<4>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(a, b));
    EXPECT_TRUE(manager.add_subordination(b, c));
    EXPECT_TRUE(manager.add_subordination(c, d));

    EXPECT_EQ(manager.get_all_subordinates(a).size(), 3);
    EXPECT_EQ(manager.get_all_subordinates(b).size(), 2);

    // 4.Tree case
    //      M
    //     / \
    //    N   P
    //   /   / \
    //  Q   R   S
    auto [m, n, p, q, r, s] = __add_few_employees<6>(manager, FOREMAN_DESCR);

    EXPECT_TRUE(manager.add_subordination(m, n));
    EXPECT_TRUE(manager.add_subordination(m, p));

    EXPECT_TRUE(manager.add_subordination(n, q));

    EXPECT_TRUE(manager.add_subordination(p, r));
    EXPECT_TRUE(manager.add_subordination(p, s));

    EXPECT_EQ(manager.get_all_subordinates(m).size(), 5);
    EXPECT_EQ(manager.get_all_subordinates(n).size(), 1);
    EXPECT_EQ(manager.get_all_subordinates(p).size(), 2);

    // 5.Employee does not have any subordinate
    EXPECT_EQ(manager.get_all_subordinates(q).size(), 0);
    EXPECT_EQ(manager.get_all_subordinates(r).size(), 0);
    EXPECT_EQ(manager.get_all_subordinates(s).size(), 0);
}

TEST(main_suite, calculate_employee_salary_worker) {
    EmployeeManager manager{};

    // Corner case: no such employee
    auto [_, ok] = manager.calculate_employee_salary(__generate_uuid(), FOREMAN_DESCR.hire_date);
    EXPECT_FALSE(ok);

    auto [worker_id] = __add_few_employees<1>(manager, WORKER_DESCR);

    // 1.Calculation date less then hire
    date_t calc_date{2024, 1, 1};
    std::tie(_, ok) = manager.calculate_employee_salary(worker_id, calc_date);
    EXPECT_FALSE(ok);

    // 2.One month of work
    calc_date = date_t{WORKER_DESCR.hire_date.year(), WORKER_DESCR.hire_date.month(), 1};

    double worker_salary;
    std::tie(worker_salary, ok) = manager.calculate_employee_salary(worker_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(worker_salary, WORKER_DESCR.base_salary, 1e-10);

    // 3.One year of work
    calc_date += bg::years(1);

    std::tie(worker_salary, ok) = manager.calculate_employee_salary(worker_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(worker_salary, 1.1 * WORKER_DESCR.base_salary, 1e-10);

    // 4.Eleven years of work
    calc_date += bg::years(10);

    std::tie(worker_salary, ok) = manager.calculate_employee_salary(worker_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(worker_salary, 2 * WORKER_DESCR.base_salary, 1e-10);
}

TEST(main_suite, calculate_employee_salary_foreman) {
    EmployeeManager manager{};

    auto [foreman_id] = __add_few_employees<1>(manager, FOREMAN_DESCR);

    double calculated_salary;
    bool   ok;

    // 1.Calculation date less then hire
    date_t calc_date{2024, 1, 1};
    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_FALSE(ok);

    // 2.Case when there are no subordinates

    // 2.1.One month of work
    calc_date = FOREMAN_DESCR.hire_date;

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(calculated_salary, FOREMAN_DESCR.base_salary, 1e-10);

    // 2.2.One year of work
    calc_date += bg::years(1);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(calculated_salary, 1.05 * FOREMAN_DESCR.base_salary, 1e-10);

    // 2.3.Eleven years of work
    calc_date += bg::years(10);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(calculated_salary, 1.4 * FOREMAN_DESCR.base_salary, 1e-10);

    // 3.Case there are three direct subordinates (1 - foreman without subordinates and 2 workers)
    auto [sub_foreman_id] = __add_few_employees<1>(manager, FOREMAN_DESCR);
    EXPECT_TRUE(manager.add_subordination(foreman_id, sub_foreman_id));

    auto [sub_worker_1, sub_worker_2] = __add_few_employees<2>(manager, WORKER_DESCR);
    EXPECT_TRUE(manager.add_subordination(foreman_id, sub_worker_1));
    EXPECT_TRUE(manager.add_subordination(foreman_id, sub_worker_2));

    // 3.1.One month of work
    calc_date = FOREMAN_DESCR.hire_date;

    double expected = FOREMAN_DESCR.base_salary +
                      0.07 * (FOREMAN_DESCR.base_salary + 2 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);

    // 3.2.One year of work
    calc_date += bg::years(1);

    expected = 1.05 * FOREMAN_DESCR.base_salary +
               0.07 * (1.05 * FOREMAN_DESCR.base_salary + 2 * 1.1 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);

    // 3.3.Eleven years of work
    calc_date += bg::years(10);

    expected = 1.4 * FOREMAN_DESCR.base_salary +
               0.07 * (1.4 * FOREMAN_DESCR.base_salary + 2 * 2 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(foreman_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);
}

TEST(main_suite, calculate_employee_salary_manager) {
    EmployeeManager manager{};

    auto [manager_id] = __add_few_employees<1>(manager, MANAGER_DESCR);

    double calculated_salary;
    bool   ok;

    // 1.Calculation date less then hire
    date_t calc_date{2024, 1, 1};
    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_FALSE(ok);

    // 2.Case when there are no subordinates

    // 2.1.One month of work
    calc_date = MANAGER_DESCR.hire_date;

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(calculated_salary, MANAGER_DESCR.base_salary, 1e-10);

    // 2.2.One year of work
    calc_date += bg::years(1);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(calculated_salary, MANAGER_DESCR.base_salary, 1e-10);

    // 2.3.Eleven years of work
    calc_date += bg::years(10);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_TRUE(ok);
    EXPECT_NEAR(calculated_salary, MANAGER_DESCR.base_salary, 1e-10);

    // 3.Case there are three direct subordinates (1 - foreman without subordinates and 2 workers)
    auto [sub_foreman_id] = __add_few_employees<1>(manager, FOREMAN_DESCR);
    EXPECT_TRUE(manager.add_subordination(manager_id, sub_foreman_id));

    auto [sub_worker_1, sub_worker_2] = __add_few_employees<2>(manager, WORKER_DESCR);
    EXPECT_TRUE(manager.add_subordination(manager_id, sub_worker_1));
    EXPECT_TRUE(manager.add_subordination(manager_id, sub_worker_2));

    // 3.1.One month of work
    calc_date = MANAGER_DESCR.hire_date;

    double expected = MANAGER_DESCR.base_salary +
                      0.03 * (FOREMAN_DESCR.base_salary + 2 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);

    // 3.2.One year of work
    calc_date += bg::years(1);

    expected = MANAGER_DESCR.base_salary +
               0.03 * (1.05 * FOREMAN_DESCR.base_salary + 2 * 1.1 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);

    // 3.3.Eleven years of work
    calc_date += bg::years(10);

    expected = MANAGER_DESCR.base_salary +
               0.03 * (1.4 * FOREMAN_DESCR.base_salary + 2 * 2 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);

    // 4.Add two indirect subordinates (eleven years of owrk)
    auto [sub_worker_3, sub_worker_4] = __add_few_employees<2>(manager, WORKER_DESCR);
    EXPECT_TRUE(manager.add_subordination(sub_foreman_id, sub_worker_3));
    EXPECT_TRUE(manager.add_subordination(sub_foreman_id, sub_worker_4));

    expected = MANAGER_DESCR.base_salary +
               0.03 * (1.4 * FOREMAN_DESCR.base_salary + 0.07 * 2 * 2 * WORKER_DESCR.base_salary +
                       4 * 2 * WORKER_DESCR.base_salary);

    std::tie(calculated_salary, ok) = manager.calculate_employee_salary(manager_id, calc_date);
    EXPECT_NEAR(calculated_salary, expected, 1e-10);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

uuid_t __generate_uuid() {
    boost::uuids::random_generator gen;
    return gen();
}
