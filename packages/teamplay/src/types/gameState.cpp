 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * gameState.cpp
 *
 *  Created on: Jul 25, 2016
 *      Author: Coen Tempelaars
 */

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

#include "int/types/gameState.hpp"

using namespace teamplay;


static bool isValid(const governingGameState& g, const governingMatchState& p, const setpieceOwner& o, const setpieceType& t)
{
    bool validGameState = false;

    /* Is the combination of inputs valid? */
    if (  (g == governingGameState::INVALID)
       || (g == governingGameState::NEUTRAL_PLAYING)
       || (g == governingGameState::NEUTRAL_STOPPED)  )
    {
        // Restrictions:
        // - For setpiece settings, the governing gamestate must be setpiece preparing/executing. 
        //   Otherwise, setpieces settings must be NONE.
        // - In case of out of play state, we cannot be playing (NEUTRAL_PLAYING => IN_PLAY)
        validGameState = ((o == setpieceOwner::NONE) && (t == setpieceType::NONE)) && ((g != governingGameState::NEUTRAL_PLAYING) || (p == governingMatchState::IN_MATCH));
    }
    else if (  (g == governingGameState::SETPIECE_PREPARING)
            || (g == governingGameState::SETPIECE_EXECUTING)  )
    {
        // Restrictions:
        // - The setpiecetype must be set
        // - (setpieceOwner::NONE <==> setpieceType::DROPPED_BALL AND NOT setpieceType::NONE), i.e.:
        //   If there is no setpiece owner, the ball must have been dropped
        //   If the ball is dropped, there can be no setpiece owner
        // - The playstate OUT_OF_PLAY is only allowed when the setpiece is PENALTY
        //   (governingMatchState::OUT_OF_MATCH => setpieceType::PENALTY)
        validGameState = (  ((o != setpieceOwner::NONE) || (t == setpieceType::DROPPED_BALL))
                         && ((o == setpieceOwner::NONE) || (t != setpieceType::DROPPED_BALL))
                         && (t != setpieceType::NONE)
                         && ((p != governingMatchState::OUT_OF_MATCH) || (t == setpieceType::PENALTY))
                         );
    }
    else
    {
        validGameState = false;
    }

    return validGameState;
}


gameState::gameState()
{
    setGameState(governingGameState::INVALID, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::NONE);
}

gameState::gameState(const governingGameState& g)
{
    setGameState(g, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::NONE);
}

gameState::gameState(const governingGameState& g, const governingMatchState& p, const setpieceOwner& o, const setpieceType& t)
{
    setGameState(g, p, o, t);
}

gameState::~gameState() { }


void gameState::setGameState(const governingGameState& g, const governingMatchState& p, const setpieceOwner& o, const setpieceType& t)
{
    if (isValid(g, p, o, t))
    {
        _governingGameState = g;
        _governingMatchState = p;
        _setpieceOwner = o;
        _setpieceType = t;
    }
    else
    {
        std::ostringstream msg;
        msg << "Error: attempt to construct an illegal gamestate. " << toString(g, p, o, t);
        std::cerr << msg.str() << std::endl;
        throw std::runtime_error(msg.str());
    }
}

bool gameState::operator== (const gameState& other) const
{
    return (  (_governingGameState == other._governingGameState)
           && (_governingMatchState == other._governingMatchState)
           && (_setpieceOwner == other._setpieceOwner)
           && (_setpieceType == other._setpieceType)  );
}

bool gameState::operator!= (const gameState& other) const
{
    return !(*this == other);
}

bool gameState::isSetPiece() const
{
    return (  (_governingGameState == governingGameState::SETPIECE_PREPARING)
           || (_governingGameState == governingGameState::SETPIECE_EXECUTING)  );
}

bool gameState::isOwnSetPiece() const
{
    return (_setpieceOwner == setpieceOwner::OWN);
}

bool gameState::isPrepareSetPiece() const
{
    return (_governingGameState == governingGameState::SETPIECE_PREPARING);
}

bool gameState::isDroppedBallSetPiece() const
{
    return (_setpieceType == setpieceType::DROPPED_BALL);
}

bool gameState::isKickoffSetPiece() const
{
    return (_setpieceType == setpieceType::KICKOFF);
}

bool gameState::isPenaltySetPiece() const
{
    return (_setpieceType == setpieceType::PENALTY);
}

bool gameState::isSidelineSetPiece() const
{
    return (  (_setpieceType == setpieceType::CORNER)
           || (_setpieceType == setpieceType::THROWIN)  );
}

bool gameState::isGoalkickSetPiece() const
{
    return (_setpieceType == setpieceType::GOALKICK);
}

