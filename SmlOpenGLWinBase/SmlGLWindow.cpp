#include <QtGlobal>

#include "SmlGLWindow.h"
#include <QMutexLocker>


SmlThreadGLRender::SmlThreadGLRender(QObject* parent, SmlGLWindow* window) :
    QObject{ parent },
    _glwin{ window }
{
}

void SmlThreadGLRender::Render()
{
    _glwin->ThreadRender();
}


void SmlGLWindow::RequestRender()
{
    if (!_multiThreadMode)
    {
        Render();
        emit RenderFrameDoneSignal(); //SmlThreadGLWindow::requestUpdate
		if constexpr (false)
		{
			connect(this, &SmlGLWindow::RenderFrameDoneSignal,
				this, &SmlGLWindow::requestUpdate);
		}

        return;
    }


    if (_requestMode)
    {
        emit RequestRenderSignal(); //SmlThreadGLRender::Render
		if constexpr (false)
		{
			connect(this, &SmlGLWindow::RequestRenderSignal,
				_render, &SmlThreadGLRender::Render);
		}
        return;
    }

    const ulong timeOut = 500;
    bool waitOK = _ctxSemphore.Wait(timeOut);
    if (!waitOK)
    {
        return;
    }

    _glctx->moveToThread(_thread); //ctx main thread --> render thread


    emit RequestRenderSignal(); //SmlThreadGLRender::Render
	if constexpr (false)
	{
		connect(this, &SmlGLWindow::RequestRenderSignal,
			_render, &SmlThreadGLRender::Render);
	}
}

static void xxxDelay()
{
    static int xxxdebug = 0;
    if (xxxdebug)
    {
        static int secs = 10;
        for (int ii = 0; ii < secs; ++ii)
        {
            QThread::msleep(1000);
        }
    }
}

void SmlGLWindow::ThreadRender()
{
    bool ctxResponsedOk = true;
    if (_requestMode)
    {
        RequestCtx();
        ctxResponsedOk = _ctxResponsedOk;
    }

    if(ctxResponsedOk)
    {
        xxxDelay();

        Render();
        _glctx->moveToThread(this->thread()); //ctx render thread --> main thread

        _ctxSemphore.Notify(false); //ok to use opengl context in main thread

        emit RenderFrameDoneSignal(); //SmlThreadGLWindow::requestUpdate
        if constexpr (false)
        {
			connect(this, &SmlGLWindow::RenderFrameDoneSignal,
				this, &SmlGLWindow::requestUpdate);
        }
    }
}

void SmlGLWindow::Render()
{
    if (isExposed())
    {

        MakeCurrentCtx(__FUNCTION__, __FILE__);

        GLPaint(_paintDev);
        _glctx->swapBuffers(this);

        DoneCurrentCtx();

    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void SmlGLWindow::paintEvent(QPaintEvent* ev)
{
    RequestRender();
}
#else
void SmlThreadGLWindow::exposeEvent(QExposeEvent* ev)
{
    RequestRender();
}
#endif


static void APIENTRY GLDebugPoc(
        GLenum source, GLenum type,
        GLuint id, GLenum severity,
        GLsizei length, const GLchar* message,
        const void* userParam)
{
    QString str = QString::fromUtf16(
                u"severity:%3 message:[%5] source:%0 type:%1 id:%2 length:%4")
            .arg(source)
            .arg(type)
            .arg(id)
            .arg(severity)
            .arg(length)
            .arg(message);

    qDebug() << str;
}

void SmlGLWindow::resizeEvent(QResizeEvent* ev)
{
    if (_SurfaceDestroyed)
    {
        return;
    }

    SML_QTBase::resizeEvent(ev);

    bool initGL = false;

    const ulong SML_INFINITE = -1UL;
    bool waitOk = _ctxSemphore.Wait(SML_INFINITE);
    if(waitOk)
    {
        if (nullptr == _glctx)
        {
            initGL = true;
            _glctx = new QOpenGLContext{ nullptr }; //cannot have parent for moving thread
            _glctx->setFormat(requestedFormat());
            _glctx->setShareContext(nullptr);
            _glctx->create();
        }

        MakeCurrentCtx(__FUNCTION__, __FILE__);

        if (initGL)
        {
            _paintDev = new QOpenGLPaintDevice{};

            initializeOpenGLFunctions();
            glDebugMessageCallback(GLDebugPoc, nullptr);


            GLInitialize();
        }

        qreal dpr = devicePixelRatio();
        _paintDev->setDevicePixelRatio(dpr);
        _paintDev->setSize(ev->size() * dpr);

        GLResize(ev->size(), ev->oldSize());

        DoneCurrentCtx();

        _ctxSemphore.Notify(false); //ok to move opengl context
    }
}


bool SmlGLWindow::event(QEvent* ev)
{
    QEvent::Type et = ev->type();

#if 0
    ++_eventCounter;
    qDebug() << _eventCounter << ev;

    switch (et)
    {
    case QEvent::Paint:
    case QEvent::Expose:
        qDebug() << _eventCounter << ev;
        break;
    }
#endif

    switch (et)
    {
    case QEvent::UpdateRequest:
        RequestRender();
        break;

    case QEvent::PlatformSurface:
    {
        auto* psev = (QPlatformSurfaceEvent*)(ev);
        if (QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed == psev->surfaceEventType())
        {
            _SurfaceDestroyed = true;
            FinalizeGL();
        }
    }
        break;

    }

    return SML_QTBase::event(ev);
}

void SmlGLWindow::FinalizeGL()
{
    const ulong SML_INFINITE = -1UL;
    bool waitOk = _ctxSemphore.Wait(SML_INFINITE);
    if(waitOk)
    {
        if (_glctx)
        {
            MakeCurrentCtx(__FUNCTION__, __FILE__);

            GLFinalize();

            delete _paintDev;
            _paintDev = nullptr;

            DoneCurrentCtx();
        }

        _ctxSemphore.Notify(false);
    }
}

bool SmlGLWindow::MakeCurrentCtx(const char* msg, const char* msg1)
{
    bool ok = _glctx->makeCurrent(this);
    Q_ASSERT_X(ok && _glctx->isValid(), msg, msg1);
    return ok;
}

void SmlGLWindow::DoneCurrentCtx()
{
    if (_multiThreadMode) //required for multi threaded mode rendering
    {
        _glctx->doneCurrent();
    }

}

void SmlGLWindow::RequestCtx()
{
    emit RequestCtxSignal(); //SmlThreadGLWindow::ResponseCtx
    if constexpr (false)
    {
		connect(this, &SmlGLWindow::RequestCtxSignal,
			this, &SmlGLWindow::ResponseCtx);
    }

    const ulong SML_INFINITE = -1UL;
    _eventCtxResponsed.Wait(SML_INFINITE);
}

inline void SmlGLWindow::checkCompileErrors(GLuint shader, const char* type)
{
    GLint success;
    GLchar infoLog[1024];
    if (0 == stricmp(type, "PROGRAM"))
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, _countof(infoLog), NULL, infoLog);
            qDebug() << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
                << "\n" << infoLog
                << "\n -- --------------------------------------------------- -- ";
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, _countof(infoLog), NULL, infoLog);
            qDebug() << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
                << "\n" << infoLog
                << "\n -- --------------------------------------------------- -- ";
        }
    }
}

