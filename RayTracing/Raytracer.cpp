//=============================================================================
//
//  CLASS Raytracer - IMPLEMENTATION
//
//=============================================================================
#define RAYTRACER_WHITTED_CPP

//== INCLUDES =================================================================
#include <assert.h>
#include <QCoreApplication> // for qApp
#include <algorithm>
#include <iostream>
#include <QMainWindow>
#include <QProgressBar>
#include "Raytracer.h"




//== IMPLEMENTATION ==========================================================
// The main function of the raytracer: Here the rays are generated
void Raytracer::render(
    SurfaceList     *pclScene, // The geometry of the scene
    unsigned int    uiMaxDepth, // Maximum recursion depth
    QImage          *clQImage, // the finished picture
    RenderingMode   mode // Indicates whether the raytracer is in the timing mode or in the ad mode
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ColorType   col;
	Ray         cl_ray;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Inititialize progress Bar if in Timing Mode
	QProgressDialog progressDialog;
	progressDialog.setRange(0, clQImage->width() );
	progressDialog.setLabelText("Ray Tracing Progress");
	progressDialog.setModal(true);
    

	// Initialisierung der Kamera
	m_clPinholeCamera.setAspectRatio( clQImage->width() / (double) clQImage->height() );

    // For all pixels of the picture
	for ( int i = 0; i < clQImage->width(); ++i )
	{
		progressDialog.setValue(i);
		if ( progressDialog.wasCanceled() )
			break;
		qApp->processEvents();

		for ( int j = 0; j < clQImage->height(); ++j )
		{
            // OpenGL stuff, not important to understanding raytracers
            // Therefore please do not pay attention
            // Who wants to know: In glRays the rays are saved, the
            // will be displayed later in the OpenGL window if you enable "Show Rays"
			std::vector<GLRay> *glRays;
			if ( mode == MODE_DISPLAY )
			{
				glRays = new std::vector<GLRay>;
				glRays->clear();
			}
			else
				glRays = NULL;

            // Generate ray
			Ray ray = getCamera().generateRay( i, j, clQImage->width(), clQImage->height() );
			assert( !ray.getDir().isNull() );

            // trace Ray into the scene and get color
			col = traceRay( pclScene, ray, uiMaxDepth, glRays );

            // Convert color to Qt colors (approximately 0..1 -> 0..255)
			col *= 255;
			col.setX( fmax( 0, fmin(col.x(), 255 ) ) );
			col.setY( fmax( 0, fmin(col.y(), 255 ) ) );
			col.setZ( fmax( 0, fmin(col.z(), 255 ) ) );

			// Setzen des Pixels im Qt-Image
			clQImage->setPixel( i, j, qRgb( col.x(), col.y(), col.z() ) );

            // Display the beams and the image when timing disabled
			if ( mode == MODE_DISPLAY )
			{
				emit updateRays( glRays );
			}
		}
	}
}

// Tracking the rays through the scene
ColorType Raytracer::traceRay(
    const Surface       *pclScene,
    const Ray           &crclRay,
    unsigned int        uiDepth,
    std::vector<GLRay>  *glRays )
{
	ColorType col;

    // Maximum recursion depth reached?
	if ( 0 == uiDepth )
		return col;
	--uiDepth;

	TracingContext context;

    // Calculate the next intersection with the scene
	if ( pclScene->intersect(crclRay, SELF_ISECT_OFFSET, FLT_MAX, context) ) {
		assert(context.m_t > 0);
        // Outgoing direction is the inverted directional vector of the incident beam
		context.m_v3Outgoing = -crclRay.getDir();
		context.m_v3Outgoing.normalize();

        // context.m_pclShader contains after the intersection calculation the Phong parameters for the currently next object
		const Shader* shader = context.m_pclShader;

        QVector3D outn = context.m_v3Normal;
        if (QVector3D::dotProduct(crclRay.getDir(), context.m_v3Normal) > 0.0) {
            outn = -outn;
        }
      
        
        // Remember normal and intersection, since context is changed by shader (...)
		QVector3D n(outn);
		QVector3D h( context.m_v3HitPoint);

        float fresnel;
        float cos1 = -QVector3D::dotProduct(crclRay.getDir(), n);
        float ro = (shader->getRefractiveIndex() - 1)/(1 + shader->getRefractiveIndex());
        ro *= ro;
        if(cos1 <=0 )
            fresnel = 1;
        else{
            float x = 1-cos1;
            fresnel = ro +(1-ro)*x*x*x*x*x;}
        
        
        col = shade( pclScene, context);
        ColorType reflectioncolor;
        ColorType refractioncolor;
		if (shader->isSpecular() ) {
            Ray reflect_ray = crclRay.reflectedRay(h, n);
            reflectioncolor = traceRay(pclScene, reflect_ray, uiDepth, glRays);
            col += reflectioncolor*shader->getSpecularColor();
		}
        
       
		if ( shader->isRefractive() ) {
            Ray refract;
            
            if(crclRay.refractedRay(h, n, shader->getRefractiveIndex(), refract)){
      
                
                
                refractioncolor = traceRay(pclScene, refract, uiDepth, glRays);
                
              //  Ray reflect_ray = crclRay.reflectedRay(h, n);
               
            //   reflectioncolor =  traceRay(pclScene, reflect_ray, uiDepth, glRays);
               
                col += refractioncolor*shader->getRefractiveColor();//*(1-fresnel+ reflectioncolor*shader->getSpecularColor()*fresnel;
            
                
            }

		}
     
 
        // set the rays for the OpenGL window
        // Not important for the understanding of the raytracer
		if ( glRays != NULL ) {
			GLRay   glray;
			glray.m_start = crclRay.getOrigin();
			glray.m_stop = context.m_v3HitPoint;
			glRays->push_back( glray );
		}
     
        return col;
	}
    else {
        // set the rays for the OpenGL window
        // Not important for the understanding of the raytracer
		if ( glRays != NULL ) {
			GLRay   glray;
			glray.m_start = crclRay.getOrigin();
			glray.m_stop = crclRay.getOrigin() + crclRay.getDir() * 10000;
			glRays->push_back( glray );
		}
        // If the ray does not intersect the scene: Return the background color
		return m_colBackground;
	}
}

