 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * cActionEnumTypes.hpp
 *
 *  Created on: Jul 22, 2017
 *      Author: Tim Kouters
 */

#ifndef CACTIONENUMTYPES_HPP_
#define CACTIONENUMTYPES_HPP_

enum class actionEnum
{
    INVALID = 0,
    SUCCESS,
    MOVE_WHILE_TURNING,
    MOVE,
    STOP,
    SHOOT,
    TURN,
    POSITION_BEFORE_POI,
    POSITION_BEHIND_POI,
    FACE_NEAREST_TEAMMEMBER,
    GET_BALL,
    GOALKEEPER,
    MOVE_TO_FREE_SPOT,
	MOVE_TO_PENALTY_ANGLE,
    INTERCEPT_BALL,
	AVOID_POI,
	GET_BALL_ON_VECTOR,
	LONG_TURN_TO_GOAL,
	AIM_FOR_SHOT_ON_GOAL,
	DEFEND_ASSIST,

    SIZE_OF_ENUM
};


#endif /* CACTIONENUMTYPES_HPP_ */
