#pragma once

#include <QObject>
#include "SmlGLWindow.h"

#include <glm/glm.hpp>
#include "SmlAxisCoord.h"

class SmlGLWindowTriangle : public SmlGLWindow
{
	Q_OBJECT

private:
	using XQTBase = SmlGLWindow;

private:
	bool _isAnimating{ false };
	int _counter{ 0 };




private:
	GLuint _programId{ GLuint(-1) };
	GLuint _vao{ GLuint(-1) };

	GLuint _vboPos{ GLuint(-1) };
	GLuint _vboColor{ GLuint(-1) };
	GLuint _vboTextCoord{ GLuint(-1) };
	GLuint _vboElemet{ GLuint(-1) };

	GLuint _texture{ GLuint(-1) };

	//    GLuint _vboPosLine{GLuint(-1)};
	//    GLuint _vboColorLine{GLuint(-1)};
	//    GLuint _vboElemetLine{GLuint(-1)};



	GLint _mvpLocation{ -1 };
    GLint _texSamplerLocation{ -1 };

	GLint _nearFarMaxFogLocation{ -1 };
	GLint _fogColorLocation{ -1 };

	//QTimer* _updateTimer{nullptr};


	//glm::mat4 _eyeAxis{1.0f};
	//glm::vec3 _eye{0.0f, 0.0f, 0.0f};

	float _offsetZ{ 0.0f };
	bool _dirInc{ false };

	bool _axisInited{ false };

	inline static constexpr int posLocation = 0;
	inline static constexpr int colorLocation = 1;
	inline static constexpr int texCoordLocation = 2;


	SmartLib::AxisCoord<float> _axisModel;
	SmartLib::AxisCoord<float> _axisEye;
	glm::mat4 _frustum;

	float _nearPlane{ 0 };
	float _farPlane{ 0 };
	//QColor _bgcolor;



private:
	virtual void GLInitialize() override;
	virtual void GLResize(const QSize& size, const QSize& oldSize) override;
	virtual void GLPaint(QPaintDevice* paintDev) override;
	virtual void GLFinalize() override;

private:
	virtual void keyPressEvent(QKeyEvent* ev) override;

private slots:
	void on_timeout();



public:
	SmlGLWindowTriangle(QWindow *parent);
	virtual ~SmlGLWindowTriangle() override;
};
