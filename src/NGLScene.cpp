#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <QPainter>
#include <iostream>

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Campfire Scene or fluid sim IDK right now");
}

//----------------------------------------------------------------------------------------------------------------------

NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(90.0f, float(_w)/_h,0.1f,2000);
}

//----------------------------------------------------------------------------------------------------------------------

//constexpr auto shaderProgram = "ObjectShader";
constexpr auto shaderProgram = "PBR";

//----------------------------------------------------------------------------------------------------------------------

//void NGLScene::initializeGL()
//{
//  // we must call that first before any other GL commands to load and link the
//  // gl commands from the lib, if that is not done program will crash
//  ngl::NGLInit::initialize();
//  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
//  // enable depth testing for drawing
//  glEnable(GL_DEPTH_TEST);
//  // enable multisampling for smoother drawing
//  glEnable(GL_MULTISAMPLE);
//  m_emmiter=std::make_unique<Emitter>(10000,10000);
//
//    ngl::ShaderLib::loadShader("shaderProgram","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");
////  ngl::ShaderLib::loadShader("ParticleShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");
////  ngl::ShaderLib::loadShader("ObjectShader","shaders/ObjectVertex.glsl","shaders/ObjectFragment.glsl");
////  ngl::ShaderLib::use("ObjectShader");
//
////  m_CameraPosition = ngl::Vec3(0,170,170);
////  m_Centre = ngl::Vec3(0,50,0);
//
//    // We now create our view matrix for a static camera
//    ngl::Vec3 from(0.0f, 2.0f, 10.0f);
//    ngl::Vec3 to(0.0f, 0.0f, 0.0f);
//    ngl::Vec3 up(0.0f, 1.0f, 0.0f);
//    // now load to our new camera
//    m_view = ngl::lookAt(from, to, up);
////  m_view = ngl::lookAt({0,170,170},{0,50,0},{0,1,0});
//
////  ngl::ShaderLib::use(ngl::nglColourShader);
////  ngl::ShaderLib::setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
////  ngl::ShaderLib::setUniform("MVP",ngl::Mat4());
////    ngl::ShaderLib::setUniform("MVP",m_project*m_view*m_mouseGlobalTX);
//    ngl::VAOPrimitives::createDisk("disk", 300.0f, 120);
////    ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
//
//  m_win.FoV = 90.0f;
//
//  startTimer(10);
//}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::initializeGL()
{
    // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
    // be done once we have a valid GL context but before we call any GL commands. If we dont do
    // this everything will crash
    ngl::NGLInit::initialize();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Grey Background
    // enable depth testing for drawing
    glEnable(GL_DEPTH_TEST);
    m_lightAngle = 0.0;
    m_lightPos.set(sinf(m_lightAngle), 2, cosf(m_lightAngle));
    m_emmiter=std::make_unique<Emitter>(10000,10000);


    // now to load the shader and set the values
    // we are creating a shader called PBR to save typos
    // in the code create some constexpr
    constexpr auto vertexShader = "PBRVertex";
    constexpr auto fragShader = "PBRFragment";
    // create the shader program
    ngl::ShaderLib::createShaderProgram(shaderProgram);
    // now we are going to create empty shaders for Frag and Vert
    ngl::ShaderLib::attachShader(vertexShader, ngl::ShaderType::VERTEX);
    ngl::ShaderLib::attachShader(fragShader, ngl::ShaderType::FRAGMENT);
    // attach the source
    ngl::ShaderLib::loadShaderSource(vertexShader, "shaders/PBRVertex.glsl");
    ngl::ShaderLib::loadShaderSource(fragShader, "shaders/PBRFragment.glsl");
    // compile the shaders
    ngl::ShaderLib::compileShader(vertexShader);
    ngl::ShaderLib::compileShader(fragShader);
    // add them to the program
    ngl::ShaderLib::attachShaderToProgram(shaderProgram, vertexShader);
    ngl::ShaderLib::attachShaderToProgram(shaderProgram, fragShader);
    // now we have associated that data we can link the shader
    ngl::ShaderLib::linkProgramObject(shaderProgram);
    // and make it active ready to load values
    ngl::ShaderLib::use(shaderProgram);

//    ngl::ShaderLib::loadShader("ParticleShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");

    // We now create our view matrix for a static camera
    ngl::Vec3 from(0.0f, 2.0f, 10.0f);
    ngl::Vec3 to(0.0f, 0.0f, 0.0f);
    ngl::Vec3 up(0.0f, 1.0f, 0.0f);
    // now load to our new camera
    m_view = ngl::lookAt(from, to, up);
    m_project = ngl::perspective(45.0f, 1024.0f/720.0f, 0.05f, 350.0f);

    ngl::ShaderLib::setUniform("camPos", from);
    // now a light
    m_lightPos.set(0.0, 400.0f, 0.0f, 1.0f);
    // setup the default shader material and light porerties
    // these are "uniform" so will retain their values
    ngl::ShaderLib::setUniform("lightPosition", m_lightPos.toVec3());
    ngl::ShaderLib::setUniform("lightColor", 40000.0f, 40000.0f, 40000.0f);
    ngl::ShaderLib::setUniform("exposure", 3.0f);
    ngl::ShaderLib::setUniform("albedo", 0.950f, 0.71f, 0.29f);

    ngl::ShaderLib::setUniform("metallic", 1.0f);
    ngl::ShaderLib::setUniform("roughness", 0.38f);
    ngl::ShaderLib::setUniform("ao", 0.2f);

    ngl::VAOPrimitives::createDisk("disk", 200.0f, 120);
    // this timer is going to trigger an event every 40ms which will be processed in the
    //
    m_lightTimer = startTimer(40);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::timerEvent(QTimerEvent *_event)
{
    m_emmiter->update();
    update();
}

//----------------------------------------------------------------------------------------------------------------------

//void NGLScene::loadMatricesToShader()
//{
////    ngl::Mat4 MVP = m_project * m_view * m_mouseGlobalTX;
////
////    ngl::ShaderLib::setUniform("MVP", MVP);
//
//    ngl::ShaderLib::use("ObjectShader");
//    struct transform
//    {
//        ngl::Mat4 MVP;
//        ngl::Mat4 normalMatrix;
//        ngl::Mat4 M;
//    };
//
//    transform t;
//    t.M = m_view * m_mouseGlobalTX * m_transform.getMatrix();
//
//    t.MVP = m_project * t.M;
//    t.normalMatrix = t.M;
//    t.normalMatrix.inverse().transpose();
//    ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
//
//    ngl::ShaderLib::setUniform("MVP", t.MVP);
////    ngl::ShaderLib::setUniform("MVP",m_project*m_view*m_mouseGlobalTX);
//}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::loadMatricesToShader()
{
    ngl::ShaderLib::use("PBR");
    struct transform
    {
        ngl::Mat4 MVP;
        ngl::Mat4 normalMatrix;
        ngl::Mat4 M;
    };

    transform t;
    t.M = m_view * m_mouseGlobalTX * m_transform.getMatrix();


    t.MVP = m_project * t.M;
    t.normalMatrix = t.M;
    t.normalMatrix.inverse().transpose();
    if (m_showParticles)
    {
        t.M = m_view * m_mouseGlobalTX * m_transform.getMatrix();
    }
    ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);

    ngl::ShaderLib::setUniform("lightPosition", (m_mouseGlobalTX * m_lightPos).toVec3());
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::drawScene(const std::string &_shader)
{
    ngl::ShaderLib::use(_shader);
    // Rotation based on the mouse position for our global transform
    ngl::Mat4 rotX = ngl::Mat4::rotateX(m_win.spinXFace);
    ngl::Mat4 rotY = ngl::Mat4::rotateY(m_win.spinYFace);
    // multiply the rotations
    m_mouseGlobalTX = rotY * rotX;
    // add the translations
    m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
    m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
    m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

    m_transform.reset();
    {
        ngl::ShaderLib::setUniform("lightColor", 40000.0f, 40000.0f, 40000.0f);
        ngl::ShaderLib::use("PBR");
        m_transform.setPosition(0.0f, -40.0f, 0.0f);
        m_transform.setRotation(90.0f, 0.0f, 0.0f);
        loadMatricesToShader();
        ngl::VAOPrimitives::draw("disk");
    } // and before a pop

    m_transform.reset();
    {
        ngl::ShaderLib::setUniform("lightColor", 40000.0f, 0.0f, 0.0f);
        loadMatricesToShader();
        m_emmiter->render();
    } // and before a pop

    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Times New Roman", 15));
    painter.drawText(QRect(50, -50, width(), height()), Qt::AlignLeft + Qt::AlignBottom, "Scroll: Zoom \n "
                                                                                         "Left-Click hold: Tumble Scene \n "
                                                                                         "Right-Click hold: Shift Scene \n"
                                                                                         "Space: Reset camera");

}

//void NGLScene::paintGL()
//{
//    // clear the screen and depth buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glViewport(0, 0, m_win.width, m_win.height);
//    drawScene("PBR");
//}

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::paintGL()
{
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_win.width, m_win.height);

    // Draw scene with PBR shader
    drawScene("PBR");

//    if (m_showDisk)
//    {
//        // Draw scene with PBR shader
//        drawScene("PBR");
//    }
//    if (m_showParticles)
//    {
//        //Draw scene with particleShader
//        drawScene("ParticleShader");
//    }
}

