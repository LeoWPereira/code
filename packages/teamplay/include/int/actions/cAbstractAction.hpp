 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * cAbstractAction.hpp
 *
 *  Created on: Apr 30, 2016
 *      Author: Erik Kouters
 */

#ifndef CABSTRACTACTION_HPP_
#define CABSTRACTACTION_HPP_

#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <boost/assign/list_of.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/assign/ptr_map_inserter.hpp>

#include "int/types/intentionType.hpp"
#include "int/types/cDecisionTreeTypes.hpp"

#include "int/adapters/cHALInterface.hpp"
#include "int/adapters/cPathPlanningInterface.hpp"
#include "int/adapters/cShootPlanningInterface.hpp"

#include "area2D.hpp"
#include "vector2d.hpp"
#include "linepoint2D.hpp"

// These vectors contain the valid parameter values for different actions.
// e.g. a Move accepts a parameter "target", with as value "ball".
// The allowed values for the parameter target is listed here.
// poi == environment.poi
// area == environment.area

static std::string emptyValue = "tbd";
static std::string poiValue = "poi";
static std::string areaValue = "area";

// This list of values is part of the "default point of interest" list: defaultPOI
static std::vector<std::string> defaultPOI { "ball", "lastKnownBallLocation", "robot", "closestTeammember", "closestAttacker", "closestAttackerOnOppHalf", "closestAttackerToOppGoal", "closestDefender", "closestOpponent", "closestOpponentToOwnGoal", "potentialOppAttacker", poiValue };
// This list of values is part of the "default areas" list: defaultArea
static std::vector<std::string> defaultArea { areaValue };
// This list of values is part of the "default shoot types" list: defaultShootTypes
static std::vector<std::string> defaultShootTypes { "passTowardsNearestTeammember", "shootTowardsGoal", "passTowardsNearestAttacker", "passTowardsFurthestAttacker", "lobTowardsGoal", "passTowardsTipInPosition" };
// This list of values is part of the "motion profiles" defined by pathplanning.
static std::vector<std::string> defaultMotionProfiles { "normal", "setpiece" };

class cAbstractAction
{
	public:
		cAbstractAction();
		virtual ~cAbstractAction();

		virtual behTreeReturnEnum execute(const std::map<std::string, std::string> &parameters);
		virtual void setHALInterface (cHALInterface* halInterface);
		virtual void setPathPlanningInterface (cPathPlanningInterface* pathPlanningInterface);
		virtual void setShootPlanningInterface (cShootPlanningInterface* shootPlanningInterface);

		boost::optional<Position2D> getPos2DFromStr(const std::map<std::string, std::string> &parameters, std::string &param);
		boost::optional<Area2D> getArea2DFromStr(const std::map<std::string, std::string> &parameters, std::string &param);

		// key: parameters (e.g. "target")
		// value: pair( vector<string> allowedValues, bool optional ) (e.g. pair( {"ball", "robot"}, false ))
		std::map< std::string, std::pair< std::vector<std::string>, bool > > _actionParameters;

	protected:
		void moveTo(double x, double y, double phi, const std::string& motionProfile = "normal");
        void shoot (double shotPower);
        void lobShot (const Point2D& target);
        void stop();
        bool positionReached(double x, double y, double phi);
        bool positionReached(double x, double y, double phi, double xy_threshold, double phi_threshold);

        bool isCurrentPosValid() const;
        bool isTargetPosInsideSafetyBoundaries (const Position2D&) const;
        std::vector<polygon2D> getForbiddenAreas() const;
        std::vector<polygon2D> getForbiddenActionAreas() const;

        Point2D getPreferredPartOfGoal() const;
        void sendIntention();

        double XYpositionTolerance = 0.1;
        double PHIpositionTolerance = 0.05;
        intentionStruct intention;

	private:
        cHALInterface* hal;
		cPathPlanningInterface* pp;
        cShootPlanningInterface* sp;


};

#endif /* CABSTRACTACTION_HPP_ */
