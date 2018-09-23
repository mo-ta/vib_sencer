/*
arduino-cli compile -b arduino:avr:uno SerialEvent
arduino-cli upload -p COM3 -b arduino:avr:uno SerialEvent
*/
#include <MsTimer2.h>

#define VERSION "ver0.20 180921"

#define COMMAND_CELL 3
#define COM_BUF_SIZE  50
#define BPS 115200
#define ULONG_MAX -1

#define ALGO_DIFF              1
#define ALGO_MAX_MIN           2
#define ALGO_VAL               3
#define ALGO_VAL_NO_AVE        4
#define ALGO_DISPERSION        5

#define C_NONE              0
#define C_TEXT              1
#define C_BINARY            2
 

typedef struct MeasureOption
{
    float        thresh;
    unsigned int interval;
    unsigned int algolism;
    unsigned int dout_pin;
    unsigned int ain_pin;
    unsigned int comm_type;
};

String inputString;                 // a String to hold incoming data

bool   fStringComplete = false;    // whether the string is complete
bool   fInSetting      = true;
bool   fInCalc         = false;
bool   f1stCycle       = true;
String params[COMMAND_CELL];

struct MeasureOption opt;

unsigned long cnt_loop     = 0;
unsigned long cnt_interval = 0;

unsigned int  data_keep_buf[2];
unsigned int  data_num        = 0;
unsigned long data_sum_buf    = 0;
float         data_square_buf = 0.0;

//prototype
void execCmd(String arg[]);
void execCmdSet(String arg[]);
void execCmdGet(String arg[]);

int splitCommand(String para[], int para_num, String in, int in_size);
void each_interval(void); 

void startMeasure(void) ;
void stopMeasure(void) ;

//�N������
void setup() {
    opt.thresh    = 100.0;
    opt.interval  = 5;
    opt.algolism  = ALGO_DIFF;
    opt.dout_pin  = 13;
    opt.ain_pin   = 0;
    opt.comm_type = C_TEXT;

    inputString.reserve(COM_BUF_SIZE);
    for(int i = 0; i < COMMAND_CELL; i++) params[i].reserve(COM_BUF_SIZE);
    fInSetting = true;

    Serial.begin(BPS);
    Serial.print("\n\nready:\n>");
}

