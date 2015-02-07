/**
    Copyright 2015 Emil Maskovsky

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
    @file

    Cookies puzzle.

    @author Emil Maskovsky

    Determine the minimum time to reach the target with different types
    of cookies generating credit.

    Git repository: https://github.com/emaskovsky/puzzles-cookies
*/


#include <ctime>

#include <map>
#include <vector>

#include <iostream>
using namespace std;


typedef map< double, double > ValuesT;
typedef vector< unsigned > CountsT;


double minTimeOptimized(
    const ValuesT::const_iterator & start,
    const ValuesT::const_iterator & end,
    const CountsT::iterator & counts,
    const double total,
    const double cps = 0)
{
    if (start == end)
    {
        return (cps > 0) ? (total / cps) : 0.0;
    }
    const double G = start->first;
    const double P = start->second;
    const double CPS = (cps > 0) ? cps : G;
    ValuesT::const_iterator valuesNext = start;
    ++valuesNext;
    CountsT::iterator countsNext = counts;
    ++countsNext;
    // compute the minimum for the current status
    // (the result must not be worse)
    double timeMin = minTimeOptimized(
        valuesNext,
        end,
        countsNext,
        total,
        CPS);
    // time to get N factories
    double timeN = 0.0;
    // if we are at the last cookie, we can estimate the N
    if (valuesNext == end)
    {
        const double tmpN = total / P - CPS / G;
        static const unsigned MAX_INTEGER_VALUE = 0u - 1u;
        if (tmpN > MAX_INTEGER_VALUE)
        {
            cerr << "ERROR: Loop variable overflow!" << endl;
        }
        const unsigned finalN = (tmpN < 0)
                                ? 0
                                : static_cast< unsigned >(tmpN);
        // go the opposite side from lower numbers to bigger
        // (to mitigate the floating point imprecision)
        for (unsigned N = finalN; N > 0; --N)
        {
            // time to get the N-th factory from (N-1)-th
            // is added to the total time to get N
            timeN += P / ((N - 1) * G + CPS);
        }
        // time to get T with current N factories
        // (next sources checked recursively)
        const double timeMinNext = minTimeOptimized(
            valuesNext,
            end,
            countsNext,
            total,
            finalN * G + CPS);
        // and add the time to get the N factories
        *counts = finalN;
        timeMin = timeN + timeMinNext;
    }
    else
    {
        for (unsigned N = 1; /* empty */; ++N)
        {
            // time to get the N-th factory from (N-1)-th
            // is added to the total time to get N
            timeN += P / ((N - 1) * G + CPS);
            // time to get T with current N factories
            // (next sources checked recursively)
            const double timeMinNext = minTimeOptimized(
                valuesNext,
                end,
                countsNext,
                total,
                N * G + CPS);
            // and add the time to get the N factories
            const double timeTotal = timeN + timeMinNext;
            if (timeTotal >= timeMin)
            {
                // found the optimum - the time to get T with current N is
                // greater
                // than or equat to the previous minimum
                *counts = N - 1;
                break;
            }
            // found a new minimum
            timeMin = timeTotal;
        }
    }
    return timeMin;
}


double minTimeBruteForce(
    const ValuesT::const_iterator & start,
    const ValuesT::const_iterator & end,
    const CountsT::iterator & counts,
    const double total,
    const double cps = 0)
{
    if (start == end)
    {
        return (cps > 0) ? (total / cps) : 0.0;
    }
    const double G = start->first;
    const double P = start->second;
    const double CPS = (cps > 0) ? cps : G;
    ValuesT::const_iterator valuesNext = start;
    ++valuesNext;
    CountsT::iterator countsNext = counts;
    ++countsNext;
    // compute the minimum for the current status
    // (the result must not be worse)
    double timeMin = minTimeBruteForce(
        valuesNext,
        end,
        countsNext,
        total,
        CPS);
    // time to get N factories
    double timeN = 0.0;
    for (unsigned N = 1; /* empty */; ++N)
    {
        // time to get the N-th factory from (N-1)-th
        // is added to the total time to get N
        timeN += P / ((N - 1) * G + CPS);
        // time to get T with current N factories
        // (next sources checked recursively)
        const double timeMinNext = minTimeBruteForce(
            valuesNext,
            end,
            countsNext,
            total,
            N * G + CPS);
        // and add the time to get the N factories
        const double timeTotal = timeN + timeMinNext;
        if (timeTotal >= timeMin)
        {
            // found the optimum - the time to get T with current N is greater
            // than or equat to the previous minimum
            *counts = N - 1;
            break;
        }
        // found a new minimum
        timeMin = timeTotal;
    }
    return timeMin;
}


