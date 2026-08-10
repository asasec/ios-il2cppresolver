#import <UIKit/UIKit.h>
#include "../IL2CPP_Resolver.hpp"
#include <fstream>
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <string>

using namespace IL2CPP;

typedef void* (*DomainGet_t)();
typedef void** (*DomainGetAssemblies_t)(void* domain, size_t* size);
typedef void* (*AssembliesGetImage_t)(void* assembly);
typedef const char* (*ImageGetName_t)(void* image);
typedef int (*ImageGetClassCount_t)(void* image);
typedef void* (*ImageGetClass_t)(void* image, int index);
typedef void* (*ClassGetMethods_t)(void* klass, void** iter);

typedef const char* (*il2cpp_class_get_name_t)(void* klass);
typedef const char* (*il2cpp_class_get_namespace_t)(void* klass);
typedef const char* (*il2cpp_method_get_name_t)(void* method);
typedef const char* (*il2cpp_type_get_name_t)(void* type);
typedef void* (*il2cpp_method_get_return_type_t)(void* method);
typedef uint32_t (*il2cpp_method_get_param_count_t)(void* method);
typedef const char* (*il2cpp_method_get_param_name_t)(void* method, uint32_t index);
typedef void* (*il2cpp_method_get_param_t)(void* method, uint32_t index);

void showNativeAlert(NSString *title, NSString *message) {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
        [alert addAction:[UIAlertAction actionWithTitle:@"Tamam" style:UIAlertActionStyleDefault handler:nil]];
        
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
        [window.rootViewController presentViewController:alert animated:YES completion:nil];
    });
}

void shareDumpFile(NSString *filePath) {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSURL *fileURL = [NSURL fileURLWithPath:filePath];
        UIActivityViewController *activityVC = [[UIActivityViewController alloc] initWithActivityItems:@[fileURL] applicationActivities:nil];
        
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
        
        UIViewController *rootVC = window.rootViewController;
        if (!rootVC) return;
        
        if (activityVC.popoverPresentationController) {
            activityVC.popoverPresentationController.sourceView = rootVC.view;
            activityVC.popoverPresentationController.sourceRect = CGRectMake(rootVC.view.bounds.size.width / 2, rootVC.view.bounds.size.height / 2, 0, 0);
            activityVC.popoverPresentationController.permittedArrowDirections = 0;
        }
        [rootVC presentViewController:activityVC animated:YES completion:nil];
    });
}

