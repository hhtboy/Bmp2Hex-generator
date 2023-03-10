# Bmp2Hex-generator
BMP파일을 hex 파일로 변경해주는 프로그램입니다. 2bit 색상 또는 4bit색상을 지원합니다.



## 사용 방법


  
 아래 명령어를 통해 a.out을 생성합니다.
 
    gcc bmp_to_hex.c
 
    

<br><br>  
파일을 실행하면 아래와 같이 뜹니다.

    ./a.out
    <Usage> 
    1. Enter bmp file name.
    2. Enter bit size of colors : 4 or 2
    File format must be 24bit bmp.
    Image size must be smaller than 160 X 240. 
    Length of bmp file name must be shorter than 30.
    
bmp파일은 다음과 같은 형식을 필요로 합니다.
1. 파일 포맷이 24bit bmp여야 합니다.(다른 형식일 경우 bmp 변환 사이트를 이용해서 변환해줍니다)
2. 이미지 사이즈는 160 X 240보다 작아야 합니다. (LCD 크기가 160 X 240)
3. bmp 파일 이름의 길이가 30보다 작아야 합니다.

<br><br>  
첫 번째 인자로 bmp 파일 이름을 입력하고(.bmp 확장자까지 입력해야 합니다),
두 번째 인자로 색상 비트 수를 입력하면 됩니다. (4 또는 2)

    emblem.bmp
    4
    

<br><br>  
그럼 이름과 색상 비트수로 조합한 헤더 파일이 생성됩니다.

    emblem_4bit.h
  
<br><br>  
## 헤더 파일
헤더 파일을 열어보면 아래와 같습니다.

    #ifndef EMBLEM_4BIT_H
    #define EMBLEM_4BIT_H
    static const IMAGE_DATA emblem_4bit_image = {50, 70, 4, { 
    //..
    };
    #endif
    
static const로 선언된 emblem_4bit_image의 맨 앞부분엔 헤더가 있습니다.<br>
50, 70은 이미지의 크기를 의미합니다. (50 X 70) <br>
4는 색상 비트 수를 의미합니다.