void reportCounts(const ValuesT & values, const CountsT & counts)
{
    size_t i = 0;
    ValuesT::const_iterator itV = values.begin();
    CountsT::const_iterator itC = counts.begin();
    while (itV != values.end())
    {
        cout << "\t#" << ++i << ":"
             << " gen = " << itV->first << " c/s,"
             << " price = " << itV->second << " c"
             << "\t... " << *itC << endl;
        ++itV, ++itC;
    }
}


void testMinTime(
    const ValuesT & values,
    const double total,
    const bool checkBf = true)
{
    CountsT counts(values.size(), 0);
    CountsT countsReference(values.size(), 0);

    double timeReference = 0.0;
    double elapsedBf = 0.0;
    if (checkBf)
    {
        clock_t startBf;
        const clock_t tmpBf = clock();
        do
        {
            startBf = clock();
        }
        while (tmpBf == startBf);

        timeReference = minTimeBruteForce(
            values.begin(),
            values.end(),
            countsReference.begin(),
            total);

        const clock_t endBf = clock();
        elapsedBf = static_cast< double >(endBf - startBf)
                    / CLOCKS_PER_SEC;
    }

    clock_t startOpt;
    const clock_t tmpOpt = clock();
    do
    {
        startOpt = clock();
    }
    while (tmpOpt == startOpt);

    const double time = minTimeOptimized(
        values.begin(),
        values.end(),
        counts.begin(),
        total);

    const clock_t endOpt = clock();
    const double elapsedOpt = static_cast< double >(endOpt - startOpt)
                              / CLOCKS_PER_SEC;

    // there is one cookie #1 already available at the beginning
    if (values.empty())
    {
        ++counts[0];
        ++countsReference[0];
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "Calculation for cookies list:" << endl;
    cout << "========================================" << endl;
    reportCounts(values, counts);
    cout << "----------------------------------------" << endl;
    cout << "Reached " << total << " credits in: " << time << " s" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Calculated in: " << elapsedOpt << " s";
    if (checkBf)
    {
        cout << " (brute force: " << elapsedBf << " s)";
    }
    cout << endl;

    if (checkBf && (fabs(timeReference - time) > 0.1))
    {
        cout << endl;
        cout << "WARNING:"
             << " Reference minimal time differs from the current result!"
             << endl;
        cout << "\tReference minimal time: " << timeReference << " s" << endl;
    }
    if (checkBf && (counts != countsReference))
    {
        cout << endl;
        cout << "WARNING:"
             << " Reference counts differ from the current result!"
             << endl;
        reportCounts(values, countsReference);
    }
    cout << endl;
}


void test_1source()
{
    ValuesT values;
    values[1] = 5;
    testMinTime(values, 100);
}


void test_2sources()
{
    ValuesT values;
    values[1] = 5;
    values[4] = 16;
    testMinTime(values, 100);
}


void test_2sources2ndExpensive()
{
    ValuesT values;
    values[1] = 5;
    values[4] = 21;
    testMinTime(values, 100);
}


void test_3sources()
{
    ValuesT values;
    values[1] = 5;
    values[4] = 16;
    values[20] = 75;
    testMinTime(values, 1000);
}


void test_3sourcesBigNumbers()
{
    ValuesT values;
    values[0.1] = 15;
    values[0.5] = 100;
    values[4.0] = 500;
    testMinTime(values, 1e6);
}


void test_MultiSources(
    const unsigned n,
    const double total,
    const bool checkBf = true)
{
    ValuesT values;
    double gen = 0.2;
    double price = 3;
    for (unsigned i = 0; i < n; ++i)
    {
        values[gen] = price;
        gen *= 4;
        price *= 3;
    }
    testMinTime(values, total, checkBf);
}


int main()
{
    test_1source();
    test_2sources();
    test_2sources2ndExpensive();
    test_3sources();
    test_3sourcesBigNumbers();
    test_MultiSources(5, 1e6);
    test_MultiSources(10, 1e6, false);
    test_MultiSources(20, 1e9, false);
    return EXIT_SUCCESS;
}


/* EOF */