void ExecuteIl2CppDump() {
    @try {
        if (!Globals.m_GameFramework) {
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"Oyun modülü (Globals.m_GameFramework) yüklenmedi!");
            });
            return;
        }

        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths firstObject];
        NSString *filePath = [documentsDirectory stringByAppendingPathComponent:@"AsasecDump.cs"];
        
        std::ofstream dumpFile([filePath UTF8String], std::ios::out | std::ios::trunc);
        if (!dumpFile.is_open()) {
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Kayıt Hatası", [NSString stringWithFormat:@"Documents dizinine yazılamadı:\n%@", filePath]);
            });
            return;
        }

        dumpFile << "=== @asasec IL2CPP Class, Method & Offset Dumped ===\n\n";

        DomainGet_t f_DomainGet = (DomainGet_t)Functions.m_DomainGet;
        DomainGetAssemblies_t f_DomainGetAssemblies = (DomainGetAssemblies_t)Functions.m_DomainGetAssemblies;
        AssembliesGetImage_t f_AssembliesGetImage = (AssembliesGetImage_t)Functions.m_AssembliesGetImage;
        ImageGetName_t f_ImageGetName = (ImageGetName_t)Functions.m_ImageGetName;
        ImageGetClassCount_t f_ImageGetClassCount = (ImageGetClassCount_t)Functions.m_ImageGetClassCount;
        ImageGetClass_t f_ImageGetClass = (ImageGetClass_t)Functions.m_ImageGetClass;
        ClassGetMethods_t f_ClassGetMethods = (ClassGetMethods_t)Functions.m_ClassGetMethods;

        il2cpp_class_get_name_t f_ClassName = (il2cpp_class_get_name_t)dlsym(Globals.m_GameFramework, "il2cpp_class_get_name");
        il2cpp_class_get_namespace_t f_ClassNamespace = (il2cpp_class_get_namespace_t)dlsym(Globals.m_GameFramework, "il2cpp_class_get_namespace");
        il2cpp_method_get_name_t f_MethodName = (il2cpp_method_get_name_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_name");
        il2cpp_type_get_name_t f_TypeName = (il2cpp_type_get_name_t)dlsym(Globals.m_GameFramework, "il2cpp_type_get_name");
        il2cpp_method_get_return_type_t f_MethodReturnType = (il2cpp_method_get_return_type_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_return_type");
        il2cpp_method_get_param_count_t f_MethodParamCount = (il2cpp_method_get_param_count_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_param_count");
        il2cpp_method_get_param_name_t f_MethodParamName = (il2cpp_method_get_param_name_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_param_name");
        il2cpp_method_get_param_t f_MethodParam = (il2cpp_method_get_param_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_param");

        if (!f_DomainGet || !f_DomainGetAssemblies) {
            dumpFile.close();
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"IL2CPP temel fonksiyonları çözülemedi.");
            });
            return;
        }

        void* domain = f_DomainGet();
        size_t size = 0;
        void** assemblies = f_DomainGetAssemblies(domain, &size);
        if (!assemblies || size == 0) {
            dumpFile.close();
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"Assembly listesi boş veya alınamadı!");
            });
            return;
        }

        int totalClasses = 0;
        int totalMethods = 0;
        
        uint64_t baseAddress = 0;
        for (uint32_t i = 0; i < _dyld_image_count(); i++) {
            const char *imageName = _dyld_get_image_name(i);
            if (imageName && strstr(imageName, "UnityFramework")) {
                baseAddress = (uint64_t)_dyld_get_image_header(i);
                break;
            }
        }
        if (baseAddress == 0) {
            baseAddress = (uint64_t)Globals.m_GameFramework;
        }

        for (size_t i = 0; i < size; ++i) {
            void* assembly = assemblies[i];
            if (!assembly) continue;

            void* image = f_AssembliesGetImage ? f_AssembliesGetImage(assembly) : nullptr;
            if (!image) continue;

            const char* imageName = f_ImageGetName ? f_ImageGetName(image) : "Unknown";
            int classCount = f_ImageGetClassCount ? f_ImageGetClassCount(image) : 0;
            totalClasses += classCount;

            dumpFile << "// Image: " << (imageName ? imageName : "Unknown") << "\n";

            for (int j = 0; j < classCount; ++j) {
                void* klass = f_ImageGetClass ? f_ImageGetClass(image, j) : nullptr;
                if (!klass) continue;
                
                const char* className = f_ClassName ? f_ClassName(klass) : "Unknown";
                const char* classNamespace = f_ClassNamespace ? f_ClassNamespace(klass) : "";
                
                dumpFile << "\npublic class " << (classNamespace && classNamespace[0] ? classNamespace : "") 
                         << (classNamespace && classNamespace[0] ? "." : "") << (className ? className : "Unknown") 
                         << " // TypeDefIndex: " << j << "\n{\n";

                void* iter = nullptr;
                while (void* method = f_ClassGetMethods ? f_ClassGetMethods(klass, &iter) : nullptr) {
                    if (!method) continue;
                    totalMethods++;

                    const char* methodName = f_MethodName ? f_MethodName(method) : "Unknown";
                    
                    struct MethodInfo_Internal {
                        void* methodPointer;
                        void* invoker_pointer;
                        const char* name;
                        void* klass;
                        void* return_type;
                        void* parameters;
                    };
                    
                    uint64_t rawPointer = (uint64_t)((MethodInfo_Internal*)method)->methodPointer;
                    uint64_t methodPointer = rawPointer & ~((uint64_t)1);
                    
                    uint64_t relativeOffset = 0;
                    if (methodPointer >= baseAddress) {
                        relativeOffset = methodPointer - baseAddress;
                    }

                    const char* returnTypeName = "void";
                    if (f_MethodReturnType && f_TypeName) {
                        void* retTypeObj = f_MethodReturnType(method);
                        if (retTypeObj) {
                            const char* tName = f_TypeName(retTypeObj);
                            if (tName) returnTypeName = tName;
                        }
                    }

                    std::string paramList = "";
                    if (f_MethodParamCount && f_MethodParam && f_TypeName && f_MethodParamName) {
                        uint32_t paramCount = f_MethodParamCount(method);
                        for (uint32_t p = 0; p < paramCount; ++p) {
                            void* paramType = f_MethodParam(method, p);
                            const char* pTypeName = paramType ? f_TypeName(paramType) : "object";
                            const char* pName = f_MethodParamName(method, p);
                            
                            if (p > 0) paramList += ", ";
                            paramList += std::string(pTypeName ? pTypeName : "object") + " " + std::string(pName ? pName : "p" + std::to_string(p));
                        }
                    }

                    dumpFile << "    // RVA: 0x" << std::hex << relativeOffset 
                             << " Offset: 0x" << relativeOffset 
                             << " VA: 0x" << methodPointer << std::dec << "\n";
                    dumpFile << "    public " << returnTypeName << " " << (methodName ? methodName : "Unknown") << "(" << paramList << ") { }\n\n";
                }
                
                dumpFile << "}\n\n";
            }
        }

        dumpFile.flush();
        dumpFile.close();

        dispatch_async(dispatch_get_main_queue(), ^{
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"@asasec-mod"
                                                                            message:[NSString stringWithFormat:@"Dump Başarılı\nSınıf: %d | Metot: %d\nDosya Documents dizinine oluşturuldu.", totalClasses, totalMethods]
                                                                     preferredStyle:UIAlertControllerStyleAlert];
            
            [alert addAction:[UIAlertAction actionWithTitle:@"💾" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                shareDumpFile(filePath);
            }]];
            
            [alert addAction:[UIAlertAction actionWithTitle:@"❌" style:UIAlertActionStyleCancel handler:nil]];
            
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
            [window.rootViewController presentViewController:alert animated:YES completion:nil];
        });
    }
    @catch (NSException *exception) {
        dispatch_async(dispatch_get_main_queue(), ^{
            showNativeAlert(@"Olmadı / Hata", [NSString stringWithFormat:@"Exception: %@", exception.reason]);
        });
    }
}

