#include <QMouseEvent>
#include <QGuiApplication>

#include "ngl/Random.h"
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <iostream>



NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Campfire Scene");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(90.0f, float(_w)/_h,0.1f,2000);
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  m_emmiter=std::make_unique<Emitter>(10000,10000);
  ngl::ShaderLib::loadShader("ParticleShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");
  ngl::ShaderLib::use("ParticleShader");
  ngl::ShaderLib::loadShader("ObjectShader","shaders/ObjectVertex.glsl","shaders/ObjectFragment.glsl");
  ngl::ShaderLib::use("ObjectShader");

  m_CameraPosX = 0;
  m_CameraPosX = 170;
  m_CameraPosX = 170;
  m_CameraPosition = ngl::Vec3(0,170,170);
  m_Centre = ngl::Vec3(0,50,0);
  m_view = ngl::lookAt({0,170,170},{0,50,0},{0,1,0});

//  ngl::ShaderLib::use(ngl::nglColourShader);
//  ngl::ShaderLib::setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
//  ngl::ShaderLib::setUniform("MVP",ngl::Mat4());
//  ngl::VAOPrimitives::createSphere("sphere",10.0f,10);
    ngl::VAOPrimitives::createDisk("disk", 0.8f, 120);
//  ngl::VAOPrimitives::draw("sphere");

  m_win.FoV = 90.0f;

  startTimer(10);
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
    m_emmiter->update();
    update();
}

void NGLScene::loadMatricesToShader()
{
    ngl::Mat4 MVP = m_project * m_view * m_mouseGlobalTX * m_transform.getMatrix();

    ngl::ShaderLib::setUniform("MVP", MVP);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  auto  rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto  rotY = ngl::Mat4::rotateY(m_win.spinYFace);
  auto mouseRotation = rotX * rotY;
  mouseRotation.m_m[3][0] = m_modelPos.m_x;
  mouseRotation.m_m[3][1] = m_modelPos.m_y;
  mouseRotation.m_m[3][2] = m_modelPos.m_z;

  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);

  ngl::ShaderLib::use("ParticleShader");
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);
  loadMatricesToShader();
  ngl::VAOPrimitives::draw("mySphere");
  m_emmiter->render();
}

//----------------------------------------------------------------------------------------------------------------------


void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    // This method is called every time the main window receives a key event.
    // We then switch on the key value and set the camera in the GLWindow
    switch (_event->key())
    {
        // Escape key to quit
        case Qt::Key_Escape :
            QGuiApplication::exit(EXIT_SUCCESS);
            break;
        case Qt::Key_Space :
            m_win.spinXFace = 0;
            m_win.spinYFace = 0;
            m_modelPos.set(ngl::Vec3::zero());
            break;
        default :
            break;
    }
    // Finally, update the GLWindow and re-draw
    update();
}


//void NGLScene::keyPressEvent(QKeyEvent *_event)
//{
//    // This method is called every time the main window receives a key event.
//    // We then switch on the key value and set the camera in the GLWindow
//    switch (_event->key())
//    {
//        // Escape key to quit
//        case Qt::Key_Escape :
//            QGuiApplication::exit(EXIT_SUCCESS);
//            break;
//        case Qt::Key_Space :
//            m_win.spinXFace = 0;
//            m_win.spinYFace = 0;
//            m_modelPos.set(ngl::Vec3::zero());
//            break;
//        case Qt::Key_W :
//            // Move forward
//            m_modelPos += ngl::Vec3(0, 0, 10);
//            break;
//        case Qt::Key_A :
//            // Move left
//            m_modelPos += ngl::Vec3(-10, 0, 0);
//            break;
//        case Qt::Key_S :
//            // Move backward
//            m_modelPos += ngl::Vec3(0, 0, -10);
//            break;
//        case Qt::Key_D :
//            // Move right
//            m_modelPos += ngl::Vec3(10, 0, 0);
//            break;
//        case Qt::Key_R :
//            glClearColor(0, 0, 0, 0);
//            break;
//        case Qt::Key_F :
//            m_view = ngl::lookAt({0,170,170},{0,50,0},{0,1,0});
//            break;
//        case Qt::Key_Up :
//            // Move forward
//            m_CameraPosition += ngl::Vec3(0, 0, 10);
//            m_Centre -= ngl::Vec3(0, 0, 10);
//            // Update the view matrix based on the new camera position and center point
//            m_view = ngl::lookAt(m_CameraPosition, {0,50,0}, {0, 1,0 });
//            break;
//        case Qt::Key_Left :
//            // Move left
//            m_CameraPosition += ngl::Vec3(-10, 0, 0);
//            m_Centre -= ngl::Vec3(-10, 0, 0);
//            // Update the view matrix based on the new camera position and center point
//            m_view = ngl::lookAt(m_CameraPosition, {0,50,0}, {0, 1, 0});
//            break;
//        case Qt::Key_Down :
//            // Move backward
//            if (m_CameraPosition.m_z < 0)
//            {
//                m_CameraPosition += ngl::Vec3(0, 0, 10);
//                m_Centre -= ngl::Vec3(0, 0, -10);
//            }
//            else if (m_CameraPosition.m_z > 0)
//            {
//                m_CameraPosition += ngl::Vec3(0, 0, -10);
//                m_Centre -= ngl::Vec3(0, 0, -10);
//            }
//            // Move backward
//            // Update the view matrix based on the new camera position and center point
//            m_view = ngl::lookAt(m_CameraPosition, {0,50,0}, {0, 1, 0});
//            break;
//        case Qt::Key_Right :
//            // Move right
//            m_CameraPosition += ngl::Vec3(10, 0, 0);
//            m_Centre -= ngl::Vec3(10, 0, 0);
//            // Update the view matrix based on the new camera position and center point
//            m_view = ngl::lookAt(m_CameraPosition, {0,50,0}, {0, 1, 0});
//            break;
//        default :
//            break;
//    }
//    // Finally, update the GLWindow and re-draw
//    update();
//}