// Implements the Phong Lighting Model
ColorType Raytracer::shade( const Surface *pclScene, TracingContext  &tContext )
{
    /*
    The local lighting model is derived from shaders
    Classes defined. Interesting here are the virtual methods
    shadeAmbient () and shade (). The current shader is located
    in the argument tContext.m_
    The light sources are located in the STL container m_LightList.
    Light sources are derived from light and implement
    the method shadowFeeler (), which beam direction, validity range
    and intensity.
    The check cut beam scene can be over the method
    Surface scene-> intersect (...) are queried. It is general
    in Surface.h, from which each object is derived.
    */
    // set the ambient term of the color

    // for all light sources
	LightListType::const_iterator it = m_LightList.begin();
	LightListType::const_iterator it_end = m_LightList.end();
    const Shader* shader = tContext.m_pclShader;
    PinholeCamera cam;
     ColorType color(shader->getAmbientColor());
    ColorType accu;
     QVector3D n(tContext.m_v3Normal);

	for ( ; it != it_end; ++it )
	{

            Ray shawdowRay;
            double domain;
            ColorType light;

        (*it) -> shadowFeeler(shawdowRay, domain, light);
        shawdowRay.setOrigin(tContext.m_v3HitPoint);
    
        double NdotI =  std::max(0.0f, QVector3D::dotProduct(n,  shawdowRay.getDir().normalized()));
//if(NdotI > 0.0)
  //  {

    if(!pclScene->intersect(shawdowRay, SELF_ISECT_OFFSET, FLT_MAX, tContext))
    {
        TracingContext tmp;
        tmp.m_v3Incoming = shawdowRay.getDir();
        tmp.m_v3Outgoing = cam.getEye();
        tmp.m_v3Normal = tContext.m_v3Normal;
        tmp.m_v3HitPoint = tContext.m_v3HitPoint;
        accu += shader->shade(tmp)*light*NdotI;

        
    }


    //}
}
    color += accu;
    return color;
}

// reset the light sources
void Raytracer::removeAllLights()
{
	if ( m_bOwnsLights )
	{
		LightListType::iterator it = m_LightList.begin();
		LightListType::iterator it_end = m_LightList.end();
		for ( ; it != it_end; ++it )
		{
			delete *it ;
		}
	}
	m_LightList.clear();
}


// Drawing the light sources in OpenGL: not important for the raytracer
void Raytracer::renderLightsGL( QOpenGLFunctions_1_5 &opengl )
{
	LightListType::iterator it = m_LightList.begin();
	LightListType::iterator it_end = m_LightList.end();
	opengl.glEnable( GL_LIGHTING );
	for ( unsigned int lightcounter = 0; it != it_end; ++it )
	{
		GLint li;
		switch ( lightcounter )
		{
		    case 0:     li = GL_LIGHT0; break;
		    case 1:     li = GL_LIGHT1; break;
		    case 2:     li = GL_LIGHT2; break;
		    case 3:     li = GL_LIGHT3; break;
		    case 4:     li = GL_LIGHT4; break;
		    case 5:     li = GL_LIGHT5; break;
		    case 6:     li = GL_LIGHT6; break;
		    case 7:     li = GL_LIGHT7; break;
		    default:    li = GL_LIGHT7; break;
		}
		( *it )->renderGL( opengl, li );
		opengl.glEnable( li );
		++lightcounter;
	}
}
