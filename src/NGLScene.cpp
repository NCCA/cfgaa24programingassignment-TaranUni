#include <QMouseEvent>
#include <QGuiApplication>

#include "ngl/Random.h"
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Util.h>
#include <ngl/ShaderLib.h>
#include <iostream>



NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Blank NGL");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f, float(_w)/_h,0.1f,200);
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.4f, 0.35f, 0.3f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  m_emmiter=std::make_unique<Emitter>(100000,50000);
  ngl::ShaderLib::loadShader("ParticleShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");
  ngl::ShaderLib::use("ParticleShader");
  m_view = ngl::lookAt({0,170,170},{0,50,0},{0,1,0});

//  ngl::ShaderLib::use(ngl::nglColourShader);
//  ngl::ShaderLib::setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
//  ngl::ShaderLib::setUniform("MVP",ngl::Mat4());
//  ngl::VAOPrimitives::createSphere("sphere",1.0f,10);



  startTimer(10);
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
    m_emmiter->update();
    update();
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
  mouseRotation.m_m[3][0] = m_modelPos.m_x;
  mouseRotation.m_m[3][0] = m_modelPos.m_x;

  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);
  glPointSize(10);
  m_emmiter->render();
//  ngl::VAOPrimitives::draw("sphere");
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
  case Qt::Key_W :
      m_view = ngl::lookAt({0,200,200},{0,50,0},{0,1,0});
//  case Qt::Key_A :
//      xVector = xVector - 10.0f;
//  case Qt::Key_S :
//      zVector = zVector - 10.0f;
//  case Qt::Key_D :
//      xVector = xVector + 10.0f;
  case Qt::Key_R :
      glClearColor(0,0,0,0);
  break;
  default : break;
  }
  // finally update the GLWindow and re-draw
//    m_view = ngl::lookAt({0,170,170},{0,50,0},{0,1,0});
    update();
}
