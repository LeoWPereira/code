 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 #ifndef FIELDWIDGET3D_H
#define FIELDWIDGET3D_H

#include <map>
#include <QVTKWidget.h>
#include <QVTKInteractor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPlaneSource.h>
#include <vtkCubeSource.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>

// Internal:
#include "int/widgets/Field/CamFeedParams.h"
#include "int/widgets/Field/Visualization/BallVisualization.h"
#include "int/widgets/Field/Visualization/ObstacleVisualization.h"
#include "int/widgets/Field/Visualization/ForbiddenAreaVisualization.h"
#include "int/widgets/Field/Visualization/RobotVisualization.h"
#include "int/widgets/Field/Visualization/ShootTargetVisualization.h"
#include "int/widgets/Field/Visualization/projectSpeedVisualization.h"
#include "int/widgets/Field/SettingsDialog.h"
#include "int/widgets/Widget.h"

// External:
#include "polygon2D.hpp"
#include "linepoint2D.hpp"

#define OBSTACLE_HEIGHT 0.2

class GameSignalAdapter;
class FieldWidget3D;

/*
* Class that handles subscriber data for the FieldWidget
* Use delegation to avoid diamond of death on QObject
*/
class FieldWidgetGameSignalSubscriber : public QObject, public WidgetGameSignalSubscriber<FieldWidget3D>
{
    Q_OBJECT
public:
    using WidgetGameSignalSubscriber::WidgetGameSignalSubscriber;
    virtual void subscribe(GameSignalAdapter* signalAdapter) override;
    virtual void setSignalMode(SignalMode mode) override;

protected:
    virtual void onTeamModeChanged() override;
    virtual void onRobotModeChanged() override;
    virtual bool displayDataFilter(const uint8_t& robotID, const SignalMode& signalMode, const DataType& dataType);

public Q_SLOTS:

    virtual void onElapsedTimeChanged(double t); // Time elapsed in log session

    virtual void onBallPositionChanged(ObjectId id, SignalMode signalMode, PositionVelocity& posvel, float confidence) override; // Ball position according to one robot
    virtual void onBallPossessionChanged(uint8_t senderRobotId, SignalMode signalMode, BallPossessionType type, uint8_t robotId) override; // Ball possession according to one robot
    virtual void onOwnTeamPositionChanged(uint8_t senderRobotId, SignalMode signalMode, uint8_t robotId, PositionVelocity& posvel) override; // Team member position according to one robot
    virtual void onObstaclePositionChanged(ObjectId id, SignalMode signalMode, PositionVelocity& posvel) override; // Obstacle position according to one robot
    virtual void onForbiddenAreaChanged(ObjectId id, SignalMode signalMode, polygon2D& area) override; // Obstacle position according to one robot
    virtual void onShootTargetChanged(uint8_t senderRobotId, SignalMode mode, PositionVelocity& posvel, bool aiming) override; // Shoot target according to one robot
    virtual void onProjectSpeedChanged(ObjectId id, SignalMode signalMode, linepoint2D& line) override; // projected speed vector according to one robot

    virtual void onPathPlanningInProgress(uint8_t senderRobotId, std::vector<PositionVelocity>& path) override;
};

class FieldEventHandler;

/*
* Field rendering class
*/
class FieldWidget3D : public QVTKWidget, public Widget<FieldWidgetGameSignalSubscriber, FieldWidget3D, Visualizer::Field::Settings::SettingsDialog>
{
        Q_OBJECT
friend FieldWidgetGameSignalSubscriber;

public:
    explicit FieldWidget3D(QWidget *parent = 0);
    ~FieldWidget3D();

    template <class TFieldEventHandler>
    void addFieldEventHandler()
    {
        TFieldEventHandler* handler = new TFieldEventHandler(this);
        _eventHandlers.push_back(handler);
    }

    virtual void saveState() override;
    virtual void restoreState() override;

public Q_SLOTS:
    void resetZoomPanRotate();
    void flip(bool flip); // Rotate field 180 degrees (resets camera position as well)
    virtual void showSettingsDialog() override
    {
        Widget::showSettingsDialog();
    }

protected:
    virtual void reloadSettings() override;

private Q_SLOTS:  /* == Rendering == */
    void render();

public:
    vtkRenderer* getRenderer() { return _mainRenderer; };
    vtkSmartPointer<RobotVisualization> getTeamRobot(uint8_t robotId); // key: robotId [1..n]
    void setLogTimeStamp(double t) { _timestamp = t; };

private: 
    QTimer *_updateTimer; // Timeout signal used to update
    QMutex _renderMutex;

