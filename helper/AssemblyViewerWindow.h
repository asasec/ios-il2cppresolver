#import <UIKit/UIKit.h>

@interface AssemblyViewerWindow : UIView <UITextFieldDelegate>
@property (nonatomic, strong) UIView *containerView;
@property (nonatomic, strong) UITextField *addressTextField;
@property (nonatomic, strong) UITextView *outputTextView;
@property (nonatomic, strong) UIButton *showButton;
@property (nonatomic, strong) UIButton *closeButton;
@end

@implementation AssemblyViewerWindow

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.4];
        
        // Ana Konteyner Pencere
        self.containerView = [[UIView alloc] initWithFrame:CGRectMake(30, 60, 320, 340)];
        self.containerView.backgroundColor = [UIColor colorWithRed:0.08 green:0.08 blue:0.10 alpha:0.98];
        self.containerView.layer.cornerRadius = 16.0;
        self.containerView.layer.borderWidth = 1.5;
        self.containerView.layer.borderColor = [UIColor colorWithRed:0.30 green:0.60 blue:1.00 alpha:1.0].CGColor;
        [self addSubview:self.containerView];

        // Başlık
        UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(16, 10, 230, 30)];
        titleLabel.text = @"⚙️ Assembly / Memory Viewer";
        titleLabel.textColor = [UIColor whiteColor];
        titleLabel.font = [UIFont boldSystemFontOfSize:14];
        [self.containerView addSubview:titleLabel];

        // Kapat Butonu
        self.closeButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.closeButton.frame = CGRectMake(280, 10, 30, 30);
        [self.closeButton setTitle:@"✕" forState:UIControlStateNormal];
        [self.closeButton setTitleColor:[UIColor colorWithRed:1.0 green:0.35 blue:0.35 alpha:1.0] forState:UIControlStateNormal];
        self.closeButton.titleLabel.font = [UIFont boldSystemFontOfSize:14];
        [self.closeButton addTarget:self action:@selector(closeTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.containerView addSubview:self.closeButton];

        // Adres Giriş Kutusu
        self.addressTextField = [[UITextField alloc] initWithFrame:CGRectMake(16, 50, 210, 32)];
        self.addressTextField.backgroundColor = [UIColor colorWithRed:0.15 green:0.15 blue:0.18 alpha:1.0];
        self.addressTextField.textColor = [UIColor whiteColor];
        self.addressTextField.font = [UIFont systemFontOfSize:12];
        self.addressTextField.layer.cornerRadius = 6.0;
        self.addressTextField.layer.borderWidth = 1.0;
        self.addressTextField.layer.borderColor = [UIColor colorWithRed:0.30 green:0.30 blue:0.35 alpha:1.0].CGColor;
        self.addressTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 8, 32)];
        self.addressTextField.leftViewMode = UITextFieldViewModeAlways;
        self.addressTextField.attributedPlaceholder = [[NSAttributedString alloc] initWithString:@"Adres (örn: 0x100000000)" attributes:@{NSForegroundColorAttributeName: [UIColor colorWithRed:0.6 green:0.6 blue:0.6 alpha:1.0]}];
        self.addressTextField.delegate = self;
        [self.containerView addSubview:self.addressTextField];

        // Göster Butonu
        self.showButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.showButton.frame = CGRectMake(234, 50, 70, 32);
        self.showButton.backgroundColor = [UIColor colorWithRed:0.20 green:0.60 blue:1.00 alpha:1.0];
        [self.showButton setTitle:@"Göster" forState:UIControlStateNormal];
        [self.showButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.showButton.titleLabel.font = [UIFont boldSystemFontOfSize:12];
        self.showButton.layer.cornerRadius = 6.0;
        [self.showButton addTarget:self action:@selector(showTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.containerView addSubview:self.showButton];

        // Çıktı Alanı (TextView)
        self.outputTextView = [[UITextView alloc] initWithFrame:CGRectMake(16, 92, 288, 230)];
        self.outputTextView.backgroundColor = [UIColor colorWithRed:0.12 green:0.12 blue:0.15 alpha:1.0];
        self.outputTextView.textColor = [UIColor colorWithRed:0.0 green:0.8 blue:1.0 alpha:1.0];
        self.outputTextView.font = [UIFont fontWithName:@"Courier" size:11] ?: [UIFont systemFontOfSize:11];
        self.outputTextView.editable = NO;
        self.outputTextView.layer.cornerRadius = 6.0;
        [self.containerView addSubview:self.outputTextView];
    }
    return self;
}

- (void)closeTapped {
    [self removeFromSuperview];
}

- (void)showTapped {
    [self.addressTextField resignFirstResponder];
    NSString *text = self.addressTextField.text;
    uint64_t addr = strtoull([text UTF8String], nullptr, 0);
    
    if (addr == 0) {
        self.outputTextView.text = @"Geçersiz veya boş adres girdiniz!";
        return;
    }
    
    NSMutableString *result = [NSMutableString string];
    uint32_t *ptr = (uint32_t *)addr;
    
    @try {
        for (int i = 0; i < 15; i++) {
            uint32_t op = ptr[i];
            [result appendFormat:@"0x%llx:  0x%08X\n", (addr + (i * 4)), op];
        }
    } @catch (NSException *exception) {
        [result appendString:@"\nHata: Bellek okunamadı (Segmentation Fault / Korumalı Alan)"];
    }
    
    self.outputTextView.text = result;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

@end
