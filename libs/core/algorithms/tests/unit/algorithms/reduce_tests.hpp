//  Copyright (c) 2014-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/modules/testing.hpp>
#include <hpx/parallel/algorithms/reduce.hpp>

#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "test_utils.hpp"

int seed = std::random_device{}();
std::mt19937 gen(seed);

///////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_reduce1(IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t val(42);
    auto op = [val](auto v1, auto v2) { return v1 * v2; };

    std::size_t r1 =
        hpx::reduce(iterator(std::begin(c)), iterator(std::end(c)), val, op);

    // verify values
    std::size_t r2 = std::accumulate(std::begin(c), std::end(c), val, op);
    HPX_TEST_EQ(r1, r2);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce1(ExPolicy policy, IteratorTag)
{
    static_assert(hpx::is_execution_policy<ExPolicy>::value,
        "hpx::is_execution_policy<ExPolicy>::value");

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t val(42);
    auto op = [val](auto v1, auto v2) { return v1 * v2; };

    std::size_t r1 = hpx::reduce(
        policy, iterator(std::begin(c)), iterator(std::end(c)), val, op);

    // verify values
    std::size_t r2 = std::accumulate(std::begin(c), std::end(c), val, op);
    HPX_TEST_EQ(r1, r2);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce1_async(ExPolicy p, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t val(42);
    auto op = [val](auto v1, auto v2) { return v1 * v2; };

    hpx::future<std::size_t> f =
        hpx::reduce(p, iterator(std::begin(c)), iterator(std::end(c)), val, op);
    f.wait();

    // verify values
    std::size_t r2 = std::accumulate(std::begin(c), std::end(c), val, op);
    HPX_TEST_EQ(f.get(), r2);
}

///////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_reduce2(IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t const val(42);
    std::size_t r1 =
        hpx::reduce(iterator(std::begin(c)), iterator(std::end(c)), val);

    // verify values
    std::size_t r2 = std::accumulate(std::begin(c), std::end(c), val);
    HPX_TEST_EQ(r1, r2);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce2(ExPolicy policy, IteratorTag)
{
    static_assert(hpx::is_execution_policy<ExPolicy>::value,
        "hpx::is_execution_policy<ExPolicy>::value");

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t const val(42);
    std::size_t r1 = hpx::reduce(
        policy, iterator(std::begin(c)), iterator(std::end(c)), val);

    // verify values
    std::size_t r2 = std::accumulate(std::begin(c), std::end(c), val);
    HPX_TEST_EQ(r1, r2);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce2_async(ExPolicy p, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t const val(42);
    hpx::future<std::size_t> f =
        hpx::reduce(p, iterator(std::begin(c)), iterator(std::end(c)), val);
    f.wait();

    // verify values
    std::size_t r2 = std::accumulate(std::begin(c), std::end(c), val);
    HPX_TEST_EQ(f.get(), r2);
}

///////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_reduce3(IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t r1 =
        hpx::reduce(iterator(std::begin(c)), iterator(std::end(c)));

    // verify values
    std::size_t r2 =
        std::accumulate(std::begin(c), std::end(c), std::size_t(0));
    HPX_TEST_EQ(r1, r2);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce3(ExPolicy policy, IteratorTag)
{
    static_assert(hpx::is_execution_policy<ExPolicy>::value,
        "hpx::is_execution_policy<ExPolicy>::value");

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    std::size_t r1 =
        hpx::reduce(policy, iterator(std::begin(c)), iterator(std::end(c)));

    // verify values
    std::size_t r2 =
        std::accumulate(std::begin(c), std::end(c), std::size_t(0));
    HPX_TEST_EQ(r1, r2);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce3_async(ExPolicy p, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    hpx::future<std::size_t> f =
        hpx::reduce(p, iterator(std::begin(c)), iterator(std::end(c)));
    f.wait();

    // verify values
    std::size_t r2 =
        std::accumulate(std::begin(c), std::end(c), std::size_t(0));
    HPX_TEST_EQ(f.get(), r2);
}

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_reduce_exception(ExPolicy policy, IteratorTag)
{
    static_assert(hpx::is_execution_policy<ExPolicy>::value,
        "hpx::is_execution_policy<ExPolicy>::value");

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    bool caught_exception = false;
    try
    {
        hpx::reduce(policy, iterator(std::begin(c)), iterator(std::end(c)),
            std::size_t(42), [](auto v1, auto v2) {
                return throw std::runtime_error("test"), v1 + v2;
            });

        HPX_TEST(false);
    }
    catch (hpx::exception_list const& e)
    {
        caught_exception = true;
        test::test_num_exceptions<ExPolicy, IteratorTag>::call(policy, e);
    }
    catch (...)
    {
        HPX_TEST(false);
    }

    HPX_TEST(caught_exception);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce_exception_async(ExPolicy p, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    bool caught_exception = false;
    bool returned_from_algorithm = false;
    try
    {
        hpx::future<void> f =
            hpx::reduce(p, iterator(std::begin(c)), iterator(std::end(c)),
                std::size_t(42), [](auto v1, auto v2) {
                    return throw std::runtime_error("test"), v1 + v2;
                });
        returned_from_algorithm = true;
        f.get();

        HPX_TEST(false);
    }
    catch (hpx::exception_list const& e)
    {
        caught_exception = true;
        test::test_num_exceptions<ExPolicy, IteratorTag>::call(p, e);
    }
    catch (...)
    {
        HPX_TEST(false);
    }

    HPX_TEST(caught_exception);
    HPX_TEST(returned_from_algorithm);
}

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_reduce_bad_alloc(ExPolicy policy, IteratorTag)
{
    static_assert(hpx::is_execution_policy<ExPolicy>::value,
        "hpx::is_execution_policy<ExPolicy>::value");

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    bool caught_exception = false;
    try
    {
        hpx::reduce(policy, iterator(std::begin(c)), iterator(std::end(c)),
            std::size_t(42), [](auto v1, auto v2) {
                return throw std::bad_alloc(), v1 + v2;
            });

        HPX_TEST(false);
    }
    catch (std::bad_alloc const&)
    {
        caught_exception = true;
    }
    catch (...)
    {
        HPX_TEST(false);
    }

    HPX_TEST(caught_exception);
}

template <typename ExPolicy, typename IteratorTag>
void test_reduce_bad_alloc_async(ExPolicy p, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    bool caught_exception = false;
    bool returned_from_algorithm = false;
    try
    {
        hpx::future<void> f =
            hpx::reduce(p, iterator(std::begin(c)), iterator(std::end(c)),
                std::size_t(42), [](auto v1, auto v2) {
                    return throw std::bad_alloc(), v1 + v2;
                });
        returned_from_algorithm = true;
        f.get();

        HPX_TEST(false);
    }
    catch (std::bad_alloc const&)
    {
        caught_exception = true;
    }
    catch (...)
    {
        HPX_TEST(false);
    }

    HPX_TEST(caught_exception);
    HPX_TEST(returned_from_algorithm);
}
