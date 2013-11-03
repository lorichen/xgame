//
//  testAppDelegate.h
//  test
//
//  Created by 陈 波 on 13-11-2.
//  Copyright (c) 2013年 陈 波. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EAGLView.h"

@class ViewController;

@interface testAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong,nonatomic) ViewController *viewController;

@property (strong,nonatomic) EAGLView *glView;

@end
