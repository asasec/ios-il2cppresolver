#import <UIKit/UIKit.h>
#import <mach-o/dyld.h>

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
        self.addressTextField.attributedPlaceholder = [[NSAttributedString alloc] initWithString:@"Offset (örn: 0x295fe50)" attributes:@{NSForegroundColorAttributeName: [UIColor colorWithRed:0.6 green:0.6 blue:0.6 alpha:1.0]}];
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
    NSString *text = [self.addressTextField.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    uint64_t inputAddr = strtoull([text UTF8String], nullptr, 0);
    
    if (inputAddr == 0) {
        self.outputTextView.text = @"Geçersiz veya boş adres girdiniz!";
        return;
    }
    
    // UnityFramework base adresini bulalım
    uint64_t baseAddress = 0;
    for (uint32_t i = 0; i < _dyld_image_count(); i++) {
        const char *imageName = _dyld_get_image_name(i);
        if (imageName && strstr(imageName, "UnityFramework")) {
            baseAddress = (uint64_t)_dyld_get_image_header(i);
            break;
        }
    }
    
    // Eğer girilen değer base adresten küçükse, bu bir saf offset/RVA'dır; base adres ile toplayalım.
    uint64_t targetAddr = inputAddr;
    if (inputAddr < baseAddress && baseAddress != 0) {
        targetAddr = baseAddress + inputAddr;
    }
    
    NSMutableString *result = [NSMutableString string];
    [result appendFormat:@"Base: 0x%llx\nTarget: 0x%llx\n\n", baseAddress, targetAddr];
    
    uint32_t *ptr = (uint32_t *)targetAddr;
    
    @try {
        for (int int_i = 0; int_i < 15; int_i++) {
            uint64_t currentAddress = targetAddr + (int_i * 4);
            uint32_t op = ptr[int_i];
            [result appendFormat:@"0x%llx:  0x%08X\n", currentAddress, op];
        }
    } @catch (NSException *exception) {
        [result appendFormat:@"\nHata: Bellek okunamadı (%@)", exception.reason];
    }
    
    self.outputTextView.text = result;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

@end
