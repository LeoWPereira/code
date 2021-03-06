 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * cActionMove.cpp
 *
 *  Created on: Sep 15, 2015
 *      Author: Ivo Matthijssen
 */

#include "int/actions/cActionMove.hpp"

#include <string>

#include "int/stores/ballStore.hpp"
#include "int/stores/configurationStore.hpp"
#include "int/cTeamplayCommon.hpp"
#include "int/cWorldModelInterface.hpp"
#include "int/utilities/trace.hpp"


using namespace teamplay;

cActionMove::cActionMove()
{
    boost::assign::insert( _actionParameters )
        ("target", std::make_pair(defaultPOI, false) )
        ("facing", std::make_pair(defaultPOI, true) )
        ("distanceThreshold", std::make_pair(std::vector<std::string>{"float"}, true) )
        ("angleThreshold", std::make_pair(std::vector<std::string>{"float"}, true))
        ("motionProfile", std::make_pair(std::vector<std::string>{defaultMotionProfiles}, true))
        ;

    intention.actionType = actionEnum::MOVE;
}

cActionMove::~cActionMove()
{

}


/* \brief Move to 'target' (x,y)
 *
 * \param target              The target (x,y)
 * \param facing              [Optional] The target (x,y) to face. If no 'facing' was given, face the ball (if found).
 * \param distanceThreshold   [Optional] The allowed delta between the target position and current position
 * \param angleThreshold      [Optional] The allowed delta between the target angle and current angle
 *
 * \retval  RUNNING     if the robot is not at the target position
 *          PASSED      if the robot has reached the target position
 *          FAILED      if something went wrong
 */
behTreeReturnEnum cActionMove::execute(const std::map<std::string, std::string> &parameters)
{
    try
    {
        Position2D myPos;
        cWorldModelInterface::getInstance().getOwnLocation(myPos);

        // parameter "target" is the place to go.
        std::string targetStr("target");
        boost::optional<Position2D> target = getPos2DFromStr(parameters, targetStr);

        // parameter "facing" is the place to look at.
        std::string facingStr("facing");
        boost::optional<Position2D> facing = getPos2DFromStr(parameters, facingStr);

        // parameter "distanceThreshold" is the allowed delta between the target and the real position
        std::string distanceThresholdStr("distanceThreshold");
        double distanceThreshold = XYpositionTolerance;
        auto paramValPair = parameters.find(distanceThresholdStr);
        if (paramValPair != parameters.end())
        {
            std::string distanceThresholdVal = paramValPair->second;
            if (distanceThresholdVal.compare(emptyValue) != 0)
            {
                distanceThreshold = std::stod(distanceThresholdVal);
            }
        }

        // parameter "angleThreshold" is the allowed delta between the target and the real angle
        std::string angleThresholdStr("angleThreshold");
        double angleThreshold = PHIpositionTolerance;
        paramValPair = parameters.find(angleThresholdStr);
        if (paramValPair != parameters.end())
        {
            std::string angleThresholdVal = paramValPair->second;
            if (angleThresholdVal.compare(emptyValue) != 0)
            {
                angleThreshold = std::stod(angleThresholdVal);
            }
        }

        // parameter "motionProfile" defines with which profile to move with (e.g., normal play or more careful during a setpiece)
        std::string motionProfileStr("motionProfile");
        std::string motionProfileValue = "normal";
        paramValPair = parameters.find(motionProfileStr);
        if (paramValPair != parameters.end())
        {
            motionProfileValue = paramValPair->second;
        }

        if (target)
        {
            Position2D targetPos = *target;

            intention.x = targetPos.x;
            intention.y = targetPos.y;
            sendIntention();

            // ensure that we move according to the rules
            if (!isCurrentPosValid())
            {
                // Move towards the center, while maintaining angle and motion profile
                moveTo(0.0, 0.0, targetPos.phi, motionProfileValue);
                return behTreeReturnEnum::RUNNING;
            }

            if (!isTargetPosInsideSafetyBoundaries(targetPos))
            {
                return behTreeReturnEnum::FAILED;
            }

            if (facing)
            {
                // Move to 'target', while facing 'facing'.
                Position2D facingPos = *facing;

                // Compute angle from target to facing
                double angle = angle_between_two_points_0_2pi(targetPos.x, targetPos.y, facingPos.x, facingPos.y);
                targetPos.phi = angle;
            }
            else
            {
                // No 'facing' parameter given. Default to looking at the ball.

                ball ball = ballStore::getBall();
                if (ball.isLocationKnown())
                {
                    Point3D ballPos = ball.getPosition();

                    // Compute angle from target to ball
                    double angle = angle_between_two_points_0_2pi(targetPos.x, targetPos.y, ballPos.x, ballPos.y);
                    targetPos.phi = angle;
                }
                else
                {
                    // No balls found. Do not turn.
                    targetPos.phi = myPos.phi;
                }
            }

            double wideAngleThreshold = angleThreshold + teamplay::configurationStore::getConfiguration().getShootTimerAngleThreshold();
            if (!positionReached(targetPos.x, targetPos.y, targetPos.phi, distanceThreshold, wideAngleThreshold))
            {
                _timer.reset();
            }

            // Previously determined targetPos, including the 'facing' (phi).
            // Now move there, if not already reached.
            if (positionReached(targetPos.x, targetPos.y, targetPos.phi, distanceThreshold, angleThreshold))
            {
                // Target reached. Do nothing and return PASSED
                TRACE("cActionMove PASSED (reason: target reached)");
                moveTo(myPos.x, myPos.y, myPos.phi);
                return behTreeReturnEnum::PASSED;
            }
            else
            {
                if(_timer.hasElapsed(teamplay::configurationStore::getConfiguration().getShootTimer()))
                {
                    // Target not reached, but timer elapsed. Do nothing and return PASSED
                    TRACE("cActionMove PASSED (reason: timer elapsed)");
                    moveTo(myPos.x, myPos.y, myPos.phi);
                    return behTreeReturnEnum::PASSED;
                }
                else
                {
                    /*
                     * Target not reached and timer not elapsed
                     *
                     * Before moving, calculate new angle incorporating the threshold as well
                     * Reason is to 'pull' robot over the threshold; avoiding long waits
                     * before the robot actually reaches the threshold.
                     *
                     */
                    //if((project_angle_mpi_pi(targetPos.phi) - project_angle_mpi_pi(myPos.phi)) > 0.0)
                    //{
                    //    targetPos.phi = project_angle_0_2pi(targetPos.phi + PHIpositionTolerance);
                    //}
                    //else
                    //{
                    //    targetPos.phi = project_angle_0_2pi(targetPos.phi - PHIpositionTolerance);
                    //}

                    //moveTo and return RUNNING
                    moveTo(targetPos.x, targetPos.y, targetPos.phi, motionProfileValue);
                    return behTreeReturnEnum::RUNNING;
                }
            }
        }

    }
    catch (std::exception &e)
    {
        TRACE_ERROR("Caught exception: ") << e.what();
        throw std::runtime_error(std::string("Linked to: ") + e.what());
    }

    return behTreeReturnEnum::FAILED;
}