bool gameState::isFreekickSetPiece() const
{
    return (_setpieceType == setpieceType::FREEKICK);
}

bool gameState::isCornerSetPiece() const
{
    return (_setpieceType == setpieceType::CORNER);
}

bool gameState::isThrowinSetPiece() const
{
    return (_setpieceType == setpieceType::THROWIN);
}

bool gameState::isInMatch() const
{
    return (_governingMatchState == governingMatchState::IN_MATCH);
}

bool gameState::isStopped() const
{
    return (_governingGameState == governingGameState::NEUTRAL_STOPPED);
}


/* Function that translates a valid gamestate into a compact string */
std::string gameState::toString() const
{
    std::ostringstream oss;

    switch (_governingMatchState)
    {
    case governingMatchState::IN_MATCH:
        oss << "in match ";
        break;
    case governingMatchState::OUT_OF_MATCH:
        oss << "out of match ";
        break;
    }

    switch (_setpieceOwner)
    {
    case setpieceOwner::OWN:
        oss << "own ";
        break;
    case setpieceOwner::OPPONENT:
        oss << "opp ";
        break;
    case setpieceOwner::NONE:
    default:
        break;
    }

    switch (_setpieceType)
    {
    case setpieceType::FREEKICK:
        oss << "freekick ";
        break;
    case setpieceType::GOALKICK:
        oss << "goalkick ";
        break;
    case setpieceType::KICKOFF:
        oss << "kickoff ";
        break;
    case setpieceType::CORNER:
        oss << "corner ";
        break;
    case setpieceType::THROWIN:
        oss << "throwin ";
        break;
    case setpieceType::PENALTY:
        oss << "penalty ";
        break;
    case setpieceType::DROPPED_BALL:
        oss << "dropball ";
        break;
    case setpieceType::NONE:
    default:
        break;
    }

    switch (_governingGameState)
    {
    case governingGameState::INVALID:
        oss << "invalid";
        break;
    case governingGameState::NEUTRAL_STOPPED:
        oss << "stopped";
        break;
    case governingGameState::NEUTRAL_PLAYING:
        oss << "playing";
        break;
    case governingGameState::SETPIECE_PREPARING:
        oss << "prep";
        break;
    case governingGameState::SETPIECE_EXECUTING:
        oss << "exec";
        break;
    default:
        break;
    }

    return oss.str();
}

/* Function that translates any gamestate (valid or invalid) into a complete string */
std::string gameState::toString(const governingGameState& g, const governingMatchState& p, const setpieceOwner& o, const setpieceType& t) const
{
    std::ostringstream oss;

    oss << "[";
    switch (g)
    {
    case governingGameState::INVALID:
        oss << "invalid";
        break;
    case governingGameState::NEUTRAL_STOPPED:
        oss << "neutral stopped";
        break;
    case governingGameState::NEUTRAL_PLAYING:
        oss << "neutral playing";
        break;
    case governingGameState::SETPIECE_PREPARING:
        oss << "setpiece preparing";
        break;
    case governingGameState::SETPIECE_EXECUTING:
        oss << "setpiece executing";
        break;
    default:
        oss << "UNKNOWN";
        break;
    }

    oss << ", ";
    switch (p)
    {
    case governingMatchState::IN_MATCH:
        oss << "in match";
        break;
    case governingMatchState::OUT_OF_MATCH:
        oss << "out of match";
        break;
    default:
        oss << "UNKNOWN";
        break;
    }

    oss << ", ";
    switch (o)
    {
    case setpieceOwner::NONE:
        oss << "no owner";
        break;
    case setpieceOwner::OWN:
        oss << "own";
        break;
    case setpieceOwner::OPPONENT:
        oss << "opponent";
        break;
    default:
        oss << "UNKNOWN";
        break;
    }

    oss << ", ";
    switch (t)
    {
    case setpieceType::NONE:
        oss << "no setpiece type";
        break;
    case setpieceType::FREEKICK:
        oss << "freekick";
        break;
    case setpieceType::GOALKICK:
        oss << "goalkick";
        break;
    case setpieceType::KICKOFF:
        oss << "kickoff";
        break;
    case setpieceType::CORNER:
        oss << "corner";
        break;
    case setpieceType::THROWIN:
        oss << "throw-in";
        break;
    case setpieceType::PENALTY:
        oss << "penalty";
        break;
    case setpieceType::DROPPED_BALL:
        oss << "dropped ball";
        break;
    default:
        oss << "UNKNOWN";
        break;
    }

    oss << "]";
    return oss.str();
}


/* The complex function below has been created for historic reasons. It should be removed soon,
 * but first we need all clients to use non-treeEnum functions */