//
//void NGLScene::paintGL()
//{
//    // Clear the screen and depth buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glViewport(0, 0, m_win.width, m_win.height);
//
//    // Set up the view matrix
//    auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
//    auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);
//    auto m_mouseGlobalTX = rotX * rotY;
//    // add the translations
//    m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
//    m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
//    m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
//
//
//    // Set the MVP matrix for the particles
////    ngl::ShaderLib::use("ParticleShader");
////    ngl::Mat4 particleMVP = m_project * m_view * m_mouseGlobalTX;
////    loadMatricesToShader();
////    ngl::ShaderLib::setUniform("MVP", particleMVP);
////    m_emmiter->render();
//
//    m_transform.reset();
//    {
//        ngl::ShaderLib::use("ParticleShader");
//        loadMatricesToShader();
//        m_emmiter->render();
//    } // and before a pop
//
//    m_transform.reset();
//    {
//        ngl::ShaderLib::use("ObjectShader");
//        m_transform.setPosition(0.0f, -10.0f, 1.0f);
//        m_transform.setRotation(90.0f, 0.0f, 0.0f);
//        loadMatricesToShader();
//        ngl::VAOPrimitives::draw("disk");
//    } // and before a pop
//    // Render the disk
////    ngl::ShaderLib::use("ObjectShader");
//
//    // Reset the transform to avoid cumulative transformations
////    m_transform.reset();
//
//    // Set the transformation for the disk
////    m_transform.setPosition(0.0f, -10.0f, 1.0f);
////    m_transform.setRotation(90.0f, 0.0f, 0.0f);
//
//    // Set the color uniform
////    ngl::ShaderLib::setUniform("Colour", 0.4f, 0.3f, 0.2f, 1.0f);
//
//    // Load the transformation matrices to the shader
////    loadMatricesToShader();
//
//    // Draw the disk
////    ngl::VAOPrimitives::draw("disk");
//
//    // Update the display
//    update();
//}

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