    std::vector<FieldEventHandler* > _eventHandlers;

    vtkRenderWindow *_renderWindow = nullptr;
    vtkRenderer *_mainRenderer = nullptr;
    vtkRenderer *_annotationRenderer = nullptr; // Renders annotation on top of the scene

    vtkCamera* _camera = nullptr;
    vtkActor* _field = nullptr;
    
    double _timestamp = 0; // Game logging timestamp (0.0 is start), to decide when to hide actors

    bool _flipField = false; // Flip field 180 to reflect that the playing direction changed. Note: This just changes the camera orientation, field rendering doesn't change!

    void clear(); // Make all actors invisible
    void cleanup();

    /* == Cam feed rendering (prototyping #435) == */
    bool cfEnabled = false;
    // TODO use cam feed instead of static jpg (which is just a proof of concept)
    // TODO configuration sliders for all of the parameters
    CamFeedParams cfParams;
    vtkSmartPointer<vtkImageActor> cfImageActor;
    vtkSmartPointer<vtkImageData> cfImageData;
    void cfRender();

    /* == Field rendering == */
    void addFieldActors();
    void addGoalActors();

    /* == Team rendering == */
    std::map<uint8_t, vtkSmartPointer<RobotVisualization>> _teamActors; // key: robotId, index is not zero-based but one-based
    void addRobotActor(uint8_t robotId);

    /* == Obstacle rendering == */
    std::vector<vtkSmartPointer<ObstacleVisualization>> _obstacleActors;
    std::map<int, std::pair<int, double>> _obstacleMapping; // key: obstacle id, value: (actor index, last update)
    void addObstacleActor();
    void setObstaclePosition(PositionVelocity& posvel, ObjectId id);

    /* == Forbidden area rendering == */
    std::vector<vtkSmartPointer<ForbiddenAreaVisualization>> _forbiddenAreaActors;
    std::map<int, std::pair<int, double>> _forbiddenAreaMapping; // key: obstacle id, value: (actor index, last update)
    void addForbiddenAreaActor();
    void setForbiddenAreaPosition(polygon2D& area, ObjectId id);

    /* == Ball rendering == */
    std::vector<vtkSmartPointer<BallVisualization>> _ballActors; 
    std::map<int, std::pair<int, double>> _ballMapping; // key: ball id, value: (actor index, last update)
    void addBallActor();
    void setBallPosition(PositionVelocity& posvel, ObjectId id);

    /* == Shoot target rendering == */
    std::map<uint8_t, vtkSmartPointer<ShootTargetVisualization>> _shootTargetActors;
    void addShootTargetActor(uint8_t robotId);
    void setShootTargetPosition(uint8_t robotId, PositionVelocity& posvel);
    void hideShootTarget(uint8_t robotId);

    /* == project speed vector rendering == */
    std::vector<vtkSmartPointer<projectSpeedVisualization>> _projectSpeedActors;
    std::map<int, std::pair<int, double>> _projectSpeedMapping; // key: obstacle id, value: (actor index, last update)
    void addProjectSpeedActor();
    void setProjectSpeedPosition(linepoint2D& speedVector, ObjectId id);

    /* == Annotation rendering == */
    void addCollision(uint8_t robotId); // Adds a collision marker at the point of the specified robot.

    /* == Generic rendering methods == */
    vtkSmartPointer<vtkActor> createLine(float x1, float y1, float z1, float x2, float y2, float z2);
    vtkSmartPointer<vtkActor> createColoredDashedLine(float x1, float y1, float z1, float x2, float y2, float z2, double r, double g, double b);
    vtkSmartPointer<vtkActor> createColoredDot(float x, float y, float radius, double r, double g, double b);
    vtkActor* createDashedLine(float x1, float y1, float z1, float x2, float y2, float z2);

    void renderDotAddActor(vtkRenderer* renderer, float x, float y, bool black, float radius=0.05);
    void renderArcAddActor(vtkRenderer* renderer, float x, float y, float radius, float startDeg, float endDeg);
};

/*
* Field event handlers handle mouse clicks, hovers and key presses.
* Depending on how you want the user interaction to be, add an event handler to the field.
*/
class FieldEventHandler : public QObject
{
public:
    FieldEventHandler(FieldWidget3D* field)
    {
        _field = field;
        _field->installEventFilter(this);
    }

    virtual bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (dynamic_cast<FieldWidget3D*>(watched))
        {
            handle(event);
        }

        return false;
    }

protected:
    FieldWidget3D* _field;
    virtual void handle(QEvent* event) = 0;
};

#endif // FIELDWIDGET3D_H
