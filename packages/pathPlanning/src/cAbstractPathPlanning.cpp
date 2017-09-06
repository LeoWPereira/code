 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * cAbstractPathPlanning.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: Tim Kouters
 */

#include "int/cAbstractPathPlanning.hpp"
#include <boost/thread/thread.hpp>

cAbstractPathPlanning::cAbstractPathPlanning(cPathPlanningMain* main)
{
    TRACE(">");
    _main = main;
    _dt = 0.0; // Recalculated every iteration due to event-based pathPlanning.
    gettimeofday(& _prevTimestamp, NULL);
    _prev_vel = Velocity2D();
    TRACE("<");
}

cAbstractPathPlanning::~cAbstractPathPlanning()
{
}

void cAbstractPathPlanning::execute()
{
    //TRACE(">");


    // Set target position in ppData
    Position2D targetPos;
    _main->_ppData->getTarget(targetPos);
    _ppData.pos = targetPos;

    std::list<cAbstractPathPlanning*>::iterator it;
    for (it = _ppBlocks.begin(); it != _ppBlocks.end(); ++it)
    {
        // Compute new _dt for each algorithm
        (*it)->computeDt();

        (*it)->setData(_ppData);
        (*it)->execute();
        _ppData = (*it)->getData();

        // NAGOYA CONTAINMENT
        // Removed due to possible starvation because of the amount of obstacles from forbiddenAreas
        // Throw interruption exception if interruption requested
        //boost::this_thread::interruption_point();
    }

    _main->_ppData->publishSpeed(_ppData.vel);


    //TRACE("<");
}

void cAbstractPathPlanning::setData(pp_data_struct_t &ppData)
{
    //TRACE(">");
    _ppData = ppData;
    //TRACE("<");
}

pp_data_struct_t cAbstractPathPlanning::getData()
{
    //TRACE(">");
    //TRACE("<");
    return _ppData;
}

void cAbstractPathPlanning::computeDt()
{
    // Compute new _dt

    timeval time_now;
    gettimeofday(&time_now, NULL);

    _dt = diff_seconds(time_now, _prevTimestamp);

    _prevTimestamp = time_now;
}