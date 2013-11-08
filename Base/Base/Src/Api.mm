#include "StdAfx.h"
#include "Api.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

namespace xs {
    
bool GetClientRect(void* hwnd,Rect* rc)
{
    UIView* view = (UIView*)hwnd;
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)(view->_layer);
    CGSize					newSize;
    newSize = [eaglLayer bounds].size;
    
    rc->left = 0;
    rc->top = 0;
    rc->right = 2 * newSize.width;
    rc->bottom = 2 * newSize.height;
    return true;
}

}