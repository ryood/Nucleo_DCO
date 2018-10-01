テスト内容

Nucleo_DCO_Test02 u8g2-mbedの単体テスト
Nucleo_DCO_Test03 DDSの単体テスト
Nucleo_DCO_Test04 u8g2-mbedとDDSの結合テスト
Nucleo_DCO_Test05 + 3OSCのMIXテスト
Nucleo_DCO_Test06 + MCP3008(SPI ADC)のテスト 【動作しない】
Nucleo_DCO_Test07 u8g2-mbed + DDS(3OSC) + 内蔵ADCの結合テスト
Nucleo_DCO_Test08 + 各種波形出力
Nucleo_DCO_Test09 + タクトスイッチ(InterruptIn)の入力
Nucleo_DCO_Test10 + OLEDの表示モード
Nucleo_DCO_Test11 + Frequency Range切り替え
Nucleo_DCO_Test12 + OLEDの表示項目を増やす
Nucleo_DCO_Test13 + OLEDをSSD1306 128x64 I2Cに変更
Nucleo_DCO_Test14 + ADC読み取りを補間(InterpolateFloat)
Nucleo_DCO_Test15 + ADC読み取りを7bit(128)精度に DisplayMode増やす OLED表示オフモード
Nucleo_DCO_Test16 + Rotary Encoder
Nucleo_DCO_Test17 + Rotary Encoderを増やす, LED, UART fps
Nucleo_DCO_Test18 + Clean up, パネル取り付けプッシュスイッチ
Nucleo_DCO_Test19 + ADC読み取り抑止
Nucleo_DCO_Test20 + RotaryEncoder Callback（）使用, ADC読み取り、OLED表示をThreadに

ADC_VREF_Test01         ADC_VREFの読み取り
AverageAnalogIN_Test01  AverageAnalogIn(移動平均付きAnalogIn)のテスト
FloatingPoint_Test01    浮動小数点数演算の実行時間を計測: Mbed OS 5
InternalADC_Test01      内蔵ADCの読み取り時間を計測
InterpolateFloat_Test01 InterpolateFloat(float型を補間)のテスト
InterruptIn_Test01      InterruptIn（チャタリング対策）のテスト
InterruptIn_Test02      複数のInterruptIn（チャタリング対策）のテスト: F446RE
InterruptIn_Test03      複数のInterruptIn（チャタリング対策）のテスト: F767ZI
MCP3008_Test01          MCP3008の読み取り時間を計測
random_Test01           random()関数の処理時間を計測
Pot_Panel_Test01        POT_Panelの読み取りテスト：F446RE
Pot_Panel_Test02        POT_Panelの読み取りテスト：F767ZI
RotaryEncoder_Test01    RotaryEncoderのテスト
RotaryEncoder_Test02    RotaryEncoder x 6のテスト
