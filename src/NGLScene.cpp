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
  setTitle("Fluid Sim");
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
    constexpr auto shaderProgram = "PBR";

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
    ngl::ShaderLib::setUniform("lightColor", 10000.0f, 10000.0f, 10000.0f);
    ngl::ShaderLib::setUniform("exposure", 3.0f);
    ngl::ShaderLib::setUniform("albedo", 0.950f, 0.71f, 0.29f);
    ngl::ShaderLib::setUniform("metallic", 1.0f);
    ngl::ShaderLib::setUniform("roughness", 0.38f);
    ngl::ShaderLib::setUniform("ao", 0.2f);

    ngl::VAOPrimitives::createDisk("disk", 200.0f, 120);
    // this timer is going to trigger an event every 40ms which will be processed in the

    ngl::ShaderLib::loadShader("ParticleShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");

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

void NGLScene::loadMatricesToShader(const std::string &_shader)
{
    struct transform
    {
        ngl::Mat4 MVP;
        ngl::Mat4 normalMatrix;
        ngl::Mat4 M;
    };

    transform t;
    t.M = m_view * m_mouseGlobalTX * m_transform.getMatrix();
    t.MVP = m_project * t.M;

//    ngl::ShaderLib::use("PBR");
//    t.normalMatrix = t.M;
//    t.normalMatrix.inverse().transpose();
//
//    ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
//
//    ngl::ShaderLib::setUniform("lightPosition", (m_mouseGlobalTX * m_lightPos).toVec3());

    ngl::ShaderLib::use(_shader);
    if (_shader == "PBR")
    {
        t.normalMatrix = t.M;
        t.normalMatrix.inverse().transpose();
        ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
        ngl::ShaderLib::setUniform("lightPosition", (m_mouseGlobalTX * m_lightPos).toVec3());
    }
    else if (_shader == "ParticleShader")
    {
        ngl::ShaderLib::setUniform("MVP", t.MVP);
    }
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
    if (_shader == "PBR")
    {
        ngl::ShaderLib::setUniform("lightColor", 40000.0f, 40000.0f, 40000.0f);
        m_transform.setPosition(0.0f, -20.0f, 0.0f);
        m_transform.setRotation(90.0f, 0.0f, 0.0f);
        loadMatricesToShader("PBR");
        ngl::VAOPrimitives::draw("disk");
    }
    else if (_shader == "ParticleShader")
    {
        loadMatricesToShader("ParticleShader");
        m_emmiter->render();
    }

    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Times New Roman", 15));
    painter.drawText(QRect(50, -50, width(), height()), Qt::AlignLeft + Qt::AlignBottom, "Scroll: Zoom \n "
                                                                                         "Left-Click hold: Tumble Scene \n "
                                                                                         "Right-Click hold: Shift Scene \n"
                                                                                         "Space: Reset camera \n"
                                                                                         "ESC: Exit program");
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::paintGL()
{
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_win.width, m_win.height);

    // Draw scene with PBR shader
    drawScene("PBR");

    // Draw scene with particleShader
    drawScene("ParticleShader");
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