//mainloop
void loop() {
    // print the string when a newline arrives:
    if (fInSetting)
    {
        //�R�}���h����
        if(fStringComplete)
        {
            fStringComplete = false;

            //�R�}���h���X�y�[�X��؂�ŕ���
            int ret = splitCommand(params, COMMAND_CELL, inputString, COM_BUF_SIZE);
            inputString = ""; //���������Ƃ��Ȃ��Ƃǂ�ǂ񑝂���

            //��s => �Ȃɂ����Ȃ�
            if(ret == 0){ ; }
            //�R�}���h 1-COMMAD_CELL => �R�}���h���s
            else if (0 < ret && ret <= COMMAND_CELL){ execCmd(params); }
            //����������
            else{ Serial.println("arg_error: wrong number of arguments"); }

            if(fInSetting){ Serial.print(">");} //�v���J�n�����Ƃ��̓v�����v�g���o���Ȃ�
        }
    }
    else
    {
       //�v���I������
       if(fStringComplete){
           fStringComplete = false;
	   inputString = "";
           stopMeasure();

           Serial.print("\n");
           Serial.println("measure_terminate:");
           Serial.print(">");
       }
       else
       //�v������
       {
            //data��荞��
            unsigned int input_val = analogRead(opt.ain_pin);
            float tmp = 0.0;

            fInCalc = true;
            switch(opt.algolism){
            case ALGO_DIFF:
                if(data_keep_buf[0] > input_val)
                {
                    data_sum_buf += data_keep_buf[0] - input_val;
                }
                else
                {
                    data_sum_buf += input_val - data_keep_buf[0];
                }
                data_keep_buf[0] = input_val;
                break;

            case ALGO_VAL:
                data_sum_buf += input_val;
                break;

            case ALGO_VAL_NO_AVE:
                data_keep_buf[0] = input_val;
                break;

            case ALGO_DISPERSION:
                data_sum_buf += input_val;
                tmp = input_val;  //�������|���̂�
                data_square_buf += (tmp * tmp);
                break;

            case ALGO_MAX_MIN:
                if(data_keep_buf[0]  > input_val) data_keep_buf[0] = input_val; //min
                if(data_keep_buf[1]  < input_val) data_keep_buf[1] = input_val; //max
                break;
            }
            data_num++;
            fInCalc = false;
        }
    }
    cnt_loop++;
}
// param
// splitCommand  :���s���݂̕�������X�y�[�X�ŕ������ĕʂ̔z��Ɉڂ�
// String para[] :�Ԃ��p�o�b�t�@
// int para_num  :�R�}���h��
// String in     :���͕�����
// int in_size   :���͕�����p�̃o�b�t�@�T�C�Y
// ret           :
int splitCommand(String para[], int para_num, String in, int in_size){
    int ret = 0;

    //�R�s�[�̈�̍쐬 
    String str;
    str.reserve(in_size);

    //in String�̃R�s�[
    for(int i = 0; i < in.length(); i++) str += in.charAt(i);

    //para�z��̏�����
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

//�R�}���h�̉��߂Ǝ��s
void execCmdSet(String arg[]){
    if(arg[1].equalsIgnoreCase("algolism"))
    {
        opt.algolism = arg[2].toInt();
    }
    else if(arg[1].equalsIgnoreCase("thresh"))
    {
        opt.thresh = arg[2].toFloat();
    }
    else if(arg[1].equalsIgnoreCase("interval"))
    {
        opt.interval = arg[2].toInt();
    }
    else if(arg[1].equalsIgnoreCase("dout_pin"))
    {
        opt.dout_pin = arg[2].toInt();
    }
    else if(arg[1].equalsIgnoreCase("ain_pin"))
    {
        opt.ain_pin = arg[2].toInt();
    }
    else if(arg[1].equalsIgnoreCase("comm_type"))
    {
        opt.comm_type = arg[2].toInt();
    }
    else if(arg[1].equalsIgnoreCase("dout_value"))
    {
        opt.comm_type = arg[2].toInt();
    }
    else
    {
        Serial.print("arg_error set: unknown arg ");
        Serial.println(arg[1]);
    }
}

void execCmdGet(String arg[]){

    if(arg[1].equalsIgnoreCase("algolism"))
    {
        Serial.println(opt.algolism);
    }
    else if(arg[1].equalsIgnoreCase("thresh"))
    {
        Serial.println(opt.thresh);
    }
    else if(arg[1].equalsIgnoreCase("interval"))
    {
        Serial.println(opt.interval);
    }
    else if(arg[1].equalsIgnoreCase("dout_pin"))
    {
        Serial.println(opt.dout_pin);
    }
    else if(arg[1].equalsIgnoreCase("ain_pin"))
    {
        Serial.println(opt.ain_pin);
    }
    else if(arg[1].equalsIgnoreCase("comm_type"))
    {
        Serial.println(opt.comm_type);
    }
    else if(arg[1].equalsIgnoreCase("bps")){
        Serial.println(BPS);
    }
    else if(arg[1].equalsIgnoreCase("version"))
    {
        Serial.println(VERSION);
    }
    else if(arg[1].equalsIgnoreCase("loop_count"))
    {
        Serial.println(cnt_loop);
    }
    else
    {
        Serial.print("arg_error get: unknown arg ");
        Serial.println(arg[1]);
    }
}

void execCmd(String arg[])
{
    //��s
    if(arg[0].length() == 0)
    {
        return;
    }

    //start
    if(arg[0].equalsIgnoreCase("start"))
    {
        if(!arg[1].equalsIgnoreCase(""))
        {
            Serial.println("arg_error start: expected 1");
        }
        else
        {
            Serial.println("mesure_start:");
            startMeasure();
        }
    }
    //reset
    else if(arg[0].equalsIgnoreCase("reset"))
    {
        if(!arg[1].equalsIgnoreCase(""))
        {
            Serial.println("arg_error reset: expected 1");
        }
        else
        {
            asm volatile ("  jmp 0");
        }
    }
    //set
    else if(arg[0].equalsIgnoreCase("set"))
    {
        if(arg[2].equalsIgnoreCase(""))
        {
            Serial.println("arg_error set: expected 3");
        }
        else
        {
            execCmdSet(arg);
        }
    }
    //get
    else if(arg[0].equalsIgnoreCase("get"))
    {
        if(arg[1].equalsIgnoreCase("") || !arg[2].equalsIgnoreCase("") )
        {
            Serial.println("arg_error get: expected 2");
        }
        else
        {
            execCmdGet(arg);
        }
    }
    //�s���ȃR�}���h
    else
    {
        Serial.print("arg_error: unknown ");
        Serial.println(arg[0]);
    }
}

/*
    SerialEvent occurs whenever a new data comes in the hardware serial RX. This
    routine is run between each time loop() runs, so using delay inside loop can
    delay response. Multiple bytes of data may be available.
*/
//�V���A���ʐM�̎�M�o�b�t�@�Ƀf�[�^������Ƃ��A
//loop()�֐����I�����Ď���loop()���J�n����O�ɌĂяo�����B
//���O�͕ς����Ⴞ��
//https://garretlab.web.fc2.com/arduino_reference/language/functions/communication/serial/available.html
void serialEvent(){
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n')
	{
            fStringComplete = true;
        }
    }
}

