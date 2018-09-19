/*
arduino-cli compile -b arduino:avr:uno SerialEvent
arduino-cli upload -p COM3 -b arduino:avr:uno SerialEvent
*/
#include <MsTimer2.h>
#define VERSION "ver0.01 180918"

#define COMMAND_CELL 3
#define COM_BUF_SIZE  50
#define BPS 115200
#define ULONG_MAX -1

#define A_DIFF              1
#define A_MAX_MIN           2
#define A_VAL               3

#define C_NONE              0
#define C_TEXT              1
#define C_BINARY            2
 

String inputString;                 // a String to hold incoming data
bool stringComplete   = false;    // whether the string is complete

String params[COMMAND_CELL];

unsigned int settingState      = 1;

float para_thresh     = 100;
unsigned int para_interval = 5;
unsigned int para_algolism = A_DIFF;
unsigned int para_dout_pin = 13;
unsigned int para_ain_pin  = 0;
unsigned int para_comm_type = C_TEXT;

unsigned long loop_cnt = 0;
unsigned long interval_cnt = 0;

unsigned int  data_keep[2];
unsigned int  data_counter = 0;
unsigned long data_sum     = 0;

//prototype
void execCmd(String param[]);
int splitCommand(String para[], int para_num, String in, int in_size);
void each_interval(void); 
void setup_measure(void) ;

void setup() {
    Serial.begin(BPS);
    inputString.reserve(COM_BUF_SIZE);
    for(int i = 0; i < COMMAND_CELL; i++) params[i].reserve(COM_BUF_SIZE);
    settingState = true;
    Serial.print("\n\nready:\n>");
}

void loop() {
    // print the string when a newline arrives:
    if (settingState){
        if(stringComplete) {
            int ret = splitCommand(params, COMMAND_CELL, inputString, COM_BUF_SIZE);
            inputString = ""; //初期化しとかないとどんどん増える
            if(ret == 0){
                 ;
            }
            else if(ret < 0 || COMMAND_CELL < ret){
                 Serial.println("Input Err \n");
            }
            else{
        execCmd(params);
        }

            stringComplete = false;
        if(settingState){
        Serial.print(">");
            }
        }
    }
    else
    {
       //data取り込み
       unsigned int input_val = analogRead(para_ain_pin);
       switch(para_algolism){
       case A_DIFF:
           if(data_keep[0] > input_val){
               data_sum += data_keep[0] - input_val;
           }
           else{
               data_sum +=  input_val - data_keep[0];
           }
           data_keep[0] = input_val;
           data_counter++;
           break;

       case A_VAL:
           data_sum += input_val;
           data_counter++;
           break;

       case A_MAX_MIN:
           if(data_keep[0]  > input_val) data_keep[0] = input_val; //min
           if(data_keep[1]  < input_val) data_keep[1] = input_val; //max
           data_counter++;
            break;
       }
    }
    loop_cnt++;
}
/*
    SerialEvent occurs whenever a new data comes in the hardware serial RX. This
    routine is run between each time loop() runs, so using delay inside loop can
    delay response. Multiple bytes of data may be available.
*/
// param
// splitCommand  :改行込みの文字列をスペースで分割して別の配列に移す
// String para[] :返し用バッファ
// int para_num  :コマンド数
// String in     :入力文字列
// int in_size   :入力文字列用のバッファサイズ
// ret           :
int splitCommand(String para[], int para_num, String in, int in_size){
    int ret = 0;

    //コピー領域の作成 
    String str;
    str.reserve(in_size);

    //in Stringのコピー
    for(int i = 0; i < in.length(); i++) str += in.charAt(i);

    //para配列の初期化
    for(int i = 0 ;i < para_num; i++) para[i] = ""; 

    for(int cnt = 0 ;cnt <= para_num; cnt++){
        str.trim();
        if(str.length() == 0 && ret ==0) return 0;

        ret++;
        int pos = str.indexOf(" ");
        if(pos == -1){
            if(str.length() != 0) {
                if(cnt == para_num){return -1;}
                para[cnt] = String(str);
            }
                return ret;
        }
        else{
            if(cnt == para_num){return -1;}
            para[cnt] = String(str.substring(0,pos));
            str = String(str.substring(pos));
        }
    }
    return -2;
}

