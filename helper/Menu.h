#import <UIKit/UIKit.h>
#import <thread>
#import "AssemblyViewerWindow.h"

void ExecuteIl2CppDump(void);
void ExecuteIl2CppStringDump(NSString *searchString);
void showNativeAlert(NSString *title, NSString *message);

@interface NativeMenuView : UIView <UITextFieldDelegate>
@property (nonatomic, strong) UIView *mobileMenuWindow;
@property (nonatomic, strong) UIButton *floatingIcon;
@property (nonatomic, strong) UIButton *dumpButton;
@property (nonatomic, strong) UIButton *stringDumpButton;
@property (nonatomic, strong) UIButton *assemblyViewerButton;

// Ayrı bir String Dump alt penceresi için view ve elemanlar
@property (nonatomic, strong) UIView *stringPopupView;
@property (nonatomic, strong) UITextField *searchTextField;
@property (nonatomic, strong) UIButton *confirmStringDumpButton;
@property (nonatomic, strong) UIButton *cancelStringDumpButton;
@end

@implementation NativeMenuView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        
        // Ana menü boyutu (Başlangıçta kompakt: Full Dump, String Dump, Assembly Viewer)
        self.mobileMenuWindow = [[UIView alloc] initWithFrame:CGRectMake(50, 80, 270, 160)];
        self.mobileMenuWindow.backgroundColor = [UIColor colorWithRed:0.08 green:0.08 blue:0.10 alpha:0.97];
        self.mobileMenuWindow.layer.cornerRadius = 16.0;
        self.mobileMenuWindow.layer.borderWidth = 1.5;
        self.mobileMenuWindow.layer.borderColor = [UIColor colorWithRed:0.30 green:0.60 blue:1.00 alpha:1.0].CGColor;
        self.mobileMenuWindow.layer.shadowColor = [UIColor blackColor].CGColor;
        self.mobileMenuWindow.layer.shadowOffset = CGSizeMake(0, 8);
        self.mobileMenuWindow.layer.shadowOpacity = 0.5;
        self.mobileMenuWindow.layer.shadowRadius = 10.0;
        self.mobileMenuWindow.clipsToBounds = NO;
        self.mobileMenuWindow.hidden = YES;
        [self addSubview:self.mobileMenuWindow];

        UIPanGestureRecognizer *menuPan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleMenuPan:)];
        [self.mobileMenuWindow addGestureRecognizer:menuPan];

        UIView *titleBar = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 270, 40)];
        titleBar.backgroundColor = [UIColor colorWithRed:0.14 green:0.17 blue:0.22 alpha:1.0];
        
        UIBezierPath *maskPath = [UIBezierPath bezierPathWithRoundedRect:titleBar.bounds byRoundingCorners:UIRectCornerTopLeft | UIRectCornerTopRight cornerRadii:CGSizeMake(16.0, 16.0)];
        CAShapeLayer *maskLayer = [CAShapeLayer layer];
        maskLayer.path = maskPath.CGPath;
        titleBar.layer.mask = maskLayer;
        [self.mobileMenuWindow addSubview:titleBar];

        UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(16, 0, 200, 40)];
        titleLabel.text = @"⭐ @asasec IL2CPP Dumper";
        titleLabel.textColor = [UIColor whiteColor];
        titleLabel.font = [UIFont boldSystemFontOfSize:13];
        [titleBar addSubview:titleLabel];

        UIButton *closeBtn = [UIButton buttonWithType:UIButtonTypeSystem];
        closeBtn.frame = CGRectMake(230, 8, 24, 24);
        [closeBtn setTitle:@"✕" forState:UIControlStateNormal];
        [closeBtn setTitleColor:[UIColor colorWithRed:1.0 green:0.35 blue:0.35 alpha:1.0] forState:UIControlStateNormal];
        closeBtn.titleLabel.font = [UIFont boldSystemFontOfSize:14];
        [closeBtn addTarget:self action:@selector(minimizeMenu) forControlEvents:UIControlEventTouchUpInside];
        [titleBar addSubview:closeBtn];

        // 1. Full Dump Butonu
        self.dumpButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.dumpButton.frame = CGRectMake(18, 48, 234, 32);
        self.dumpButton.backgroundColor = [UIColor colorWithRed:0.20 green:0.60 blue:1.00 alpha:1.0];
        [self.dumpButton setTitle:@"▶ Full Dump" forState:UIControlStateNormal];
        [self.dumpButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.dumpButton.titleLabel.font = [UIFont boldSystemFontOfSize:13];
        self.dumpButton.layer.cornerRadius = 6.0;
        [self.dumpButton addTarget:self action:@selector(dumpButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
        [self.mobileMenuWindow addSubview:self.dumpButton];

        // 2. String Dump Butonu (Tıklanınca alt kısımda input alanı açılacak)
        self.stringDumpButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.stringDumpButton.frame = CGRectMake(18, 86, 234, 32);
        self.stringDumpButton.backgroundColor = [UIColor colorWithRed:0.80 green:0.40 blue:0.10 alpha:1.0];
        [self.stringDumpButton setTitle:@"🔍 String Dump" forState:UIControlStateNormal];
        [self.stringDumpButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.stringDumpButton.titleLabel.font = [UIFont boldSystemFontOfSize:13];
        self.stringDumpButton.layer.cornerRadius = 6.0;
        [self.stringDumpButton addTarget:self action:@selector(stringDumpButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
        [self.mobileMenuWindow addSubview:self.stringDumpButton];

        // 3. Assembly Viewer Butonu
        self.assemblyViewerButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.assemblyViewerButton.frame = CGRectMake(18, 124, 234, 32);
        self.assemblyViewerButton.backgroundColor = [UIColor colorWithRed:0.30 green:0.50 blue:0.30 alpha:1.0];
        [self.assemblyViewerButton setTitle:@"⚙️ Assembly Viewer" forState:UIControlStateNormal];
        [self.assemblyViewerButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.assemblyViewerButton.titleLabel.font = [UIFont boldSystemFontOfSize:13];
        self.assemblyViewerButton.layer.cornerRadius = 6.0;
        [self.assemblyViewerButton addTarget:self action:@selector(assemblyViewerButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
        [self.mobileMenuWindow addSubview:self.assemblyViewerButton];

        // --- String Dump Tıklanınca Açılacak Alt Panel (Gizli Başlar) ---
        self.stringPopupView = [[UIView alloc] initWithFrame:CGRectMake(18, 86, 234, 75)];
        self.stringPopupView.backgroundColor = [UIColor colorWithRed:0.12 green:0.12 blue:0.15 alpha:1.0];
        self.stringPopupView.layer.cornerRadius = 8.0;
        self.stringPopupView.hidden = YES;
        [self.mobileMenuWindow addSubview:self.stringPopupView];

        self.searchTextField = [[UITextField alloc] initWithFrame:CGRectMake(8, 8, 218, 26)];
        self.searchTextField.backgroundColor = [UIColor colorWithRed:0.18 green:0.18 blue:0.22 alpha:1.0];
        self.searchTextField.textColor = [UIColor whiteColor];
        self.searchTextField.font = [UIFont systemFontOfSize:11];
        self.searchTextField.layer.cornerRadius = 4.0;
        self.searchTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 6, 26)];
        self.searchTextField.leftViewMode = UITextFieldViewModeAlways;
        self.searchTextField.attributedPlaceholder = [[NSAttributedString alloc] initWithString:@"Kelime girin (örn: coin)" attributes:@{NSForegroundColorAttributeName: [UIColor colorWithRed:0.6 green:0.6 blue:0.6 alpha:1.0]}];
        self.searchTextField.delegate = self;
        [self.stringPopupView addSubview:self.searchTextField];

        self.confirmStringDumpButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.confirmStringDumpButton.frame = CGRectMake(8, 40, 105, 26);
        self.confirmStringDumpButton.backgroundColor = [UIColor colorWithRed:0.20 green:0.60 blue:0.20 alpha:1.0];
        [self.confirmStringDumpButton setTitle:@"Dump Et" forState:UIControlStateNormal];
        [self.confirmStringDumpButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.confirmStringDumpButton.titleLabel.font = [UIFont boldSystemFontOfSize:11];
        self.confirmStringDumpButton.layer.cornerRadius = 4.0;
        [self.confirmStringDumpButton addTarget:self action:@selector(confirmStringDumpTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.stringPopupView addSubview:self.confirmStringDumpButton];

        self.cancelStringDumpButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.cancelStringDumpButton.frame = CGRectMake(121, 40, 105, 26);
        self.cancelStringDumpButton.backgroundColor = [UIColor colorWithRed:0.60 green:0.20 blue:0.20 alpha:1.0];
        [self.cancelStringDumpButton setTitle:@"İptal" forState:UIControlStateNormal];
        [self.cancelStringDumpButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.cancelStringDumpButton.titleLabel.font = [UIFont boldSystemFontOfSize:11];
        self.cancelStringDumpButton.layer.cornerRadius = 4.0;
        [self.cancelStringDumpButton addTarget:self action:@selector(cancelStringDumpTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.stringPopupView addSubview:self.cancelStringDumpButton];
        // -------------------------------------------------------------

        // Yüzen Simge (Floating Icon)
        self.floatingIcon = [UIButton buttonWithType:UIButtonTypeSystem];
        self.floatingIcon.frame = CGRectMake(40, 100, 54, 54);
        self.floatingIcon.backgroundColor = [UIColor colorWithRed:0.10 green:0.10 blue:0.13 alpha:0.92];
        [self.floatingIcon setTitle:@"📦" forState:UIControlStateNormal];
        self.floatingIcon.titleLabel.font = [UIFont systemFontOfSize:28];
        self.floatingIcon.layer.cornerRadius = 27.0;
        self.floatingIcon.layer.borderWidth = 2.0;
        self.floatingIcon.layer.borderColor = [UIColor colorWithRed:0.30 green:0.60 blue:1.00 alpha:1.0].CGColor;
        self.floatingIcon.layer.shadowColor = [UIColor blackColor].CGColor;
        self.floatingIcon.layer.shadowOffset = CGSizeMake(0, 4);
        self.floatingIcon.layer.shadowOpacity = 0.6;
        self.floatingIcon.layer.shadowRadius = 8.0;
        self.floatingIcon.hidden = NO;
        [self.floatingIcon addTarget:self action:@selector(restoreMenu) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.floatingIcon];

        UIPanGestureRecognizer *iconPan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleIconPan:)];
        [self.floatingIcon addGestureRecognizer:iconPan];
    }
    return self;
}

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    UIView *hitView = [super hitTest:point withEvent:event];
    if (hitView == self) return nil;
    return hitView;
}

- (void)handleMenuPan:(UIPanGestureRecognizer *)gesture {
    CGPoint translation = [gesture translationInView:self];
    CGPoint center = gesture.view.center;
    gesture.view.center = CGPointMake(center.x + translation.x, center.y + translation.y);
    [gesture setTranslation:CGPointZero inView:self];
}

- (void)handleIconPan:(UIPanGestureRecognizer *)gesture {
    CGPoint translation = [gesture translationInView:self];
    CGPoint center = gesture.view.center;
    gesture.view.center = CGPointMake(center.x + translation.x, center.y + translation.y);
    [gesture setTranslation:CGPointZero inView:self];
}

- (void)minimizeMenu {
    self.mobileMenuWindow.hidden = YES;
    self.floatingIcon.center = self.mobileMenuWindow.center;
    self.floatingIcon.hidden = NO;
}

- (void)restoreMenu {
    self.floatingIcon.hidden = YES;
    self.mobileMenuWindow.center = self.floatingIcon.center;
    self.mobileMenuWindow.hidden = NO;
}

- (void)dumpButtonTapped:(UIButton *)sender {
    std::thread(ExecuteIl2CppDump).detach();
}

- (void)stringDumpButtonTapped:(UIButton *)sender {
    // Butona basıldığında arama panelini göster, diğer butonları gizle/ört
    self.stringPopupView.hidden = NO;
    self.searchTextField.text = @"";
    [self.searchTextField becomeFirstResponder]; // Klavyeyi otomatik aç
}

- (void)confirmStringDumpTapped {
    [self.searchTextField resignFirstResponder];
    self.stringPopupView.hidden = YES;
    
    NSString *query = self.searchTextField.text;
    std::thread([query]() {
        ExecuteIl2CppStringDump(query);
    }).detach();
}

- (void)cancelStringDumpTapped {
    [self.searchTextField resignFirstResponder];
    self.stringPopupView.hidden = YES;
}

- (void)assemblyViewerButtonTapped:(UIButton *)sender {
    UIWindow *keyWindow = [UIApplication sharedApplication].windows.firstObject;
    AssemblyViewerWindow *asmWindow = [[AssemblyViewerWindow alloc] initWithFrame:keyWindow.bounds];
    [keyWindow addSubview:asmWindow];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

@end