GLuint SmlGLWindow::CreateProgram(const GLchar* const vertSource, const GLchar* const geomSource, const GLchar* const fragSource)
{
    /////////////////////////////////////////////////////////////////
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL); // vertex_shader_source is a GLchar* containing glsl shader source code
    glCompileShader(vertShader);
    checkCompileErrors(vertShader, "VERTEX");

    ///////////////////////////////////////////////////////////////
    GLuint geomShader = 0;
    if (geomSource && geomSource[0])
    {
        geomShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geomShader, 1, &geomSource, NULL); // vertex_shader_source is a GLchar* containing glsl shader source code
        glCompileShader(geomShader);
        checkCompileErrors(geomShader, "GEOMETRY");
    }

    /////////////////////////////////////////////////////////////////
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL); // vertex_shader_source is a GLchar* containing glsl shader source code
    glCompileShader(fragShader);
    checkCompileErrors(fragShader, "FRAGMENT");

    /////////////////////////////////////////////////////////////////
    GLuint programId = glCreateProgram();

    if (vertShader)
    {
        glAttachShader(programId, vertShader);
    }

    if (geomShader)
    {
        glAttachShader(programId, geomShader);
    }

    if (fragShader)
    {
        glAttachShader(programId, fragShader);
    }
    

    glLinkProgram(programId);
    checkCompileErrors(programId, "PROGRAM");

    /////////////////////////////////////////////////////////////////
    if (vertShader)
    {
        glDeleteShader(vertShader);
        vertShader = 0;
    }

    if (geomShader)
    {
        glDeleteShader(geomShader);
        geomShader = 0;
    }

    if (fragShader)
    {
        glDeleteShader(fragShader);
        fragShader = 0;
    }

    return programId;
}

void SmlGLWindow::ResponseCtx(/*QThread* targetThread*/)
{
    const ulong timeOut = 500;
    bool waitOk = _ctxSemphore.Wait(timeOut);
    if(waitOk)
    {
        _glctx->moveToThread(_thread);
        _ctxResponsedOk = true;
    }
    else
    {
         _ctxResponsedOk = false;
    }

    _eventCtxResponsed.Notify(false);
}

void SmlGLWindow::SetAnimating(bool run)
{
    _animating = run;
    if (_animating)
    {
        connect(this, &SmlGLWindow::RenderFrameDoneSignal,
                this, &SmlGLWindow::requestUpdate);
        requestUpdate();
    }
    else
    {
        disconnect(this, &SmlGLWindow::RenderFrameDoneSignal,
                   this, &SmlGLWindow::requestUpdate);
    }
}

SmlGLWindow::SmlGLWindow(QWindow* parent,
                                     bool requestMode /*= false*/,
                                     bool multiThreadMode /*= true*/)
    : QWindow(parent),
      _requestMode{ requestMode },
      _multiThreadMode{ multiThreadMode }
{
    setSurfaceType(QSurface::OpenGLSurface);

    if (_multiThreadMode)
    {
        _thread = new QThread{ this };
        _render = new SmlThreadGLRender{ nullptr, this };
        _render->moveToThread(_thread);

        connect(_thread, &QThread::finished,
                _render, &QObject::deleteLater);
        connect(this, &SmlGLWindow::RequestRenderSignal,
                _render, &SmlThreadGLRender::Render);

        if (_requestMode)
        {
            connect(this, &SmlGLWindow::RequestCtxSignal,
                    this, &SmlGLWindow::ResponseCtx);
        }

        _thread->start();
    }
}

SmlGLWindow::~SmlGLWindow()
{
    if (_multiThreadMode)
    {
        _thread->quit();
        _thread->wait();
    }

    if (_glctx)
    {
        _glctx->deleteLater();
        _glctx = nullptr;
    }

}
