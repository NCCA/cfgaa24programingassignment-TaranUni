#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Vec3.h>
#include "WindowParams.h"
// this must be included after NGL includes else we get a clash with gl libs
#include <QOpenGLWindow>
#include "Emitter.h"
#include <memory>
#include <ngl/Mat4.h>
#include <ngl/Transformation.h>
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL() override;
    void updateViewMatrix();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we resize the window
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h) override;

private:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event ) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event ) override;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a simple light use to illuminate the screen
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec4 m_lightPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a method to update the light
    //----------------------------------------------------------------------------------------------------------------------
    void updateLight();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief timer id for the animation timer
    //----------------------------------------------------------------------------------------------------------------------
    int m_lightTimer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if were animating the light
    //----------------------------------------------------------------------------------------------------------------------
    bool m_animate;
    //----------------------------------------------------------------------------------------------------------------------
    /// the angle of the light updated when the timer triggers to make it rotate
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Real m_lightAngle;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief called when the timer is triggered
    //----------------------------------------------------------------------------------------------------------------------
    void timerEvent(QTimerEvent *_event) override;
    /// @brief windows parameters for mouse control etc.
    WinParams m_win;
    /// position for our model
    ngl::Vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    std::unique_ptr<Emitter> m_emmiter;
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_view;
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_project;
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_CameraPosition;
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_Centre;
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Transformation m_transform;
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draw our scene passing in the shader to use
    /// @param[in] _shader the name of the shader to use when drawing
    //----------------------------------------------------------------------------------------------------------------------
    void drawScene(const std::string &_shader);

    bool m_showPBR = false;

    bool m_showParticles = false;

};



#endif
