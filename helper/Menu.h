#import <UIKit/UIKit.h>
#import <thread>

// Dump fonksiyonunun prototipi (Dump.mm içerisinden çağrılacak)
void ExecuteIl2CppDump(void);
void showNativeAlert(NSString *title, NSString *message);

@interface ImGuiStyleMenuView : UIView
@property (nonatomic, strong) UIView *mobileMenuWindow;
@property (nonatomic, strong) UIButton *floatingIcon;
@property (nonatomic, strong) UIButton *dumpButton;
@end

@implementation ImGuiStyleMenuView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        
        self.mobileMenuWindow = [[UIView alloc] initWithFrame:CGRectMake(50, 80, 270, 110)];
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

        self.dumpButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.dumpButton.frame = CGRectMake(18, 56, 234, 38);
        self.dumpButton.backgroundColor = [UIColor colorWithRed:0.20 green:0.60 blue:1.00 alpha:1.0];
        [self.dumpButton setTitle:@"🚀" forState:UIControlStateNormal];
        [self.dumpButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.dumpButton.titleLabel.font = [UIFont boldSystemFontOfSize:14];
        self.dumpButton.layer.cornerRadius = 8.0;
        [self.dumpButton addTarget:self action:@selector(dumpButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
        [self.mobileMenuWindow addSubview:self.dumpButton];

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
    if (hitView == self) {
        return nil;
    }
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

@end