gameState::gameState(const treeEnum& gameState)
{
    switch (gameState)
    {
    case treeEnum::IN_MATCH_NEUTRAL_STOPPED_NEUTRAL:
        setGameState(governingGameState::NEUTRAL_STOPPED, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::NONE);
        break;
    case treeEnum::IN_MATCH_NEUTRAL_PLAYING_NEUTRAL:
        setGameState(governingGameState::NEUTRAL_PLAYING, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::NONE);
        break;
    case treeEnum::IN_MATCH_OWN_KICKOFF_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::KICKOFF);
        break;
    case treeEnum::IN_MATCH_OWN_KICKOFF_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::KICKOFF);
        break;
    case treeEnum::IN_MATCH_OPP_KICKOFF_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::KICKOFF);
        break;
    case treeEnum::IN_MATCH_OPP_KICKOFF_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::KICKOFF);
        break;
    case treeEnum::IN_MATCH_OWN_FREEKICK_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::FREEKICK);
        break;
    case treeEnum::IN_MATCH_OWN_FREEKICK_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::FREEKICK);
        break;
    case treeEnum::IN_MATCH_OPP_FREEKICK_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::FREEKICK);
        break;
    case treeEnum::IN_MATCH_OPP_FREEKICK_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::FREEKICK);
        break;
    case treeEnum::IN_MATCH_OWN_GOALKICK_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::GOALKICK);
        break;
    case treeEnum::IN_MATCH_OWN_GOALKICK_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::GOALKICK);
        break;
    case treeEnum::IN_MATCH_OPP_GOALKICK_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::GOALKICK);
        break;
    case treeEnum::IN_MATCH_OPP_GOALKICK_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::GOALKICK);
        break;
    case treeEnum::IN_MATCH_OWN_THROWIN_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::THROWIN);
        break;
    case treeEnum::IN_MATCH_OWN_THROWIN_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::THROWIN);
        break;
    case treeEnum::IN_MATCH_OPP_THROWIN_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::THROWIN);
        break;
    case treeEnum::IN_MATCH_OPP_THROWIN_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::THROWIN);
        break;
    case treeEnum::IN_MATCH_OWN_CORNER_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::CORNER);
        break;
    case treeEnum::IN_MATCH_OWN_CORNER_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::CORNER);
        break;
    case treeEnum::IN_MATCH_OPP_CORNER_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::CORNER);
        break;
    case treeEnum::IN_MATCH_OPP_CORNER_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::CORNER);
        break;
    case treeEnum::IN_MATCH_OWN_PENALTY_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::PENALTY);
        break;
    case treeEnum::IN_MATCH_OWN_PENALTY_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OWN, setpieceType::PENALTY);
        break;
    case treeEnum::IN_MATCH_OPP_PENALTY_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::PENALTY);
        break;
    case treeEnum::IN_MATCH_OPP_PENALTY_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::OPPONENT, setpieceType::PENALTY);
        break;
    case treeEnum::IN_MATCH_DROPPED_BALL_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::DROPPED_BALL);
        break;
    case treeEnum::IN_MATCH_DROPPED_BALL_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::DROPPED_BALL);
        break;
    case treeEnum::OUT_OF_MATCH_NEUTRAL_STOPPED_NEUTRAL:
        setGameState(governingGameState::NEUTRAL_STOPPED, governingMatchState::OUT_OF_MATCH, setpieceOwner::NONE, setpieceType::NONE);
        break;
    case treeEnum::OUT_OF_MATCH_OWN_PENALTY_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::OUT_OF_MATCH, setpieceOwner::OWN, setpieceType::PENALTY);
        break;
    case treeEnum::OUT_OF_MATCH_OPP_PENALTY_PREPARE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_PREPARING, governingMatchState::OUT_OF_MATCH, setpieceOwner::OPPONENT, setpieceType::PENALTY);
        break;
    case treeEnum::OUT_OF_MATCH_OWN_PENALTY_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::OUT_OF_MATCH, setpieceOwner::OWN, setpieceType::PENALTY);
        break;
    case treeEnum::OUT_OF_MATCH_OPP_PENALTY_EXECUTE_NEUTRAL:
        setGameState(governingGameState::SETPIECE_EXECUTING, governingMatchState::OUT_OF_MATCH, setpieceOwner::OPPONENT, setpieceType::PENALTY);
        break;
    case treeEnum::INVALID:
    default:
        setGameState(governingGameState::INVALID, governingMatchState::IN_MATCH, setpieceOwner::NONE, setpieceType::NONE);
        break;
    }
}

/* The complex function below has been created for historic reasons. It should be removed soon,
 * but first we need all clients to use non-treeEnum functions */
