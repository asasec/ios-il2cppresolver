#import <UIKit/UIKit.h>
#include "IL2CPP_Resolver.hpp"
#import "helper/Menu.h"
#import "helper/Dumpil2cpp.mm"

// Dump.mm içerisindeki uyarı fonksiyonu referansı
extern void showNativeAlert(NSString *title, NSString *message);

__attribute__((constructor)) void initializeDumpMenu() {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSBundle *bundle = [NSBundle mainBundle];
        NSString *frameworkPath = [[bundle.bundlePath stringByAppendingPathComponent:@"Frameworks/UnityFramework.framework/UnityFramework"] stringByStandardizingPath];
        
        bool initSuccess = IL2CPP::Initialize(true, 40, [frameworkPath UTF8String]);
        if (!initSuccess) {
            initSuccess = IL2CPP::Initialize(true, 10, "UnityFramework");
        }

        if (!initSuccess) {
            showNativeAlert(@"Olmadı / Başlangıç Hatası", @"UnityFramework yüklenemedi!");
            return;
        }

        UIWindow *window = nil;
        if (@available(iOS 13.0, *)) {
            for (UIWindowScene *scene in [UIApplication sharedApplication].connectedScenes) {
                if (scene.activationState == UISceneActivationStateForegroundActive) {
                    for (UIWindow *win in scene.windows) {
                        if (win.isKeyWindow) {
                            window = win;
                            break;
                        }
                    }
                }
            }
        }
        if (!window) {
            window = [UIApplication sharedApplication].windows.firstObject;
        }
        
        if (window) {
            ImGuiStyleMenuView *menuView = [[ImGuiStyleMenuView alloc] initWithFrame:window.bounds];
            menuView.userInteractionEnabled = YES;
            [window addSubview:menuView];
        }
    });
}
