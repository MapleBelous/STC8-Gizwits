"STC8A8KS4A64单片机通过机智云通讯协议连接ESP8266-01模块，连接机智云上传DS18B20温度传感器温度值以及高低温报警，控制两路LED。"
CSDN简介：https://blog.csdn.net/belous_zxy/article/details/105485200

    Code - 代码文件夹
        main.c - 主函数 
        Basis - 单片机相关部分
            ISR.c - 中断函数
            def - 各种基本DEFINE
                MCUdef.h
                pindef.h - 引脚功能DEFINE
            delay - 各主频下精准延时，通过宏设置需要的延时函数
                delay.c
                delay.h
                delay_YX.h
            init - 初始化程序
                init.c
                init.h
            uart - 串口相关函数
                uart.c
                uart.h
        Gizwits - 机智云协议部分
            GizwitsConfig.c - 相关code数组，设置
            GizwitsConfig.h
            GizwitsHandle.c - 读取串口缓冲区接收命令，发送命令，重发命令等
            GizwitsHandle.h
            GizwitsMSG.c - 处理命令，任务函数等
            GizwitsMSG.h 
        Sensor - 传感器部分
            LCD1602 - LCD1602库
                LCD1602.c
                LCD1602.h
            DS18B20 - DS18B20库
                DS18B20.c
                DS18B20.h 

LCD1602只是顺手写的库,并没有用上，仅供参考

程序大多都详细注释了，希望能帮助到大家。
