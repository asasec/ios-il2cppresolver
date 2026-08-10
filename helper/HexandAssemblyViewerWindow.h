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
        self.containerView = [[UIView alloc] initWithFrame:CGRectMake(20, 30, 335, 410)];
        self.containerView.backgroundColor = [UIColor colorWithRed:0.08 green:0.08 blue:0.10 alpha:0.98];
        self.containerView.layer.cornerRadius = 16.0;
        self.containerView.layer.borderWidth = 1.5;
        self.containerView.layer.borderColor = [UIColor colorWithRed:0.30 green:0.60 blue:1.00 alpha:1.0].CGColor;
        [self addSubview:self.containerView];

        // Başlık
        UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(16, 10, 230, 30)];
        titleLabel.text = @"🔍 Hex & Assembly Viewer";
        titleLabel.textColor = [UIColor whiteColor];
        titleLabel.font = [UIFont boldSystemFontOfSize:14];
        [self.containerView addSubview:titleLabel];

        // Kapat Butonu
        self.closeButton = [UIButton buttonWithType:UIButtonTypeSystem];
        self.closeButton.frame = CGRectMake(295, 10, 30, 30);
        [self.closeButton setTitle:@"✕" forState:UIControlStateNormal];
        [self.closeButton setTitleColor:[UIColor colorWithRed:1.0 green:0.35 blue:0.35 alpha:1.0] forState:UIControlStateNormal];
        self.closeButton.titleLabel.font = [UIFont boldSystemFontOfSize:14];
        [self.closeButton addTarget:self action:@selector(closeTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.containerView addSubview:self.closeButton];

        // Adres Giriş Kutusu
        self.addressTextField = [[UITextField alloc] initWithFrame:CGRectMake(16, 50, 225, 32)];
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
        self.showButton.frame = CGRectMake(249, 50, 70, 32);
        self.showButton.backgroundColor = [UIColor colorWithRed:0.20 green:0.60 blue:1.00 alpha:1.0];
        [self.showButton setTitle:@"Göster" forState:UIControlStateNormal];
        [self.showButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        self.showButton.titleLabel.font = [UIFont boldSystemFontOfSize:12];
        self.showButton.layer.cornerRadius = 6.0;
        [self.showButton addTarget:self action:@selector(showTapped) forControlEvents:UIControlEventTouchUpInside];
        [self.containerView addSubview:self.showButton];

        // Çıktı Alanı (TextView)
        self.outputTextView = [[UITextView alloc] initWithFrame:CGRectMake(16, 92, 303, 300)];
        self.outputTextView.backgroundColor = [UIColor colorWithRed:0.12 green:0.12 blue:0.15 alpha:1.0];
        self.outputTextView.textColor = [UIColor colorWithRed:0.0 green:0.8 blue:1.0 alpha:1.0];
        self.outputTextView.font = [UIFont fontWithName:@"Courier" size:10] ?: [UIFont systemFontOfSize:10];
        self.outputTextView.editable = NO;
        self.outputTextView.layer.cornerRadius = 6.0;
        [self.containerView addSubview:self.outputTextView];
    }
    return self;
}

- (void)closeTapped {
    [self removeFromSuperview];
}

- (NSString *)disassembleARM64Instruction:(uint32_t)insn address:(uint64_t)addr {
    if (insn == 0xD65F03C0) return @"ret";
    if (insn == 0xD503201F) return @"nop";
    
    if ((insn & 0xFFFFFC1F) == 0xD65F0000) return @"ret";

    if ((insn & 0xFC000000) == 0x14000000) {
        int32_t imm26 = (insn & 0x03FFFFFF);
        if (imm26 & 0x02000000) imm26 |= 0xFC000000;
        uint64_t target = addr + (imm26 << 2);
        return [NSString stringWithFormat:@"b\t0x%llx", target];
    }
    if ((insn & 0xFC000000) == 0x94000000) {
        int32_t imm26 = (insn & 0x03FFFFFF);
        if (imm26 & 0x02000000) imm26 |= 0xFC000000;
        uint64_t target = addr + (imm26 << 2);
        return [NSString stringWithFormat:@"bl\t0x%llx", target];
    }

    uint32_t sf = (insn >> 31) & 0x1;
    if ((insn & 0x1F800000) == 0x12800000 || (insn & 0x1F800000) == 0x52800000 || (insn & 0x1F800000) == 0x92800000) {
        uint32_t rd = insn & 0x1F;
        uint32_t imm16 = (insn >> 5) & 0xFFFF;
        NSString *regPrefix = (sf == 1) ? @"x" : @"w";
        return [NSString stringWithFormat:@"mov\t%@%u, #0x%X", regPrefix, rd, imm16];
    }

    return [NSString stringWithFormat:@".long\t0x%08X", insn];
}

- (void)showTapped {
    [self.addressTextField resignFirstResponder];
    NSString *text = [self.addressTextField.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    uint64_t inputAddr = strtoull([text UTF8String], nullptr, 0);
    
    if (inputAddr == 0) {
        self.outputTextView.text = @"Geçersiz veya boş adres girdiniz!";
        return;
    }
    
    uint64_t baseAddress = 0;
    for (uint32_t i = 0; i < _dyld_image_count(); i++) {
        const char *imageName = _dyld_get_image_name(i);
        if (imageName && strstr(imageName, "UnityFramework")) {
            baseAddress = (uint64_t)_dyld_get_image_header(i);
            break;
        }
    }
    
    uint64_t targetAddr = inputAddr;
    if (inputAddr < baseAddress && baseAddress != 0) {
        targetAddr = baseAddress + inputAddr;
    }
    
    uint64_t currentRVA = (targetAddr >= baseAddress) ? (targetAddr - baseAddress) : targetAddr;
    
    NSMutableString *result = [NSMutableString string];
    unsigned char *bytePtr = (unsigned char *)targetAddr;
    uint32_t *intPtr = (uint32_t *)targetAddr;
    
    @try {
        [result appendString:@"--- HEX DUMP ---\n"];
        int hexRows = 8;
        int bytesPerRow = 8;
        
        for (int row = 0; row < hexRows; row++) {
            uint64_t rowRVA = currentRVA + (row * bytesPerRow);
            [result appendFormat:@"%08llX  ", rowRVA];
            
            for (int b = 0; b < bytesPerRow; b++) {
                unsigned char val = bytePtr[(row * bytesPerRow) + b];
                [result appendFormat:@"%02X ", val];
            }
            [result appendString:@"\n"];
        }
        
        [result appendString:@"----------------------------------------\n"];
        
        [result appendString:@"--- ASSEMBLY (ARM64) ---\n"];
        int asmRows = 8;
        
        for (int i = 0; i < asmRows; i++) {
            uint64_t instAddr = targetAddr + (i * 4);
            uint64_t instRVA = currentRVA + (i * 4);
            uint32_t opcVal = intPtr[i];
            
            NSString *disasm = [self disassembleARM64Instruction:opcVal address:instAddr];
            [result appendFormat:@"%08llX:  %@\n", instRVA, disasm];
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
