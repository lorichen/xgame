//
//  EAGLView.m
//  Hello_Triangle
//
//  Created by Dan Ginsburg on 6/13/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"

//#include "UITestIOS.h"
#include <unistd.h>
#include <stdio.h>

#if 0
#include "Base.h"
#else
#include "Common.h"
#include "StringHelper.h"
#include "Thread.h"
#include "FileSystem/Stream.h"
#include "FileSystem/MemStream.h"
#endif


#include "Re.h"
#include "IRenderSystem.h"
#include "IRenderEngine.h"
#include "Api.h"

#define USE_DEPTH_BUFFER 1

using namespace xs;

#include "../app_wrap.h"


// A class extension to declare private methods
@interface EAGLView () {
    //ITestCase* testCase;
    id displayLink;
    BOOL animating;
}

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;


@end


@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;


// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}


- (id) initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        
        NSString* path = [[NSBundle mainBundle] bundlePath];
        chdir([path cStringUsingEncoding:NSASCIIStringEncoding]);
        
        std::string str;
        //str = "data/Shader/OGLES2/sp_v3_uv_c.vs";
        //str = "myconfig";
        str = "test";
        //FILE* fp = fopen(str.c_str(),"r");

        
        CGSize size = [self bounds].size;
        
        if ([self respondsToSelector:@selector(setContentScaleFactor:)])
		{
			self.contentScaleFactor = [[UIScreen mainScreen] scale];
            size.width *= self.contentScaleFactor;
            size.height *= self.contentScaleFactor;
		}
        
        AppWrap::setViewSize(size.width, size.height);
        //init
        AppWrap::init(self);
        
        animationInterval = 1.0 / 60.0;
        animating = FALSE;
        displayLink = nil;
    }
    
    return self;
}

- (void)dealloc {
    
    [self stopAnimation];

    
    //uninit;
    AppWrap::uninit();
    
    [super dealloc];
}

- (void)drawView:(id)sender {
    
    //update and render!
 
    int delta_ms = 1000/60;
    AppWrap::update(getTickCount(),delta_ms);
    
//    [EAGLContext setCurrentContext:context];
//    
//    glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
//	
//	
//    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
//    [context presentRenderbuffer:GL_RENDERBUFFER];
}


- (void)layoutSubviews {
//    [EAGLContext setCurrentContext:context];
//    [self destroyFramebuffer];
//    [self createFramebuffer];
    /*
    if (testCase)
    {
        testCase->OnResize(0,0);
    }
    */
    [self drawView:self];
}

- (void)startAnimation {
    
    if (!animating)
    {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setFrameInterval:animationInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        animating = TRUE;
    }

    
//    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
    if (animating)
    {
        [displayLink invalidate];
        displayLink = nil;
        animating = FALSE;
    }
    //self.animationTimer = nil;
}


//- (void)setAnimationTimer:(NSTimer *)newTimer {
//    [animationTimer invalidate];
//    animationTimer = newTimer;
//}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
//    animationInterval = 60 * interval;
//    if (animationTimer) {
//        [self stopAnimation];
//        [self startAnimation];
//    }
    
    if (interval >= 1)
    {
        animationInterval = interval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    originalLocation = [touch locationInView:self];
}
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint currentLocation = [touch locationInView:self];
    CGPoint d;
    d.x = currentLocation.x - originalLocation.x;
    d.y = currentLocation.y - originalLocation.y;
    AppWrap::move(-d.x,-d.y);
    originalLocation = currentLocation;
    
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}


@end