treeEnum gameState::toTreeEnum() const
{
    switch (_governingGameState)
    {
    case governingGameState::NEUTRAL_PLAYING:
        return treeEnum::IN_MATCH_NEUTRAL_PLAYING_NEUTRAL;
        break;
    case governingGameState::NEUTRAL_STOPPED:
        switch(_governingMatchState)
        {
        case governingMatchState::IN_MATCH:
            return treeEnum::IN_MATCH_NEUTRAL_STOPPED_NEUTRAL;
            break;
        case governingMatchState::OUT_OF_MATCH:
            return treeEnum::OUT_OF_MATCH_NEUTRAL_STOPPED_NEUTRAL;
            break;
        }
    case governingGameState::SETPIECE_PREPARING:
        switch (_setpieceType)
        {
        case setpieceType::DROPPED_BALL:
            return treeEnum::IN_MATCH_DROPPED_BALL_PREPARE_NEUTRAL;
            break;
        case setpieceType::FREEKICK:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_FREEKICK_PREPARE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_FREEKICK_PREPARE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::GOALKICK:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_GOALKICK_PREPARE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_GOALKICK_PREPARE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::KICKOFF:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_KICKOFF_PREPARE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_KICKOFF_PREPARE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::THROWIN:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_THROWIN_PREPARE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_THROWIN_PREPARE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::CORNER:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_CORNER_PREPARE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_CORNER_PREPARE_NEUTRAL;
                break;
            }
            break;
        default:
        case setpieceType::PENALTY:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                switch (_governingMatchState)
                {
                    case governingMatchState::IN_MATCH:
                        return treeEnum::IN_MATCH_OWN_PENALTY_PREPARE_NEUTRAL;
                        break;
                    case governingMatchState::OUT_OF_MATCH:
                        return treeEnum::OUT_OF_MATCH_OWN_PENALTY_PREPARE_NEUTRAL;
                        break;
                }
            default:
            case setpieceOwner::OPPONENT:
                switch (_governingMatchState)
                {
                    case governingMatchState::IN_MATCH:
                        return treeEnum::IN_MATCH_OPP_PENALTY_PREPARE_NEUTRAL;
                        break;
                    case governingMatchState::OUT_OF_MATCH:
                        return treeEnum::OUT_OF_MATCH_OPP_PENALTY_PREPARE_NEUTRAL;
                        break;
                }
            }
            break;
        }
        break;

    case governingGameState::SETPIECE_EXECUTING:
        switch (_setpieceType)
        {
        case setpieceType::DROPPED_BALL:
            return treeEnum::IN_MATCH_DROPPED_BALL_EXECUTE_NEUTRAL;
            break;
        case setpieceType::FREEKICK:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_FREEKICK_EXECUTE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_FREEKICK_EXECUTE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::GOALKICK:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_GOALKICK_EXECUTE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_GOALKICK_EXECUTE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::KICKOFF:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_KICKOFF_EXECUTE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_KICKOFF_EXECUTE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::THROWIN:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_THROWIN_EXECUTE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_THROWIN_EXECUTE_NEUTRAL;
                break;
            }
            break;
        case setpieceType::CORNER:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                return treeEnum::IN_MATCH_OWN_CORNER_EXECUTE_NEUTRAL;
                break;
            default:
            case setpieceOwner::OPPONENT:
                return treeEnum::IN_MATCH_OPP_CORNER_EXECUTE_NEUTRAL;
                break;
            }
            break;
        default:
        case setpieceType::PENALTY:
            switch (_setpieceOwner)
            {
            case setpieceOwner::OWN:
                switch (_governingMatchState)
                {
                    case governingMatchState::IN_MATCH:
                        return treeEnum::IN_MATCH_OWN_PENALTY_EXECUTE_NEUTRAL;
                        break;
                    case governingMatchState::OUT_OF_MATCH:
                        return treeEnum::OUT_OF_MATCH_OWN_PENALTY_EXECUTE_NEUTRAL;
                        break;
                }
            default:
            case setpieceOwner::OPPONENT:
                switch (_governingMatchState)
                {
                    case governingMatchState::IN_MATCH:
                        return treeEnum::IN_MATCH_OPP_PENALTY_EXECUTE_NEUTRAL;
                        break;
                    case governingMatchState::OUT_OF_MATCH:
                        return treeEnum::OUT_OF_MATCH_OPP_PENALTY_EXECUTE_NEUTRAL;
                        break;
                }
            }
            break;
        }
        break;

    case governingGameState::INVALID:
    default:
        return treeEnum::INVALID;
        break;
    }
}
