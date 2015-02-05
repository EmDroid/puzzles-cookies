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
        return 0.0;
    }
    const double T = total;
    const double G = start->first;
    const double P = start->second;
    const double CPS = (cps > 0) ? cps : G;
    // compute the minimum off the starting rate (cannot be worse)
    double timeMin = total / CPS;
    // time to get N factories
    double timeN = 0.0;
    for (unsigned N = 1; /* empty */; ++N)
    {
        // time to get the N-th factory from (N-1)-th is P/((N -1) * G + CPS)
        // and that is added to the total time to get N
        timeN += P / ((N - 1) * G + CPS);
        // time to get T with current N factories
        const double timeTotal = T / (N * G + CPS)
                                 // and added the time to get the N factories
                                 + timeN;
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


void test_1()
{
    ValuesT values;
    values[1] = 5;
    const double time = minTime(values.begin(), values.end(), 100);
    cout << "Minimum time: " << time << " s" << endl;
}


int main()
{
    test_1();
    return EXIT_SUCCESS;
}


/* EOF */
