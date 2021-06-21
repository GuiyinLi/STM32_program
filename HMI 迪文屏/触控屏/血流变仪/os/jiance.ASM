  ; DWIN OS
  ;R245,定时器4C倒计时;R246,赋值4CH为FF;R247,4C返回到的值
  ;R248,上电初始化标志
  ;R249,RTC秒的变化比较
  ;R250,R251,上一次触摸屏幕计时
  ;R252，R253,历史编号序号1-100，最多保存100条
  ;(67*3-128)*64*1024转换成16进制4900H
  
  
  
  
    
    IJNE	R245,0,SSTART			;标志位R245,计算检测单元第4个的时间
    CALL	READING
    CALL	PASSWORD
    CALL	PASSWORD_CHANGE
    CALL	SPECCY
    CALL	COMPAER_ONE
    CALL	ORDER_H
    CALL	ORDER_M
    END
 ;==============  密码登录  ================   
PASSWORD:   
	LDWR	R0,0510H;密码登录变量地址0510H
    MOVXR	R80,1,1;OS和DGUS数据传送，把DGUS变量地址0510H数据读到入R80：R81
    IJNE	R81,1,PASS_RET;OS寄存器与1比较，不等跳转到PASS_RET相等执行下一条
    LDWR	R80,0
    MOVXR	R80,0,1;R80:R81中的数据传到DGUS变量中
    LDWR	R0,0500H
    MOVXR	R80,1,2;把DGUS变量地址0500H数据读到R80:R81:R82:R83
	LDWR	R4,0049H;首地址
    LDWR	R6,0001H;
    LDWR	R0,0510H;
    MOVXL	2,2;R4 R6数据给0510H
    MOVXR	R84,1,2;0110中数据读到R84:R85:R86:R87
    TESTS	R80,R84,4;密码比较
    IJNE	R0,0,ERROR_1;相同执行下一条，不同跳转
    LDWR	R88,11;密码正确切换到11页面
    MOVRD	R88,3,2
ERROR_1:
	LDWR	R0,0500H;
	LDBR	R80,0,4
    MOVXR	R80,0,2   
PASS_RET:
    RET
 ;----------  更改密码  ---------------   
PASSWORD_CHANGE:
	LDWR	R0,0012H;确认更改密码变量地址
    MOVXR	R80,1,1;0012中密码数据读到R80:R81
    IJNE	R81,1,CHANGE_RET;
    LDWR	R80,0
    MOVXR	R80,0,1
    LDWR	R0,0505H;更改密码变量地址
	LDWR	R4,0049H
    LDWR	R6,0001H;清空R6
    MOVXL	3,2 ;R4 R6数据给0505H，新密码写入数据库 
CHANGE_RET:
	RET  
;========== 检测4个单元，均正常进入主界面 ==========    
SPECCY:

	CALL	SPECCY1
	CALL	SPECCY2
	CALL	SPECCY3
	CALL	SPECCY4
	RET
	
SPECCY1:	
	MOVDR	4,R10,1
	IJNE	R10,1,SPE_RET1
	LDWR	R10,1
	LDWR	R0,0001H
	MOVXR	R12,1,1;把0001中数据读到R12:R13
	JS		R12,R10,SPE_RET1
	LDWR	R22,2;待跳转页面
	MOVRD	R22,3,2
	
SPE_RET1:
	RET	
SPECCY2:	
	MOVDR	4,R10,1;执行后R10：R11中是当前页面号，若当前页面为2，R10=0X00,R11=0X22
	IJNE	R10,3,SPE_RET2;不等跳转SPE_RET2,相等执行下一条
	LDWR	R10,1
	LDWR	R0,0002H
	MOVXR	R14,1,1;把DGUS变量地址0002中的数据读到R14:R15
	JS		R14,R10,SPE_RET2;比较16bit整数大小，A<B跳转，A>=B执行下一条
	LDWR	R22,4;4待跳转页面
	MOVRD	R22,3,2;执行后跳转到第四页面
	
SPE_RET2:
	RET	
