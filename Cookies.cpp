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


#include <map>

#include <iostream>
using namespace std;


typedef map< double, double > ValuesT;

double minTime(
    const ValuesT::const_iterator & start,
    const ValuesT::const_iterator & end,
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
    ValuesT::const_iterator next = start;
    ++next;
    // compute the minimum for the current status
    // (the result must not be worse)
    double timeMin = minTime(next, end, total, CPS);
    // time to get N factories
    double timeN = 0.0;
    for (unsigned N = 1; /* empty */; ++N)
    {
        // time to get the N-th factory from (N-1)-th is P/((N -1) * G + CPS)
        // and that is added to the total time to get N
        timeN += P / ((N - 1) * G + CPS);
        // time to get T with current N factories
        // (next sources checked recursively)
        const double timeMinNext = minTime(next, end, total, N * G + CPS);
        // and add the time to get the N factories
        const double timeTotal = timeN + timeMinNext;
        if (timeTotal >= timeMin)
        {
            // found the optimum - the time to get T with current N is greater
            // than or equat to the previous minimum
            break;
        }
        // found a new minimum
        timeMin = timeTotal;
    }
    return timeMin;
}


void testMinTime(const ValuesT & values, const double total)
{
    const double time = minTime(values.begin(), values.end(), total);
    cout << "Minimum time: " << time << " s" << endl;
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
    values[8] = 25;
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


int main()
{
    test_1source();
    test_2sources();
    test_2sources2ndExpensive();
    test_3sources();
    test_3sourcesBigNumbers();
    return EXIT_SUCCESS;
}


/* EOF */