// Belirtilen string ifadesine göre filtreleme yaparak tam uyumlu saf offset üreten fonksiyon
void ExecuteIl2CppStringDump(NSString *searchString) {
    @try {
        if (!Globals.m_GameFramework) {
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"Oyun modülü (Globals.m_GameFramework) yüklenmedi!");
            });
            return;
        }

        if (!searchString || searchString.length == 0) {
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"Lütfen aranacak bir kelime girin!");
            });
            return;
        }

        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths firstObject];
        NSString *fileName = [NSString stringWithFormat:@"AsasecDump-%@.cs", searchString];
        NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];
        
        std::ofstream dumpFile([filePath UTF8String], std::ios::out | std::ios::trunc);
        if (!dumpFile.is_open()) {
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Kayıt Hatası", [NSString stringWithFormat:@"Documents dizinine yazılamadı:\n%@", filePath]);
            });
            return;
        }

        dumpFile << "=== @asasec IL2CPP Filtered String Dump: " << [searchString UTF8String] << " ===\n\n";

        DomainGet_t f_DomainGet = (DomainGet_t)Functions.m_DomainGet;
        DomainGetAssemblies_t f_DomainGetAssemblies = (DomainGetAssemblies_t)Functions.m_DomainGetAssemblies;
        AssembliesGetImage_t f_AssembliesGetImage = (AssembliesGetImage_t)Functions.m_AssembliesGetImage;
        ImageGetName_t f_ImageGetName = (ImageGetName_t)Functions.m_ImageGetName;
        ImageGetClassCount_t f_ImageGetClassCount = (ImageGetClassCount_t)Functions.m_ImageGetClassCount;
        ImageGetClass_t f_ImageGetClass = (ImageGetClass_t)Functions.m_ImageGetClass;
        ClassGetMethods_t f_ClassGetMethods = (ClassGetMethods_t)Functions.m_ClassGetMethods;

        il2cpp_class_get_name_t f_ClassName = (il2cpp_class_get_name_t)dlsym(Globals.m_GameFramework, "il2cpp_class_get_name");
        il2cpp_class_get_namespace_t f_ClassNamespace = (il2cpp_class_get_namespace_t)dlsym(Globals.m_GameFramework, "il2cpp_class_get_namespace");
        il2cpp_method_get_name_t f_MethodName = (il2cpp_method_get_name_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_name");
        il2cpp_type_get_name_t f_TypeName = (il2cpp_type_get_name_t)dlsym(Globals.m_GameFramework, "il2cpp_type_get_name");
        il2cpp_method_get_return_type_t f_MethodReturnType = (il2cpp_method_get_return_type_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_return_type");
        il2cpp_method_get_param_count_t f_MethodParamCount = (il2cpp_method_get_param_count_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_param_count");
        il2cpp_method_get_param_name_t f_MethodParamName = (il2cpp_method_get_param_name_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_param_name");
        il2cpp_method_get_param_t f_MethodParam = (il2cpp_method_get_param_t)dlsym(Globals.m_GameFramework, "il2cpp_method_get_param");

        if (!f_DomainGet || !f_DomainGetAssemblies) {
            dumpFile.close();
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"IL2CPP temel fonksiyonları çözülemedi.");
            });
            return;
        }

        void* domain = f_DomainGet();
        size_t size = 0;
        void** assemblies = f_DomainGetAssemblies(domain, &size);
        if (!assemblies || size == 0) {
            dumpFile.close();
            dispatch_async(dispatch_get_main_queue(), ^{
                showNativeAlert(@"Olmadı / Hata", @"Assembly listesi boş veya alınamadı!");
            });
            return;
        }

        int matchedClasses = 0;
        int matchedMethods = 0;
        
        uint64_t baseAddress = 0;
        for (uint32_t i = 0; i < _dyld_image_count(); i++) {
            const char *imageName = _dyld_get_image_name(i);
            if (imageName && strstr(imageName, "UnityFramework")) {
                baseAddress = (uint64_t)_dyld_get_image_header(i);
                break;
            }
        }
        if (baseAddress == 0) {
            baseAddress = (uint64_t)Globals.m_GameFramework;
        }

        std::string targetStr = [[searchString lowercaseString] UTF8String];

        for (size_t i = 0; i < size; ++i) {
            void* assembly = assemblies[i];
            if (!assembly) continue;

            void* image = f_AssembliesGetImage ? f_AssembliesGetImage(assembly) : nullptr;
            if (!image) continue;

            const char* imageName = f_ImageGetName ? f_ImageGetName(image) : "Unknown";
            int classCount = f_ImageGetClassCount ? f_ImageGetClassCount(image) : 0;

            for (int j = 0; j < classCount; ++j) {
                void* klass = f_ImageGetClass ? f_ImageGetClass(image, j) : nullptr;
                if (!klass) continue;
                
                const char* className = f_ClassName ? f_ClassName(klass) : "Unknown";
                const char* classNamespace = f_ClassNamespace ? f_ClassNamespace(klass) : "";
                
                std::string fullClassName = std::string(classNamespace && classNamespace[0] ? classNamespace : "") + 
                                            (classNamespace && classNamespace[0] ? "." : "") + 
                                            std::string(className ? className : "Unknown");

                std::string lowerClassName = fullClassName;
                std::transform(lowerClassName.begin(), lowerClassName.end(), lowerClassName.begin(), ::tolower);
                bool classMatches = (lowerClassName.find(targetStr) != std::string::npos);

                std::string classBuffer = "";
                classBuffer += "\npublic class " + fullClassName + " // TypeDefIndex: " + std::to_string(j) + "\n{\n";

                void* iter = nullptr;
                int methodMatchCount = 0;

                while (void* method = f_ClassGetMethods ? f_ClassGetMethods(klass, &iter) : nullptr) {
                    if (!method) continue;

                    const char* methodName = f_MethodName ? f_MethodName(method) : "Unknown";
                    std::string mNameStr(methodName ? methodName : "Unknown");
                    std::string lowerMethodName = mNameStr;
                    std::transform(lowerMethodName.begin(), lowerMethodName.end(), lowerMethodName.begin(), ::tolower);

                    if (classMatches || lowerMethodName.find(targetStr) != std::string::npos) {
                        matchedMethods++;
                        methodMatchCount++;

                        struct MethodInfo_Internal {
                            void* methodPointer;
                            void* invoker_pointer;
                            const char* name;
                            void* klass;
                            void* return_type;
                            void* parameters;
                        };
                        
                        uint64_t rawPointer = (uint64_t)((MethodInfo_Internal*)method)->methodPointer;
                        uint64_t methodPointer = rawPointer & ~((uint64_t)1);
                        
                        uint64_t relativeOffset = 0;
                        if (methodPointer >= baseAddress) {
                            relativeOffset = methodPointer - baseAddress;
                        }

                        const char* returnTypeName = "void";
                        if (f_MethodReturnType && f_TypeName) {
                            void* retTypeObj = f_MethodReturnType(method);
                            if (retTypeObj) {
                                const char* tName = f_TypeName(retTypeObj);
                                if (tName) returnTypeName = tName;
                            }
                        }

                        std::string paramList = "";
                        if (f_MethodParamCount && f_MethodParam && f_TypeName && f_MethodParamName) {
                            uint32_t paramCount = f_MethodParamCount(method);
                            for (uint32_t p = 0; p < paramCount; ++p) {
                                void* paramType = f_MethodParam(method, p);
                                const char* pTypeName = paramType ? f_TypeName(paramType) : "object";
                                const char* pName = f_MethodParamName(method, p);
                                
                                if (p > 0) paramList += ", ";
                                paramList += std::string(pTypeName ? pTypeName : "object") + " " + std::string(pName ? pName : "p" + std::to_string(p));
                            }
                        }

                        char offsetBuffer[256];
                        sprintf(offsetBuffer, "    // RVA: 0x%llx Offset: 0x%llx VA: 0x%llx\n", relativeOffset, relativeOffset, methodPointer);
                        classBuffer += offsetBuffer;
                        classBuffer += "    public " + std::string(returnTypeName) + " " + mNameStr + "(" + paramList + ") { }\n\n";
                    }
                }

                classBuffer += "}\n\n";

                if (classMatches || methodMatchCount > 0) {
                    matchedClasses++;
                    dumpFile << "// Image: " << (imageName ? imageName : "Unknown") << "\n";
                    dumpFile << classBuffer;
                }
            }
        }

        dumpFile.flush();
        dumpFile.close();

        dispatch_async(dispatch_get_main_queue(), ^{
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"@asasec-mod"
                                                                            message:[NSString stringWithFormat:@"String Dump Başarılı\nBulunan Sınıf: %d | Metot: %d\nDosya: %@", matchedClasses, matchedMethods, fileName]
                                                                     preferredStyle:UIAlertControllerStyleAlert];
            
            [alert addAction:[UIAlertAction actionWithTitle:@"💾" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                shareDumpFile(filePath);
            }]];
            
            [alert addAction:[UIAlertAction actionWithTitle:@"❌" style:UIAlertActionStyleCancel handler:nil]];
            
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
            UIViewController *rootVC = window ? window.rootViewController : [UIApplication sharedApplication].windows.firstObject.rootViewController;
            [rootVC presentViewController:alert animated:YES completion:nil];
        });
    }
    @catch (NSException *exception) {
        dispatch_async(dispatch_get_main_queue(), ^{
            showNativeAlert(@"Olmadı / Hata", [NSString stringWithFormat:@"Exception: %@", exception.reason]);
        });
    }
}
