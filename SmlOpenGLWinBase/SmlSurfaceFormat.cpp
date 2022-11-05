#include "SmlSurfaceFormat.h"

#if defined(_USE_OPENGL_COMPT)
#define QSurfaceFormat_PROFILE QSurfaceFormat::CompatibilityProfile
#else
#define QSurfaceFormat_PROFILE QSurfaceFormat::CoreProfile
#endif

void SmlSurfaceFormatUtils::SurfaceFormat()
{
	QSurfaceFormat defaultFormat;
	defaultFormat.setVersion(4, 5);
	defaultFormat.setProfile(QSurfaceFormat_PROFILE);
	defaultFormat.setOptions(QSurfaceFormat::DebugContext);
	QSurfaceFormat::setDefaultFormat(defaultFormat);
}
