//
//  testAppDelegate.m
//  test
//
//  Created by 陈 波 on 13-11-2.
//  Copyright (c) 2013年 陈 波. All rights reserved.
//

#import "testAppDelegate.h"
#import "ViewController.h"

@implementation testAppDelegate

-(void) dealloc
{
    [_glView release];
    [_window release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    CGRect frame = CGRectMake(0, 0, screenBounds.size.height, screenBounds.size.width);
    self.window = [[[UIWindow alloc] initWithFrame:frame] autorelease];
    self.viewController = [[[ViewController alloc] initWithNibName:nil bundle:nil] autorelease];
    self.glView = [[[EAGLView alloc] initWithFrame:frame] autorelease];
    
    self.viewController.view = self.glView;
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    
    //self.glView.animationInterval = 1.0 / 60.0;
	[self.glView startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
