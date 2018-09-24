# Vibration_sensor.ino

## �T�v
- [�ݒ胂�[�h](#m_setting)�ƌv�����[�h�̂Q�̃��[�h������A�N������[�ݒ胂�[�h](#m_setting)�ŋN��
- [�v�����[�h](#m_mesure)�ł͐U���L������Ńf�W�^���|�[�g���o�͂��A�V���A���ʐM�Ńf�[�^�𑗐M
- [�v�����[�h](#m_mesure)�ł͐ݒ肵���C���^�[�o�����Ɋ����ݏ����ɂ��A�U���̗L���̔�����s���B<br>�f�[�^�擾���̂̓��C�����[�v���Ɏ��{

### sample
```
# --expample--
ready:
>RESET               #�Ƃ肠����RESET

ready:
>GeT vErsIon         #version�m�F  �啶���������͊֌W�Ȃ�
[ver0.20 180921]     #�Ԃ�l
>SET algolism 2      #algolism�ݒ� MAX-MIN
>GET algolism        #algolism�m�F
[2]
>START               #�v���J�n
c0,0,90,40           #�f�[�^ loop counter,dout,eval,data_num
c1,0,91,40
c2,1,120,40
c3,1,119,40
c4,1,121,40
c5,0,90,40
c6,0,90,40
.,.,.,.
.,.,.,.
.,.,.,.
.,.                   #���s���͂Ōv���I��
>
```

### Serial�ʐM�I�v�V����<a name="serial">

| Pram       | Value    |
|------------|----------|
| �{�[���[�g | 115200   |
| Data��     | 8bit     |
| �p���e�B   | none     |
| Stop�r�b�g | 1bit     |
| �t���[���� | none     |
| ���s <- PC | <CR><LF> |
| ���s -> PC | <CR>     |

## �ݒ胂�[�h<a name="m_setting">
- �N�����͐ݒ胂�[�h����
- �R�}���h�̓X�y�[�X��؂�B�X�y�[�X�͕����ł��A�s���A�s���ɂ����Ă��B
- �啶���A��������F�����Ȃ�(�ǂ����ł�OK)
- ���͎�tOK�̍��}�Ƃ��ăv�����v�g '>'���o��(���s�Ȃ�)
- START��[�v�����[�h](#m_mesure)�Ɉڍs
- [�v�����[�h](#m_mesure)�� ����(���s�Ō��m)������ƌv���I�����āA�ݒ胂�[�h�Ɉڍs����

### ���C���R�}���h<a name="main">

| �R�}���h  | ���e                               |
|-----------|------------------------------------|
| START     | �v���J�n  �v�����[�h�ֈڍs         |
| RESET     | �}�C�R���̃��Z�b�g                 |
|[GET](#get)| �p�����[�^��o�[�W������񓙂��擾 |
|[SET](#set)| �p�����[�^����ݒ�                 |	

- GET�͈���+1��  SET��+2��

### GET �R�}���h�I�v�V����<a name="get">

```
# --usage--
#����
GET get_option <CR><LF>
#�Ԃ�l
[VALUE]<CR>

# --expample--
                     # '<CR>1'  '<LF>' �͉��s�R�[�h
                     # `>`�̓v�����v�g arduino�̏o��
>Get BPS<CR><LF>     # PC -> Arduino
[115200]<CR>         # PC <- Arduino
>
```

[value] �ȊO�̕Ԃ�l�͂��ׂăG���[

**GET option �ꗗ**

| GET OPTION  | ���e                       |
|-------------|----------------------------|
| VERSION     | �o�[�W�������             |
| ALGOLISM    | �U������A���S���Y���̔ԍ� |
| THRESH      | �U�����莞��臒l           |
| INTERVAL    | �U������̎���(msec)       |
| DOUT_PIN    | �U�����茋�ʏo�͂̃s���ԍ� |
| AIN_PIN     | �U���Z���T���̓s���ԍ�     |
| SYNC_HEADER | �f�[�^�����p�w�b�_�o��     |
| COMM_TYPE   |                            |
| BPS         | �ʐM�{�[���[�g             |
| DEBUG       | DEBUG �t���O               |

### SET �R�}���h�I�v�V����<a name="set">

```
# --usage--
#����
SET set_option value<CR><LF>
#�Ԃ�l�͂Ȃ��A�v�����v�g('>')���Ԃ��Ă���
#����ȊO�͂��ׂăG���[
# --expample--
                          # '<CR>1'  '<LF>' �͉��s�R�[�h
                          # `>`�̓v�����v�g arduino�̏o��
>Set algolism 2<CR><LF>   # PC -> Arduino
>Get algolism<CR><LF>     # PC -> Arduino   GET�Ń`�F�b�N
[2]<CR>                   # PC <- Arduino   
>
```

�Ԃ�l�͂Ȃ��A�v�����v�g('>')���Ԃ��Ă���B
����ȊO�͂��ׂăG���[


**SET option �ꗗ**

| SET OPTION           | ���e                       |
|----------------------|----------------------------|
| [ALGOLISM](#algolism)| �U������A���S���Y���̔ԍ� |
| THRESH               | �U�����莞��臒l           |
| INTERVAL             | �U������̎���(msec)       |
| DOUT_PIN             | �U�����茋�ʏo�͂̃s���ԍ� |
| AIN_PIN              | �U���Z���T���̓s���ԍ�     |
| SYNC_HEADER          | �f�[�^�����p�w�b�_�o��     |
| COMM_TYPE            |                            |
| DEBUG                | DEBUG �t���O               |

### ALGOLISM �ԍ� <a name="algolism">

| ALGO TYPE | VALUE                                               |
|-----------|-----------------------------------------------------|
| 1         | �O�񑪒�(���C�����[�v�ł�) ������INTERVAL�Ԃ̕��ϒl |
| 2         | INTERVAL�Ԃ�MAX�AMIN�̍���                          |
| 3         | INTERVAL�ԕ��ϒl                                    |
| 4         | ���l(���ςȂ�)                                      |
| 5         | INTERVAL�Ԃ̕��U                                    |

## �v�����[�h<a name="m_mesure">
 �v�����[�h�� ����(���s�Ō��m)������ƌv���I�����āA[�ݒ胂�[�h](#m_setting)�Ɉڍs����

### �o�͌`��
- �o�͂̓R���}��؂�
- ���[�v�J�E���^,�f�W�^���o��,�]���l,�C���^�[�o�����̃f�[�^��
  - ���[�v�J�E���^           : ���[�v�J�E���^��16�i�� ��2��
  - �f�W�^���o��             : 1(HIGH) or 0(LOW)                                   
  - �]���l                   : �]���leval �� long��cast��������
  - �C���^�[�o�����̃f�[�^�� : 10�i��


## Todo
* �R�����g�A�h�L�������g����
* �o�͂̎�荞�݂��ɂ����̂� ���b�Z�[�W��SET�̕Ԃ�l�ȒP�Ɏ��ʂł���悤�ɂ���
* SET�̃p�����[�^�[�`�F�b�N���K���Ȃ̂𒼂������߂�
* �����p�o�̓I�v�V����
* [�ݒ胂�[�h](#m_setting)�ł̃f�W�^���o�́A�A�i���O�Ǎ��ݑΉ�(�|�[�g�̏������K�{�j
* binary�o��
