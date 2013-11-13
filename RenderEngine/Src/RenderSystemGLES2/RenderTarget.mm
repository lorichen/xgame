#include "StdAfx.h"
#include "TextureManager.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "RenderSystem.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>


struct OGLES2Context
{
    UIView* view;
    EAGLContext *context;
    GLuint viewRenderbuffer;
    GLuint viewFramebuffer;
    GLuint depthRenderbuffer;
};

static float g_scaleFactor = 1.0f;
float GetScaleFactor()
{
    return g_scaleFactor;
}


void RenderTarget::Present()
{
    if (m_pContext) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pContext->viewFramebuffer);
        GLenum attachments[] = { GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0 };
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 2, attachments);
        
        
        glBindRenderbuffer(GL_RENDERBUFFER, m_pContext->viewRenderbuffer);
        [m_pContext->context presentRenderbuffer:GL_RENDERBUFFER];
    }
}

bool RenderTarget::_CreateIOS(void* view,void* shareContext)
{
    
    m_pContext = new OGLES2Context;
    
    if(view)
    {
        m_pContext->view = (UIView*)(view);
        
        //get view scale factor
        CGFloat f = m_pContext->view.contentScaleFactor;
        if ([m_pContext->view respondsToSelector:@selector(contentScaleFactor)])
        {
            m_pContext->view.contentScaleFactor = [[UIScreen mainScreen] scale];
        }
        f = m_pContext->view.contentScaleFactor;
        g_scaleFactor = f;
    
        //init eagl
        CAEAGLLayer* eaglLayer = (CAEAGLLayer *)(m_pContext->view.layer);
    
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    }
    else
    {
        //todo...
    }
    
    //muilt thread gles work status todo....or not!
    if(shareContext )
    {
       //todo... ogles share object
    }
    else
    {
        //create opengles2 !
         m_pContext->context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }
    
    if (!m_pContext->context || ![EAGLContext setCurrentContext:m_pContext->context]) {
        return false;
    }
    
    //make current render context for this thread!
    [EAGLContext setCurrentContext:m_pContext->context];
    glBindFramebuffer(GL_FRAMEBUFFER, m_pContext->viewFramebuffer);
    m_FrameBufferObj = m_pContext->viewFramebuffer;
    return true;
}

void RenderTarget::_DestoryIOS()
{
    if (m_pContext) {
        if ([EAGLContext currentContext] == m_pContext->context) {
            [EAGLContext setCurrentContext:nil];
        }
        
        [m_pContext->context release];
        delete m_pContext;
        m_pContext = NULL;
    }
    
}

void RenderTarget::GetClientRect(xs::Rect* rc)
{
    float f = GetScaleFactor();
#if 1
    CAEAGLLayer* eaglLayer = (CAEAGLLayer *)(m_pContext->view.layer);
    CGSize	newSize;
    newSize = [eaglLayer bounds].size;
    
    rc->left = 0;
    rc->top = 0;
    rc->right = newSize.width * f;
    rc->bottom = newSize.height * f;
#else
    xs::GetClientRect(m_pContext->view, rc);
    rc->right *= f;
    rc->bottom *= f;
#endif
}

void* RenderTarget::getView()
{
    return m_pContext->view;
}

void* RenderTarget::getRenderContext()
{
    return m_pContext->context;
}

GLuint  RenderTarget::GetFrameBufferObj()
{
    if(m_pContext)
        return m_pContext->viewFramebuffer;
    
    return 0;
}


bool RenderTarget::Reset()
{
    if (m_pContext) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)(m_pContext->view.layer);
        
        //CGSize					newSize;
        //newSize = [eaglLayer bounds].size;
        //newSize.width = roundf(newSize.width);
        //newSize.height = roundf(newSize.height);
        
        [EAGLContext setCurrentContext:m_pContext->context];
        glDeleteFramebuffers(1, &(m_pContext->viewFramebuffer));
        m_pContext->viewFramebuffer = 0;
        glDeleteRenderbuffers(1, &(m_pContext->viewRenderbuffer));
        m_pContext->viewRenderbuffer = 0;
        
        if(m_pContext->depthRenderbuffer) {
            glDeleteRenderbuffers(1, &(m_pContext->depthRenderbuffer));
            m_pContext->depthRenderbuffer = 0;
        }
        
        
        glGenFramebuffers(1, &(m_pContext->viewFramebuffer));
        glGenRenderbuffers(1, &(m_pContext->viewRenderbuffer));
        
        glBindFramebuffer(GL_FRAMEBUFFER, m_pContext->viewFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_pContext->viewRenderbuffer);
        [m_pContext->context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)(m_pContext->view.layer)];
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_pContext->viewRenderbuffer);
        
        GLint backingWidth, backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        if (1) {
            glGenRenderbuffers(1, &(m_pContext->depthRenderbuffer));
            glBindRenderbuffer(GL_RENDERBUFFER, m_pContext->depthRenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_pContext->depthRenderbuffer);
        }
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return false;
        }
        
        //m_nScreenW = backingWidth;
        //m_nScreenH = backingHeight;
        //m_pScreenTarget->UpdateFrameBuffer(m_pContext->viewFramebuffer, m_nScreenW, m_nScreenH);
        return true;
    }
    
    
    return false;
}