 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * cActionDefendAssist.cpp
 *
 *  Created on: May 28, 2017
 *      Author: Ivo Matthijssen
 */
#include "int/actions/cActionDefendAssist.hpp"

#include "int/stores/ballStore.hpp"
#include "int/cWorldModelInterface.hpp"
#include "int/stores/fieldDimensionsStore.hpp"
#include "int/utilities/trace.hpp"
#include "int/cTeamplayCommon.hpp"

using namespace teamplay;

cActionDefendAssist::cActionDefendAssist()
{
	intention.actionType = actionEnum::DEFEND_ASSIST;
}

cActionDefendAssist::~cActionDefendAssist()
{
}

// Always position between goal and ball, contrary to goalkeeper behavior
// Always face against ball
// If no ball was found, go outside penalty area in front of middle of the goal
// Returns: RUNNING always, action does not end

behTreeReturnEnum cActionDefendAssist::execute(const std::map<std::string, std::string> &parameters)
{
    try
    {
    	// ball position
        ball ball = ballStore::getBall();

        // own position
        Position2D myPos;
        cWorldModelInterface::getInstance().getOwnLocation(myPos);

        // By default goto center of goal on line y referenced to outside penalty-area
        Point2D penaltyarea_left_corner = fieldDimensionsStore::getFieldDimensions().getLocation(fieldPOI::OWN_PENALTYAREA_CORNER_LEFT);
		Point2D goalCenter = fieldDimensionsStore::getFieldDimensions().getLocation(fieldPOI::OWN_GOALLINE_CENTER);
		Position2D targetPos = Position2D(goalCenter.x, penaltyarea_left_corner.y + _LINE_OFFSET_NORMAL, M_PI_2); // Face forward

        // if ball is known, calculate new positions for targetPos
        if (ball.isLocationKnown())
        {
        	// update line y depending on how close the ball is to own goal
        	if (ball.getPosition().y < _BALL_THRESHOLD_LINE_OFFSET)
        	{
        		targetPos.y = penaltyarea_left_corner.y + _LINE_OFFSET_CLOSE_TO_GOAL;
        	}

            // projection of line on which defender assist moves
            Vector2D leftPosDefendAssist((fieldDimensionsStore::getFieldDimensions().getWidth() / -2.0), targetPos.y);
            Vector2D rightPosDefendAssist((fieldDimensionsStore::getFieldDimensions().getWidth() / 2.0), targetPos.y);

        	Point2D projectedGoalPost = fieldDimensionsStore::getFieldDimensions().getLocation(fieldPOI::OWN_GOALPOST_LEFTBACK);
        	if (ball.isAtLeftSide())
        	{
    	        projectedGoalPost = fieldDimensionsStore::getFieldDimensions().getLocation(fieldPOI::OWN_GOALPOST_RIGHTBACK);
        	}

        	Vector2D ballPos(ball.getPosition().x, ball.getPosition().y);
        	Vector2D projGoalPost(projectedGoalPost.x, projectedGoalPost.y);

			// intersect
			Vector2D intersectResult;
			if (intersect(ballPos, projGoalPost, leftPosDefendAssist, rightPosDefendAssist, intersectResult))
			{
				TRACE("intercept ActionDefendAssist: ") << std::to_string(intersectResult.x) << ", " << std::to_string(intersectResult.y);
	        	if (ball.isAtLeftSide())
	        	{
	        		//Defend Assist moves from x=0 to the right when ball is on left side
	        		targetPos.x = fmax(intersectResult.x, goalCenter.x);
	        	}
	        	else
	        	{
	        		//Defend Assist moves from x=0 to the left when ball is on right side
	        		targetPos.x = fmin(intersectResult.x, goalCenter.x);
	        	}
			}

			// facing ball
            double angle = angle_between_two_points_0_2pi(targetPos.x, targetPos.y, ballPos.x, ballPos.y);
            targetPos.phi = angle;
        }

        intention.x = targetPos.x;
        intention.y = targetPos.y;
        sendIntention();

        // ensure that we move according to the rules
        if (!isCurrentPosValid())
        {
            // Move towards the center, while maintaining angle
            moveTo(0.0, 0.0, targetPos.phi);
            return behTreeReturnEnum::RUNNING;
        }

        if (!isTargetPosInsideSafetyBoundaries(targetPos))
        {
            return behTreeReturnEnum::FAILED;
        }

        //this is new

        // if move is legal, move to calculated targetPos
        moveTo(targetPos.x, targetPos.y, targetPos.phi);
        return behTreeReturnEnum::RUNNING;
    }
    catch (std::exception &e)
    {
        TRACE_ERROR("Caught exception: ") << e.what();
        throw std::runtime_error(std::string("Linked to: ") + e.what());
    }

    return behTreeReturnEnum::FAILED;
}