SPECCY3:	
	MOVDR	4,R10,1
	IJNE	R10,5,SPE_RET3
	LDWR	R10,1
	LDWR	R0,0003H 
	MOVXR	R16,1,1;0003中数据读到R16:R17
	JS		R16,R10,SPE_RET3
	LDWR	R22,6;待跳转页面
	MOVRD	R22,3,2;执行后跳转到第6页
	
SPE_RET3:
	RET	
SPECCY4:	
	MOVDR	4,R10,1
	IJNE	R10,7,SPE_RET4
	LDWR	R10,1
	LDWR	R0,0004H;
	MOVXR	R18,1,1
	JS		R18,R10,SPE_RET4
	LDWR	R22,8;待跳转页面
	MOVRD	R22,3,2;执行后跳转到8页面
	

	
	LDBR	R11,1,1
	AND		R11,R13,1
	AND		R11,R15,1
	AND		R11,R17,1
	AND		R11,R19,1
	
	IJNE	R11,1,COUNT_SECOND
	LDWR	R22,9;
	MOVRD	R22,3,2
	
SPE_RET4: 
	RET
COUNT_SECOND:
	LDBR	R246,FFH,1
	MOVRD	R246,4CH,1
	LDBR	R245,1,1
SSTART:	
	MOVDR	4CH,R247,1
	IJNE	R247,0,END1
	
	LDBR	R245,0,1
	LDWR	R22,0	
	MOVRD	R22,3,2	
	RET	
END1:
	END
	
;==========  上电读取部分固定的数据信息 =========    
READING: 
	LDWR	R4,0049H
	LDWR	R6,0
	LDWR	R0,4100H
	MOVXL	2,1
	MOVXR	R150,1,1
	LDWR	R152,5AA5H
	TESTS	R150,R152,2
	IJNE	R0,0,WRITE_1		 ;第一次初始密码
	
	IJNE  	R248,0,SAVE_INFOR	
    LDBR  	R248,1,1			  ;标志位R248
 	COMSET 	0,651  			      ;COM1初始,9600bps
 	
 	LDWR	R4,0049H
	LDWR	R6,1
	LDWR	R0,4201H
	MOVXL	2,2DH
	
	
	
	LDWR	R0,4204H
	LDWR	R2,0030H			 ;0060004，医院名称、科室名称、医生信息
	MOVXX	26
	
		
 	RET

WRITE_1:
	LDWR	R150,5AA5H
	LDWR	R152,1
	LDWR	R154,E240H
	LDWR	R156,5
	LDWR	R0,4120H
	MOVXR	R150,0,4
	LDWR	R4,0049H
	LDWR	R6,0
	MOVXL	3,4
	
	LDWR	R150,0
	LDWR	R0,3300H
	MOVXR	R150,0,1
	LDWR	R4,0049H
	LDWR	R6,60H
	MOVXL	3,1
	RET
 	
