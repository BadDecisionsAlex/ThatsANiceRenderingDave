# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.utgraphicsutil.Debug:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Debug/libutgraphicsutil.a:
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Debug/libutgraphicsutil.a


PostBuild.particles.Debug:
PostBuild.utgraphicsutil.Debug: /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/Debug/particles
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/Debug/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libGLEW.dylib\
	/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Debug/libutgraphicsutil.a\
	/usr/local/lib/libGLEW.dylib
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/Debug/particles


PostBuild.utgraphicsutil.Release:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Release/libutgraphicsutil.a:
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Release/libutgraphicsutil.a


PostBuild.particles.Release:
PostBuild.utgraphicsutil.Release: /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/Release/particles
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/Release/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libGLEW.dylib\
	/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Release/libutgraphicsutil.a\
	/usr/local/lib/libGLEW.dylib
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/Release/particles


PostBuild.utgraphicsutil.MinSizeRel:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/MinSizeRel/libutgraphicsutil.a:
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/MinSizeRel/libutgraphicsutil.a


PostBuild.particles.MinSizeRel:
PostBuild.utgraphicsutil.MinSizeRel: /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/MinSizeRel/particles
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/MinSizeRel/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libGLEW.dylib\
	/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/MinSizeRel/libutgraphicsutil.a\
	/usr/local/lib/libGLEW.dylib
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/MinSizeRel/particles


PostBuild.utgraphicsutil.RelWithDebInfo:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/RelWithDebInfo/libutgraphicsutil.a:
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/RelWithDebInfo/libutgraphicsutil.a


PostBuild.particles.RelWithDebInfo:
PostBuild.utgraphicsutil.RelWithDebInfo: /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/RelWithDebInfo/particles
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/RelWithDebInfo/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libGLEW.dylib\
	/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/RelWithDebInfo/libutgraphicsutil.a\
	/usr/local/lib/libGLEW.dylib
	/bin/rm -f /Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/bin/RelWithDebInfo/particles




# For each target create a dummy ruleso the target does not have to exist
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Debug/libutgraphicsutil.a:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/MinSizeRel/libutgraphicsutil.a:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/RelWithDebInfo/libutgraphicsutil.a:
/Users/Matt/Desktop/Graphics/ThatsANiceRenderingDave/XcodeBuild/Release/libutgraphicsutil.a:
/usr/local/lib/libGLEW.dylib:
