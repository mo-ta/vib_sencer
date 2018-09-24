# Vibration_sensor.ino

## 概要
- [設定モード](#m_setting)と計測モードの２つのモードがあり、起動時は[設定モード](#m_setting)で起動
- [計測モード](#m_mesure)では振動有無判定でデジタルポートを出力し、シリアル通信でデータを送信
- [計測モード](#m_mesure)では設定したインターバル毎に割込み処理により、振動の有無の判定を行う。<br>データ取得自体はメインループ毎に実施

### sample
```
# --expample--
ready:
>RESET               #とりあえずRESET

ready:
>GeT vErsIon         #version確認  大文字小文字は関係ない
[ver0.20 180921]     #返り値
>SET algolism 2      #algolism設定 MAX-MIN
>GET algolism        #algolism確認
[2]
>START               #計測開始
c0,0,90,40           #データ loop counter,dout,eval,data_num
c1,0,91,40
c2,1,120,40
c3,1,119,40
c4,1,121,40
c5,0,90,40
c6,0,90,40
.,.,.,.
.,.,.,.
.,.,.,.
.,.                   #改行入力で計測終了
>
```

### Serial通信オプション<a name="serial">

| Pram       | Value    |
|------------|----------|
| ボーレート | 115200   |
| Data長     | 8bit     |
| パリティ   | none     |
| Stopビット | 1bit     |
| フロー制御 | none     |
| 改行 <- PC | <CR><LF> |
| 改行 -> PC | <CR>     |

## 設定モード<a name="m_setting">
- 起動時は設定モードから
- コマンドはスペース区切り。スペースは複数でも可、行頭、行末にあっても可。
- 大文字、小文字を認識しない(どっちでもOK)
- 入力受付OKの合図としてプロンプト '>'を出力(改行なし)
- STARTで[計測モード](#m_mesure)に移行
- [計測モード](#m_mesure)で 入力(改行で検知)があると計測終了して、設定モードに移行する

### メインコマンド<a name="main">

| コマンド  | 内容                               |
|-----------|------------------------------------|
| START     | 計測開始  計測モードへ移行         |
| RESET     | マイコンのリセット                 |
|[GET](#get)| パラメータやバージョン情報等を取得 |
|[SET](#set)| パラメータ等を設定                 |	

- GETは引数+1個  SETは+2個

### GET コマンドオプション<a name="get">

```
# --usage--
#入力
GET get_option <CR><LF>
#返り値
[VALUE]<CR>

# --expample--
                     # '<CR>1'  '<LF>' は改行コード
                     # `>`はプロンプト arduinoの出力
>Get BPS<CR><LF>     # PC -> Arduino
[115200]<CR>         # PC <- Arduino
>
```

[value] 以外の返り値はすべてエラー

**GET option 一覧**

| GET OPTION  | 内容                       |
|-------------|----------------------------|
| VERSION     | バージョン情報             |
| ALGOLISM    | 振動判定アルゴリズムの番号 |
| THRESH      | 振動判定時の閾値           |
| INTERVAL    | 振動判定の周期(msec)       |
| DOUT_PIN    | 振動判定結果出力のピン番号 |
| AIN_PIN     | 振動センサ入力ピン番号     |
| SYNC_HEADER | データ同期用ヘッダ出力     |
| COMM_TYPE   |                            |
| BPS         | 通信ボーレート             |
| DEBUG       | DEBUG フラグ               |

### SET コマンドオプション<a name="set">

```
# --usage--
#入力
SET set_option value<CR><LF>
#返り値はなく、プロンプト('>')が返ってくる
#それ以外はすべてエラー
# --expample--
                          # '<CR>1'  '<LF>' は改行コード
                          # `>`はプロンプト arduinoの出力
>Set algolism 2<CR><LF>   # PC -> Arduino
>Get algolism<CR><LF>     # PC -> Arduino   GETでチェック
[2]<CR>                   # PC <- Arduino   
>
```

返り値はなく、プロンプト('>')が返ってくる。
それ以外はすべてエラー


**SET option 一覧**

| SET OPTION           | 内容                       |
|----------------------|----------------------------|
| [ALGOLISM](#algolism)| 振動判定アルゴリズムの番号 |
| THRESH               | 振動判定時の閾値           |
| INTERVAL             | 振動判定の周期(msec)       |
| DOUT_PIN             | 振動判定結果出力のピン番号 |
| AIN_PIN              | 振動センサ入力ピン番号     |
| SYNC_HEADER          | データ同期用ヘッダ出力     |
| COMM_TYPE            |                            |
| DEBUG                | DEBUG フラグ               |

### ALGOLISM 番号 <a name="algolism">

| ALGO TYPE | VALUE                                               |
|-----------|-----------------------------------------------------|
| 1         | 前回測定(メインループでの) 差分のINTERVAL間の平均値 |
| 2         | INTERVAL間のMAX、MINの差分                          |
| 3         | INTERVAL間平均値                                    |
| 4         | 直値(平均なし)                                      |
| 5         | INTERVAL間の分散                                    |

## 計測モード<a name="m_mesure">
 計測モードで 入力(改行で検知)があると計測終了して、[設定モード](#m_setting)に移行する

### 出力形式
- 出力はコンマ区切り
- ループカウンタ,デジタル出力,評価値,インターバル中のデータ数
  - ループカウンタ           : ループカウンタの16進数 下2桁
  - デジタル出力             : 1(HIGH) or 0(LOW)                                   
  - 評価値                   : 評価値eval を longでcastしたもの
  - インターバル中のデータ数 : 10進数


## Todo
* コメント、ドキュメント整備
* 出力の取り込みしにくいので メッセージとSETの返り値簡単に識別できるようにする
* SETのパラメーターチェックが適当なのを直すか決める
* 同期用出力オプション
* [設定モード](#m_setting)でのデジタル出力、アナログ読込み対応(ポートの初期化必須）
* binary出力
