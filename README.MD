# 加速度センサー
 
## 将来的に実現したいこと
ジャイロ＆加速度センサー(MPU6886)で計測したデータを用いてをクラウドまで転送。
そのデータから機械のON/OFFを検知したい。
 
## 実装範囲
今回の検証では、SDカードにセンサーデータを保存までを行う。
 
- 機能
    - ログ出力のON/OFF切り替え機能
 
- 保存するデータ
    - 時刻
    - ジャイロセンサーデータ(gyro x, gyro y, gyro z)
    - 加速度(acce x, acce y, acce z)
    - 温度(temperature) ※稼働温度を超えないか見るため
 
- 画面表示
    - バッテリー残量(Volt, ％)
    - SDカードの残容量(MB)
    - 時刻(YYYY/MM/DD HH:mm:ss)
    - ジャイロセンサーデータ(gyro x, gyro y, gyro z)
    - 加速度(acce x, acce y, acce z)
    - 温度(temperature) ※稼働温度を超えないか見るため
    - ログステータス
 
 
 
## 雑多メモ
- sd(microSD)は16GBまで
- リチウムイオン電池ではバッテリー電圧が4.2VでMAX、3.2Vで残量0とすることが一般的（？）らしい。参考サイトでは3.65Vでカットオフが起きて使えなくなってそう。
 
### 稼働時間の目安
m5core2はInput Voltageが5V@500mA（消費電力2.5Wh） 市販の10000mhA（5V）のものでおよそ20時間計算。計測頻度とか減らせばもっと伸びるかも？
 
## 参考
- [1.公式サイト](https://docs.m5stack.com/ja/core/core2)
- [2.ベースのコード](https://tomosoft.jp/design/?p=45288)
- [3.バッテリーに関するあれこれ](https://note.com/khe00716/n/n1eac1a9461e6)