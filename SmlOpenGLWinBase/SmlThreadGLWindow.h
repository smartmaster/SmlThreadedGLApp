#pragma once

#include <QObject>
#include <QWindow>
#include <QPaintDevice>
#include <QEvent>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QOpenGLPaintDevice>
#include <QOpenGLContext>
#include <QSize>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#if defined(_USE_OPENGL_COMPT)
#include <QOpenGLFunctions_4_5_Compatibility>
#define QOpenGLFunctions_PROFILE QOpenGLFunctions_4_5_Compatibility
#else
#include <QOpenGLFunctions_4_5_Core>
#define QOpenGLFunctions_PROFILE QOpenGLFunctions_4_5_Core
#endif


#include "SmlWaitObject.h"

class SmlThreadGLWindow;
class SmlThreadGLRender : public QObject
{
    Q_OBJECT

private:
    SmlThreadGLWindow* _glwin{ nullptr };

public:
    SmlThreadGLRender(QObject* parent, SmlThreadGLWindow* window);

public slots:
    void Render();
};



class SmlThreadGLWindow : public QWindow, protected QOpenGLFunctions_PROFILE
{
    Q_OBJECT

private:
    friend class SmlThreadGLRender;
    using SML_QTBase = QWindow;

private:
    QOpenGLPaintDevice* _paintDev{ nullptr };
    QOpenGLContext* _glctx{ nullptr };

    int _eventCounter{ 0 };
    bool _animating{ false };
    bool _SurfaceDestroyed{ false };



    QThread* _thread{nullptr};
    SmlThreadGLRender* _render{ nullptr };
    SmlSemphore _ctxSemphore{ 1 };

    bool _requestMode{ false };
    bool _ctxResponsedOk{false};
    SmlEvent _eventCtxResponsed{true};

    bool _multiThreadMode{ true };


private:
    void ThreadRender();
    void Render();
    void RequestRender();
    void FinalizeGL();

    bool MakeCurrentCtx(const char* msg, const char* msg1);
    void DoneCurrentCtx();

    void RequestCtx();

private:
    void checkCompileErrors(GLuint shader, const char* type);
protected:
    GLuint CreateProgram(const GLchar* const vertSource, const GLchar* const  geomSource, const GLchar* const  fragSource);

public slots:
    void ResponseCtx(/*QThread* targetThread*/);

signals:
    void RequestCtxSignal(/*QThread* targetThread*/);
    void RequestRenderSignal();
    void RenderFrameDoneSignal();

private:
    virtual bool event(QEvent* ev) override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual void paintEvent(QPaintEvent* ev) override;
#else
    virtual void exposeEvent(QExposeEvent* ev) override;
#endif

    virtual void resizeEvent(QResizeEvent* ev) override;


protected:
    virtual void GLInitialize() = 0;
    virtual void GLResize(const QSize& size, const QSize& oldSize) = 0;
    virtual void GLPaint(QPaintDevice* paintDev) = 0;
    virtual void GLFinalize() = 0;

public:
    void SetAnimating(bool run);

public:
    SmlThreadGLWindow(QWindow* parent, bool requestMode = false, bool multiThreadMode = true);
    virtual ~SmlThreadGLWindow();
};