; 	LDBR 	R4,89,1    				;89号字库，当前历史报告存到的字库位置
;	LDBR 	R5,0,3
;	LDWR 	R0,0009H
;	MOVXL 	0,1
;	MOVXR 	R23,1,1
;	LDBR  	R25,90,1
;	LDBR  	R26,119,1
;	JS	 	R24,R25,2			,	;90-119号字库
;	JS    	R26,R24,1
;	GOTO	SAVE_INFOR
;	LDBR	R24,90,1					R24=90
;----------------------           
 SAVE_INFOR:	           
	LDWR	R0,0010H
	MOVXR	R30,1,1
	IJNE	R31,1,7
	LDWR	R30,0
	MOVXR	R30,0,1
	LDWR 	R4,0049H   			  ;00600004(15个字），保存医院名称
	LDWR 	R6,4
	LDWR 	R0,0030H
	MOVXL 	3,15
	RET
	
	IJNE	R31,2,7
	LDWR	R30,0
	MOVXR	R30,0,1
	LDWR 	R4,0049H   			  ;00600013(6个字），保存科室名称
	LDWR 	R6,0013H
	LDWR 	R0,003FH
	MOVXL 	3,6
	RET
	
	IJNE	R31,3,7
	LDWR	R30,0
	MOVXR	R30,0,1
	LDWR 	R4,0049H    			  ;00600019(5个字），保存医生信息
	LDWR	R6,19H
	LDWR 	R0,0045H
	MOVXL 	3,5
	RET
	
	
	;========== 进入一次测试界面 ===================== 
COMPAER_ONE:
		LDWR	R0,001AH		
  		MOVXR	R30,1,1
  		IJNE	R31,1,COMPARE_RET
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
  		LDWR 	R4,60H         ;;;读上一次历史编号
	    LDWR 	R6,50H
	    LDWR	R0,300H
  	    MOVXL 	2,6	          
 		MOVXR	R201,1,6	
RI:	
	MOVDR	20H,R50,3		;;;比较时间是否为同一天
	BITS	R52,0304H       ;;;日
	    LDWR	R50,0
	    LDWR	R0,0300H
	    MOVXR	R50,0,1
	    LDWR	R50,0
	    LDWR	R0,0301H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0302H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0303H
	    MOVXR	R50,0,1
	BITI	R61,0300H
	    LDWR	R50,0
	    LDWR	R0,0304H
	    MOVXR	R50,0,1
	    LDWR	R50,0
	    LDWR	R0,0305H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0306H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0307H
	    MOVXR	R50,0,1
	BITI	R62,0304H
			CJNE	R61,R207,2			;比较日的前一位数是否相同
			CJNE	R62,R208,1			;比较日的后一位数是否相同
	        GOTO	YUE
  		MOV		R61,R207,2				;日不同则日的两位ASC码R61  R62给R207  R208
  		LDBR	R209,30H,3				;
  		
  YUE:	
	MOVDR	20H,R50,3		;;;比较时间是否为同一天
	BITS	R51,0304H       ;;;月
	    LDWR	R50,0
	    LDWR	R0,0300H
	    MOVXR	R50,0,1
	    LDWR	R50,0
	    LDWR	R0,0301H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0302H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0303H
	    MOVXR	R50,0,1
	BITI	R61,0300H
	    LDWR	R50,0
	    LDWR	R0,0304H
	    MOVXR	R50,0,1
	    LDWR	R50,0
	    LDWR	R0,0305H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0306H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0307H
	    MOVXR	R50,0,1
	BITI	R62,0304H
			CJNE	R61,R205,2
			CJNE	R62,R206,1
	        GOTO	NIAN
  		MOV		R61,R205,2		;月不同则月的两位ASC码R61  R62给R205  R206
  		LDBR	R209,30H,3		;R209 R210 R211的ASC码从000开始
  		
NIAN:	
	MOVDR	20H,R50,3		;;;比较时间是否为同一天
	BITS	R50,0304H       ;;;年
	    LDWR	R50,0
	    LDWR	R0,0300H
	    MOVXR	R50,0,1
	    LDWR	R50,0
	    LDWR	R0,0301H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0302H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0303H
	    MOVXR	R50,0,1
	BITI	R61,0300H
	    LDWR	R50,0
	    LDWR	R0,0304H
	    MOVXR	R50,0,1
	    LDWR	R50,0
	    LDWR	R0,0305H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0306H
	    MOVXR	R50,0,1
	    LDWR	R50,1
	    LDWR	R0,0307H
	    MOVXR	R50,0,1
	BITI	R62,0304H
			CJNE	R61,R203,2
			CJNE	R62,R204,1
	        GOTO	BIANHAO
  		MOV		R61,R203,2			;年不同则年的两位ASC码R61  R62给R203  R204
  		LDBR	R209,30H,3 		
  		
  	BIANHAO:
  			LDBR	R201,32H,1
  			LDBR	R202,30H,1		;R201 R202是20？（年的前两位）
  			LDBR	R212,00H,1		;R212赋值FF，不显示这一字节
	  			INC		R211,0,1	;编号的最低位在原来的基础上加1
	  			IJNE	R211,3AH,7
	  			LDBR	R211,30H,1
	  			INC		R210,0,1
	  			IJNE	R210,3AH,4
	  			LDBR	R210,30H,1
	  			INC		R209,0,1
	  			IJNE	R209,3AH,1
	  			LDBR	R209,30H,1
  			LDWR	R0,0110H		;显示这编号
  			MOVXR	R201,0,6
  			LDWR 	R4,49H         ;;;保存最新的历史编号
	    	LDWR 	R6,50H
	        MOVXL   3,6
COMPARE_RET:
		RET
;==============  动作和状态指令 =========
ORDER_H:
	LDWR	R0,0136H		
  	MOVXR	R30,1,1
  	IJNE	R31,1,6
  	LDWR	R30,0
  	MOVXR	R30,0,1					;开始检测
  	LDWR	R101,5AA5H
	LDWR	R103,F101H
 	COMTXI	0,R101,4
    RET
    
    LDWR	R0,0006H		
  	MOVXR	R30,1,1
  	IJNE	R31,1,6
  	LDWR	R30,0					;没有清洗液
  	MOVXR	R30,0,1	
  	LDWR	R101,5AA5H
	LDWR	R103,F102H
 	COMTXI	0,R101,4
	RET
	
	LDWR	R0,0007H		
  	MOVXR	R30,1,1
  	IJNE	R31,1,6
  	LDWR	R30,0					;废纸桶已满
  	MOVXR	R30,0,1
  	LDWR	R101,5AA5H
	LDWR	R103,F103H
 	COMTXI	0,R101,4
 	RET
 	
	LDWR	R0,0020H		
  	MOVXR	R30,1,1
  	IJNE	R31,1,6
  	LDWR	R30,0
  	MOVXR	R30,0,1	
  	LDWR	R101,5AA5H				;校准
	LDWR	R103,F108H
 	COMTXI	0,R101,4
 	RET
 	
 	
 	IJNE	R31,2,6
  	LDWR	R30,0
  	MOVXR	R30,0,1	
  	LDWR	R101,5AA5H				;清洗1
	LDWR	R103,F109H
 	COMTXI	0,R101,4
 	RET
 	
    IJNE    R31,3,6
  	LDWR	R30,0
  	MOVXR	R30,0,1	
  	LDWR	R101,5AA5H				;关机
	LDWR	R103,F105H
 	COMTXI	0,R101,4
 	RET
 	
 	LDWR	R0,000AH		
  	MOVXR	R30,1,1
  	IJNE	R30,0,7
  	IJNE	R31,ABH,6
  	LDWR	R30,0
  	MOVXR	R30,0,1	
  	LDWR	R101,5AA5H				;显示单元指令
	LDWR	R103,F1ABH
 	COMTXI	0,R101,4
 	RET
 	
; 	LDWR	R0,013AH		
;  	MOVXR	R30,1,1
;  	IJNE	R31,1,5
;  	LDWR	R30,0
;  	MOVXR	R30,0,1	
;  	LDWR	R101,5AA5H				;重复
;	LDWR	R103,F108H
; 	COMTXI	0,R101,4
; 	RET
;=============  打印和保存 =======================
ORDER_M:
;--- 检测页面的打印	----------	
		LDWR	R0,0137H		
  		MOVXR	R30,1,1
  		IJNE	R31,1,DATA_1	;打印键按下
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
  		LDWR	R101,5AA5H
		LDBR	R103,F2H,1
		LDWR	R0,0110H
	    MOVXR	R50,1,6
	    MOV		R50,R104,11
	    	    
	    	   LDWR  R0,0120H
	           LDBR	 R9,4,1
		B:	   MOVXR R50,1,1
	           IJNE  R50,FFH,3		;查看姓名是否位空
	           LDBR	 R50,0,8		;姓名空则发送空格
	           MOVXR R50,0,0		;R9决定发送的长度，4个字的空格
	           GOTO	 B1
	           INC	 R0,1,1
	           DEC	 R9,0,1
	           IJNE  R9,0,B
	    B1:    LDWR	    R0,0120H	
	    	   MOVXR	R50,1,4
	    	   MOV		R50,R115,8
	    
	    MOVDR	20H,R50,7
	    MOV		R50,R123,3			;打印的日期年月日
	    MOV		R54,R126,3			;打印的日期时分秒
	    LDWR	R0,0030H
	    MOVXR	R50,1,15
	    MOV		R50,R129,30
	    LDWR	R0,003FH
	    MOVXR	R50,1,6
	    MOV		R50,R159,10
	    LDWR	R0,0131H
	    MOVXR	R50,1,1
	    MOV		R51,R169,1
	    LDWR	R0,0132H
	    MOVXR	R50,1,1
	    MOV		R51,R170,1
	    
	           LDWR	 R0,0140H
	           LDBR	 R9,4,1
		C:	   MOVXR R50,1,1
	           IJNE  R50,FFH,3
	           LDBR	 R50,0,8
	           MOVXR R50,0,0	
	           GOTO	 C1
	           INC	 R0,1,1
	           DEC	 R9,0,1
	           IJNE  R9,0,C
	   C1:     LDWR	 R0,0140H        
 			   MOVXR	R50,1,4
  			   MOV		R50,R171,8
    
	    LDWR	R0,0170H	    
	    MOVXR	R50,1,11
	    MOV		R51,R179,1
	    MOV		R53,R180,1
	    MOV		R55,R181,1
	    MOV		R57,R182,1
	    MOV		R59,R183,1
	    MOV		R61,R184,1
	    MOV		R63,R185,1
	    MOV		R65,R186,1
	    MOV		R67,R187,1
	    MOV		R69,R188,1
	    MOV		R71,R189,1
	     
	    COMTXI	0,R101,89
		RET
	    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;实时测试存储;;;;;;;;;;;;;;;;;;;;;;;;	
DATA_1:
		LDWR	R0,3400H
		LDWR	R4,0049H
		LDWR	R6,60H
		MOVXL	2,1
		MOVXR	R252,1,1			;历史编号的序号1-100
		
		LDWR	R0,0138H		
  		MOVXR	R30,1,1
  		IJNE	R31,1,DATA_2
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
  		IJNE	R253,100,1
 		LDWR	R252,0
 		INC		R252,1,1
 		LDWR	R0,3400H
 		MOVXR	R252,0,1
		LDWR	R4,0049H
		LDWR	R6,60H				;历史编号序号加1
		MOVXL	3,1
  		
  		
  		
  		LDWR	R101,5AA5H
		LDBR	R103,F2H,1 		
		LDWR	R0,0110H
	    MOVXR	R50,1,6			;编号
	    MOV		R50,R104,11
	    	    
	    	   LDWR  R0,0120H
	           LDBR	 R9,4,1
		D:	   MOVXR R50,1,1
	           IJNE  R50,FFH,3
	           LDBR	 R50,0,8
	           MOVXR R50,0,0			;姓名
	           GOTO	 D1
	           INC	 R0,1,1
	           DEC	 R9,0,1
	           IJNE  R9,0,D
	    D1:    LDWR	    R0,0120H
	    	   MOVXR	R50,1,4
	    	   MOV		R50,R115,8
	    
	    MOVDR	20H,R50,7
	    MOV		R50,R123,3
;	    MOV		R54,R126,3		;时间
	    LDWR	R0,0030H
	    MOVXR	R50,1,15
	    MOV		R50,R126,30
;	    LDBR	R159,FFH,1		;FF无意义
	    LDWR	R0,0131H
	    MOVXR	R50,1,1
	    MOV		R51,R156,1		;性别
	    LDWR	R0,0132H
	    MOVXR	R50,1,1
	    MOV		R51,R157,1		;年龄
;	    
;	           LDWR	 R0,0140H
;	           LDBR	 R9,4,1
;		E:	   MOVXR R50,1,1
;	           IJNE  R50,FFH,3
;	           LDBR	 R50,0,8
;	           MOVXR R50,0,0			;送检者
;	           GOTO	 E1
;	           INC	 R0,1,1
;	           DEC	 R9,0,1
;	           IJNE  R9,0,E
;	   E1:     LDWR	 R0,0140H        
; 			   MOVXR	R50,1,4
;  			   MOV		R50,R162,8
;    
	    LDWR	R0,0170H	    
	    MOVXR	R50,1,11
	    MOV		R51,R158,1
	    MOV		R53,R159,1
	    MOV		R55,R160,1
	    MOV		R57,R161,1
	    MOV		R59,R162,1
	    MOV		R61,R163,1
	    MOV		R63,R164,1
	    MOV		R65,R165,1
	    MOV		R67,R166,1
	    MOV		R69,R167,1
	    MOV		R71,R168,1
	    
	    
 		
 		LDBR	R170,0,16
		MOV		R252,R172,2
		LDWR	R176,100
		MAC		R170,R174,R178
		MOV		R182,R170,4
		LDWR	R174,0070H
		LDWR	R176,0
		ADD		R170,R174,R178
		MOV		R182,R4,4
		
		LDWR	 R0,0300H        
 		MOVXR	 R101,0,35
		MOVXL	3,35
 		
 		
; 		MOV	 R20,R4,1
;	    LDBR R5,0,3
;	    MOVXL 1,60
;	    INC R20,0,1    ;90号字库,历史报告储存首字库，最大119号字库
;	    IJNE R20,120,1
;	    LDBR R20,90,1
;	    
;	    LDBR R4,88,1    ;88号字库，当前历史报告存到的字库位置
;	    LDBR R5,0,3
;	    LDWR R0,0009H
;	    MOVXR R19,0,1
;	    MOVXL 1,1
    	    
		RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;实时测试返回;;;;;;;;;;;;;;;;;;;;;;;;

DATA_2: 
		LDWR	R0,0139H		
  		MOVXR	R30,1,1
  		IJNE	R31,1,DATA_3
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
		LDWR	R0,0110H
		LDBR	R50,0,20
		MOVXR	R50,0,10
		LDWR	R0,0120H
		LDBR	R50,0,10
		MOVXR	R50,0,5
		LDWR	R0,0131H
		LDBR	R50,0,4
		MOVXR	R50,0,2
		LDWR	R0,0140H
		LDBR	R50,0,30
		MOVXR	R50,0,15
		LDWR	R0,0170H
		LDBR	R50,0,22
		MOVXR	R50,0,11
		
		RET 
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;查询;;;;;;;;;;;;;;;;;;;;;;;;
DATA_3:	
		LDWR	R0,00A1H		
  		MOVXR	R30,1,1
  		IJNE	R31,0,1
  		GOTO	H
  		LDWR	R30,0
  		MOVXR	R30,0,1
                  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;当前输入 		
  		LDWR	R0,0070H
	    MOVXR	R210,1,6
	    IJNE   	R210,0,1		;编号不为0则扫描
	    RET
;	    	   LDWR  R0,0080H
;	           LDBR	 R9,4,1
;		F:	   MOVXR R50,1,1
;	           IJNE  R50,FFH,3
;	           LDBR	 R50,0,8
;	           MOVXR R50,0,0
;	           GOTO	 F1
;	           INC	 R0,1,1
;	           DEC	 R9,0,1
;	           IJNE  R9,0,F
;	    F1:    LDWR	    R0,0080H
;	    	   MOVXR	R230,1,4
;	    	   
;	    	   IJNE   R210,0,1
;	    	   IJNE	  R230,0,1
;	    	  RET
	    	     ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;扫描历史
	      
 		LDWR 	R4,0070H
	    LDWR 	R6,100
	    LDWR	R0,3500H
   G:   MOVXL 	0,35	          
 		MOVXR	R101,1,35	
 			IJNE	R210,0,1
 			GOTO	N_RET
 			CJNE	R210,R104,11
 			CJNE	R211,R105,10
 			CJNE	R212,R106,9
 			CJNE	R213,R107,8
 			CJNE	R214,R108,7
 			CJNE	R215,R109,6
 			CJNE	R216,R110,5
 			CJNE	R217,R111,4
 			CJNE	R218,R112,3
 			CJNE	R219,R113,2
 			CJNE	R220,R114,1
 			GOTO	G1
 	M:		
;			IJNE	R230,0,1
; 			GOTO	N
; 			CJNE	R230,R115,8
; 			CJNE	R231,R116,7
; 			CJNE	R232,R117,6
; 			CJNE	R233,R118,5
; 			CJNE	R234,R119,4
; 			CJNE	R235,R120,3
; 			CJNE	R236,R121,2
; 			CJNE	R237,R122,1
; 			GOTO	G1
	 N:	    INC 	R6,1,100    ;查询下一个序号的保存记录
	 		LDWR	R230,2710H
	    	TESTS 	R6,R230,2
	    	IJNE	R0,0,G
	    	LDWR 	R6,100  
	    	RET
	    	
	G1:	
		
		LDWR	R101,5AA5H
		LDBR	R103,F2H,1
		
;		LDWR	R0,0210H
;		MOV		R104,R50,11
;		LDBR	R61,0,1
;	    MOVXR	R50,0,6
	    
	    LDWR	R0,0220H
	    MOV		R115,R50,8		;姓名
	    MOVXR	R50,0,4
	    
	    LDWR	R0,0262H
	    MOV		R123,R50,3		;日期年月日
	    LDBR	R53,0,1
	    MOVXR	R50,0,2
;	    LDWR	R0,0264H
;	    MOV		R126,R50,3
;	    LDBR	R53,0,1
;	    MOVXR	R50,0,2
	    	    
;	    LDWR	R0,1280H
;	    MOV		R126,R50,30
;	    MOVXR	R50,0,15
	    
;	    LDWR	R0,0260H
;	    LDBR	R50,0,1
;	    MOV		R159,R51,1
;	    MOVXR	R50,0,1
	    
	    LDWR	R0,0231H
	    LDBR	R50,0,1			;性别
	    MOV		R156,R51,1
	    MOVXR	R50,0,1
	    
	    LDWR	R0,232H
	    LDBR	R50,0,1
	    MOV		R157,R51,1
	    MOVXR	R50,0,1
	    
;	    LDWR	R0,0240H
;	    MOV		R162,R50,8
;	    MOVXR	R50,0,4
	    
	    LDWR	R0,0270H
	    LDBR	R50,0,22
	    MOV		R158,R51,1
	    MOV		R159,R53,1
	    MOV		R160,R55,1
	    MOV		R161,R57,1
	    MOV		R162,R59,1
	    MOV		R163,R61,1
	    MOV		R164,R63,1
	    MOV		R165,R65,1
	    MOV		R166,R67,1
	    MOV		R167,R69,1
	    MOV		R178,R71,1
	    MOVXR	R50,0,22
N_RET:	    
		RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;查询返回;;;;;;;;;;;;;;;;;;;;;;;;  
	H:	LDWR	R0,00A2H		
  		MOVXR	R30,1,1
  		IJNE	R31,1,9
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
		LDWR	R0,0070H
		LDBR	R50,0,20
		MOVXR	R50,0,10
		LDWR	R0,0220H
		LDBR	R50,0,10
		MOVXR	R50,0,5
		LDWR	R0,0231H
		LDBR	R50,0,4
		MOVXR	R50,0,2
		LDWR	R0,0270H
		LDBR	R50,0,22
		MOVXR	R50,0,11
		LDWR	R0,0262H
		LDBR	R50,0,6
		MOVXR	R50,0,3
		
		RET  
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;查询打印;;;;;;;;;;;;;;;;;;;;;;;;   
		LDWR	R0,0237H		
  		MOVXR	R30,1,1
  		IJNE	R31,1,44
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
  		LDWR	R101,5AA5H
		LDBR	R103,F2H,1
		LDWR	R0,0070H			;编号
	    MOVXR	R50,1,6
	    MOV		R50,R104,11
	    LDWR	R0,0220H			;姓名
	    MOVXR	R50,1,4
	    MOV		R50,R115,8
	    LDWR	R0,0262H			;检测时间
	    MOVXR	R50,1,2
	    MOV		R50,R123,3
;	    MOV		R54,R126,3
	    LDWR	R0,30H				;医院名
	    MOVXR	R50,1,15
	    MOV		R50,R126,30
;	     LDBR	R159,FFH,1
	    LDWR	R0,0231H
	    MOVXR	R50,1,1
	    MOV		R51,R156,1
	    LDWR	R0,232H
	    MOVXR	R50,1,1
	    MOV		R51,R157,1
;	    LDWR	R0,0240H
;	    MOVXR	R50,1,4
;	    MOV		R50,R162,8
	    LDWR	R0,0270H
	    MOVXR	R50,1,11
	    MOV		R51,R158,1
	    MOV		R53,R159,1
	    MOV		R55,R160,1
	    MOV		R57,R161,1
	    MOV		R59,R162,1
	    MOV		R61,R163,1
	    MOV		R63,R164,1
	    MOV		R65,R165,1
	    MOV		R67,R166,1
	    MOV		R69,R167,1
	    MOV		R71,R168,1
	    

	    
	    COMTXI	0,R101,68
		RET	
	
	