//�v���J�n
void startMeasure()
{
    //pinmode�ݒ�
    pinMode(opt.ain_pin,  INPUT);   //�O�̂��� �A�i���O���[�h��
    pinMode(opt.dout_pin, OUTPUT);

    //�|�[�g������
    digitalWrite(opt.dout_pin, LOW); //������LOW
    digitalWrite(opt.ain_pin, LOW);  //�O�̂��� �v���A�b�vOFF 

    //data buffer ������
    data_keep_buf[0] = 0;
    data_keep_buf[1] = 0;
    data_num = 0;
    data_sum_buf = 0;
    data_square_buf = 0.0; 
    if(opt.algolism == ALGO_MAX_MIN) data_keep_buf[0] = ULONG_MAX;
    cnt_loop = cnt_interval = 0;

    //timer start
    fInSetting = false;//�v�����[�h��
    MsTimer2::set(opt.interval, each_interval); //Timer�ݒ�
    MsTimer2::start();
    f1stCycle= true;
}

//�v���I��
void stopMeasure()
{
    fInSetting = true;
    MsTimer2::stop();
}

//�����ݏ���  Dout��ON-OFF����ƃV���A���o�͂�����
void each_interval() {
    static float eval; //�]���l
    static char serial_out[100]; //�o��
    static bool dout = 0;
    static long tmp_long = 0; 
    eval = 0.0;
    serial_out[0] = '\0';

    //�]���l�Z�o
    switch(opt.algolism)
    {
    case ALGO_DIFF:
    case ALGO_VAL:
        if(data_num)
        {
            eval = data_sum_buf;
            eval /= data_num;
        }
        break;

    case ALGO_VAL_NO_AVE:
        eval = data_keep_buf[0];
        break;

    case ALGO_MAX_MIN:
        eval = data_keep_buf[1] - data_keep_buf[0];
        break;

    case ALGO_DISPERSION:
        if(data_num)
            eval =  data_sum_buf;
            eval /= data_num;
            eval =  -1.0 * eval * eval;
            eval += (data_square_buf / data_num);

            if(!f1stCycle && (fInCalc || eval < 0))
       {
            Serial.println("====");
            Serial.println(cnt_interval);
            Serial.print("in:");
            Serial.println(fInCalc);
            Serial.print("sum:");
            Serial.println(data_sum_buf);
            Serial.print("sqr:");
            Serial.println(data_square_buf);
            Serial.print("num:");
            Serial.println(data_num);
            Serial.print("eval:");
            Serial.println(eval);
            Serial.println("=--=");
        }
        break;
    }
    if(f1stCycle){
        eval = 0.0;
        f1stCycle = false;
    }
    //����
    if (eval > opt.thresh)
    {
        digitalWrite(opt.dout_pin, HIGH);
        dout = 1;
    }
    else
    {
        digitalWrite(opt.dout_pin, LOW);
        dout = 0;
    }

    //Serial�o��
    switch(opt.comm_type)
    {
    case C_NONE: 
        break;

    case C_TEXT:
        sprintf(serial_out, "%x,%d,%ld,%d\n", (byte)cnt_interval, dout, (long)eval, (byte)data_num);
        Serial.print(serial_out);
        break;

    case C_BINARY:
        Serial.println(eval);
        break; 
    }

    //�f�[�^������
    data_sum_buf = data_num = 0;
    data_square_buf = 0.0;
    if(opt.algolism == ALGO_MAX_MIN)
    {
        data_keep_buf[0] = ULONG_MAX;
        data_keep_buf[1] = 0;
    }

    cnt_interval++;
    interrupts();//���܂��Ȃ�
}


