#include <QtGlobal>

#include "SmlThreadGLWindow.h"
#include <QMutexLocker>


SmlThreadGLRender::SmlThreadGLRender(QObject* parent, SmlThreadGLWindow* window) :
    QObject{ parent },
    _glwin{ window }
{
}

void SmlThreadGLRender::Render()
{
    _glwin->ThreadRender();
}


void SmlThreadGLWindow::RequestRender()
{

    if (!_multiThreadMode)
    {
        Render();
        emit RenderFrameDoneSignal();
        return;
    }


    if (_requestMode)
    {
        emit RequestRenderSignal();
        return;
    }

    const ulong timeOut = 500;
    bool waitOK = _ctxSemphore.Wait(timeOut);
    if (!waitOK)
    {
        return;
    }

    _glctx->moveToThread(_thread);


    emit RequestRenderSignal();
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

void SmlThreadGLWindow::ThreadRender()
{

    if (_requestMode)
    {
        RequestCtx();
    }

    xxxDelay();

    Render();
    _glctx->moveToThread(this->thread());

    _ctxSemphore.Notify(false); //ok to use opengl context in main thread

    emit RenderFrameDoneSignal();
}

void SmlThreadGLWindow::Render()
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
void SmlThreadGLWindow::paintEvent(QPaintEvent* ev)
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

void SmlThreadGLWindow::resizeEvent(QResizeEvent* ev)
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


bool SmlThreadGLWindow::event(QEvent* ev)
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

void SmlThreadGLWindow::FinalizeGL()
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

bool SmlThreadGLWindow::MakeCurrentCtx(const char* msg, const char* msg1)
{
    bool ok = _glctx->makeCurrent(this);
    Q_ASSERT_X(ok && _glctx->isValid(), msg, msg1);
    return ok;
}

void SmlThreadGLWindow::DoneCurrentCtx()
{
    if (_multiThreadMode) //required for multi threaded mode rendering
    {
        _glctx->doneCurrent();
    }

}

void SmlThreadGLWindow::RequestCtx()
{
    emit RequestCtxSignal(/*QThread::currentThread()*/);

    const ulong SML_INFINITE = -1UL;
    _eventCtxResponsed.Wait(SML_INFINITE);
}

void SmlThreadGLWindow::ResponseCtx(/*QThread* targetThread*/)
{
    const ulong SML_INFINITE = -1UL;
    bool waitOk = _ctxSemphore.Wait(SML_INFINITE);
    if(waitOk)
    {
        _glctx->moveToThread(_thread);
        _eventCtxResponsed.Notify(false);
    }
}

void SmlThreadGLWindow::SetAnimating(bool run)
{
    _animating = run;
    if (_animating)
    {
        connect(this, &SmlThreadGLWindow::RenderFrameDoneSignal,
                this, &SmlThreadGLWindow::requestUpdate);
        requestUpdate();
    }
    else
    {
        disconnect(this, &SmlThreadGLWindow::RenderFrameDoneSignal,
                   this, &SmlThreadGLWindow::requestUpdate);
    }
}

SmlThreadGLWindow::SmlThreadGLWindow(QWindow* parent,
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
        connect(this, &SmlThreadGLWindow::RequestRenderSignal,
                _render, &SmlThreadGLRender::Render);

        if (_requestMode)
        {
            connect(this, &SmlThreadGLWindow::RequestCtxSignal,
                    this, &SmlThreadGLWindow::ResponseCtx);
        }

        _thread->start();
    }
}

SmlThreadGLWindow::~SmlThreadGLWindow()
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