void execCmd(String param[]){
    if(param[0].length() == 0) {
        return;
    }
    if(param[0].equalsIgnoreCase("start")){
        if(!param[1].equalsIgnoreCase("")){    
            Serial.println("Param is too much");
            return;
        }
        settingState = 0;
        Serial.println("start");
        setup_measure();
    }
    else if(param[0].equalsIgnoreCase("set")){
        if(param[2].equalsIgnoreCase("")){    
            Serial.println("SetValue is Nothing");
            return;
        }
        if(param[1].equalsIgnoreCase("algolism")){
            para_algolism = param[2].toInt();
        }
        else if(param[1].equalsIgnoreCase("thresh")){
            para_thresh = param[2].toFloat();
        }
        else if(param[1].equalsIgnoreCase("interval")){
            para_interval = param[2].toInt();
        }
        else if(param[1].equalsIgnoreCase("dout_pin")){
            para_dout_pin = param[2].toInt();
        }
        else if(param[1].equalsIgnoreCase("ain_pin")){
            para_ain_pin = param[2].toInt();
        }
        else if(param[1].equalsIgnoreCase("comm_type")){
            para_comm_type = param[2].toInt();
        }
        else{
            Serial.print("Unkown SetParam :");
            Serial.println(param[1]);
        }
    }
    else if(param[0].equalsIgnoreCase("get")){

        if(!param[2].equalsIgnoreCase("")){    
            Serial.println("Param is too much");
            return;
        }
        if(param[1].equalsIgnoreCase("algolism")){
            Serial.println(para_algolism);
        }
        else if(param[1].equalsIgnoreCase("thresh")){
            Serial.println(para_thresh);
        }    
        else if(param[1].equalsIgnoreCase("interval")){
            Serial.println(para_interval);
        }
        else if(param[1].equalsIgnoreCase("dout_pin")){
            Serial.println(para_dout_pin);
        }
        else if(param[1].equalsIgnoreCase("ain_pin")){
            Serial.println(para_ain_pin);
        }
        else if(param[1].equalsIgnoreCase("comm_type")){
            Serial.println(para_comm_type);
        }
        else if(param[1].equalsIgnoreCase("bps")){
            Serial.println(BPS);
        }
        else if(param[1].equalsIgnoreCase("version")){
            Serial.println(VERSION);
        }
        else if(param[1].equalsIgnoreCase("loop_count")){
            Serial.println(loop_cnt);
        }
        else{
            Serial.print("Unkown GetParam :");    
            Serial.println(param[1]);    
        }

    }
    
}
void serialEvent(){
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n') {
            stringComplete = true;
        }
    }
}

void setup_measure() {
    pinMode(para_ain_pin, INPUT);   //念のため アナログモードに
    digitalWrite(para_ain_pin, LOW);//念のため プルアップOFF 
    pinMode(para_dout_pin, OUTPUT);
    digitalWrite(para_dout_pin, LOW); //初期はLOW
    data_keep[0] = 0;
    data_keep[1] = 0;
    data_counter = 0;
    data_sum     = 0;
    if(para_algolism == A_MAX_MIN) data_keep[0] = ULONG_MAX;
    loop_cnt = 0;
    MsTimer2::set(para_interval, each_interval); //Timer設定
    MsTimer2::start();
}

void each_interval() {
    static unsigned long eval; //評価値
    static char serial_out[100]; //出力
    static bool dout = 0;

    eval = 0;
    serial_out[0] = '\0';

    //評価値算出
    switch(para_algolism){
    case A_DIFF:
    case A_VAL:
        eval = (data_counter) ? (data_sum / data_counter) : 0;
        break;

    case A_MAX_MIN:
        eval = (data_keep[1] > data_keep[0]) ? data_keep[1] - data_keep[0] : 0;
        break;
    }

    //判定
    if (eval > para_thresh) {
        digitalWrite(para_dout_pin, HIGH);
        dout = 1;
    }
    else {
        digitalWrite(para_dout_pin, LOW);
        dout = 0;
    }

    //Serial出力
    switch(para_comm_type){
    case C_NONE: break;

    case C_TEXT:
    sprintf(serial_out, "%x,%d,%ld,%d\n", (byte)interval_cnt, dout, eval, (byte)data_counter);
        Serial.print(serial_out);
    break;

    case C_BINARY:
    break; 

    }
    //データ初期化
    data_sum = data_counter = 0;
    if(para_algolism == A_MAX_MIN){
        data_keep[0] = ULONG_MAX;
        data_keep[1] = 0;
    }

    interval_cnt++;
    interrupts();//おまじない
}


