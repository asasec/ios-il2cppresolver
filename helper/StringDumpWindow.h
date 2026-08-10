#import <UIKit/UIKit.h>
#import <thread>

// Dump fonksiyonunun prototipi
void ExecuteIl2CppStringDump(NSString *searchString);

@interface StringDumpWindow : UIView <UITextFieldDelegate>
@property (nonatomic, strong) UIView *containerView;
@property (nonatomic, strong) UITextField *searchTextField;
@property (nonatomic, strong) UIButton *dumpButton;
@property (nonatomic, strong) UIButton *cancelButton;
@end

@implementation StringDumpWindow

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.4];
        
        // Ana Konteyner Pencere
        self.containerView = [[UIView alloc] initWithFrame:CGRectMake(50, 150, 270, 150)];
        self.containerView.backgroundColor = [UIColor colorWithRed:0.08 green:0.08 blue:0.10 alpha:0.98];
        self.containerView.layer.cornerRadius = 16.0;
        self.containerView.layer.borderWidth = 1.5;
        self.containerView.layer.borderColor = [UIColor colorWithRed:0.80 green:0.40 blue:0.10 alpha:1.0].CGColor;
        [self addSubview:self.containerView];

        // Başlık
        UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(16, 12, 238, 24)];
        titleLabel.text = @"🔍 String Dump Arama";
        titleLabel.textColor = [UIColor whiteColor];
        titleLabel.font = [UIFont boldSystemFontOfSize:13];
        [self.containerView addSubview:titleLabel];

        // Arama Metin Kutusu
        self.searchTextField = [[UITextField alloc] initWithFrame:CGRectMake(16, 46, 238, 32)];
        self.searchTextField.backgroundColor = [UIColor colorWithRed:0.15 green:0.15 blue:0.18 alpha:1.0];
        self.searchTextField.textColor = [UIColor whiteColor];
        self.searchTextField.font = [UIFont systemFontOfSize:12];
        self.searchTextField.layer.cornerRadius = 6.0;
        self.searchTextField.layer.borderWidth = 1.0;
        self.searchTextField.layer.borderColor = [UIColor colorWithRed:0.30 green:0.30 blue:0.35 alpha:1.0].CGColor;
        self.searchTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 8, 32)];
        self.searchTextField.leftViewMode = UITextFieldViewModeAlways;
        self.searchTextField.attributedPlaceholder = [[NSAttributedString alloc] initWithString:@"Kelime girin (örn: coin)" attributes:@{NSForegroundColorAttributeName: [UIColor colorWithRed:0.6 green:0.6 blue:0.6 alpha:1.0]}];
        self.searchTextField.delegate = self;
        [self.containerView addSubview:self.searchTextField];

        // Dump Et Butonu
        self.dumpButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.dumpButton.frame = CGRectMake(16, 92, 112, 34);
        self.dumpButton.backgroundColor = [UIColor colorWithRed:0.80 green:0.40 blue:0.10 alpha:1.0];
        [self.dumpButton setTitle:@"Dump Et" forState:UIControlStateNormal];
        [self.dumpButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.dumpButton.titleLabel.font = [UIFont boldSystemFontOfSize:12];
        self.dumpButton.layer.cornerRadius = 6.0;
        [self.dumpButton addTarget:self action:@selector(dumpTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.containerView addSubview:self.dumpButton];

        // İptal Butonu
        self.cancelButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.cancelButton.frame = CGRectMake(142, 92, 112, 34);
        self.cancelButton.backgroundColor = [UIColor colorWithRed:0.40 green:0.40 blue:0.45 alpha:1.0];
        [self.cancelButton setTitle:@"İptal" forState:UIControlStateNormal];
        [self.cancelButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.cancelButton.titleLabel.font = [UIFont boldSystemFontOfSize:12];
        self.cancelButton.layer.cornerRadius = 6.0;
        [self.cancelButton addTarget:self action:@selector(cancelTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.containerView addSubview:self.cancelButton];
    }
    return self;
}

- (void)cancelTapped {
    [self removeFromSuperview];
}

- (void)dumpTapped {
    [self.searchTextField resignFirstResponder];
    NSString *query = self.searchTextField.text;
    [self removeFromSuperview];
    
    std::thread([query]() {
        ExecuteIl2CppStringDump(query);
    }).detach();
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

@end
