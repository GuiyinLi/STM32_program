  ; DWIN OS
	 ;R10—— 页面
	 ;R20—— 历史报告字库存储位置
 	 ;R30—— 读按键状态
 	 ;R50~—— 数据缓存
 	 ;0300H~—— 数据缓存
 	 ;R101~—— 串口发送数据缓存
 	 
SYS_INIT:

  IJNE  R200,0,MAIN	
  LDBR  R200,1,1;R200置一
 	;		   COMSET 0,651  ;COM1初始,9600bps
 	;		   LDWR		R0,0505H
 	;		   LDWR		R30,0000H
  	;		   MOVXR	R30,0,1
   				     
	           LDBR R4,85,1    ;85号字库，温度
	           LDBR R5,0,3
	           LDWR R0,0103H
	           MOVXL 0,1
	           
	           LDBR R4,86,1    ;86号字库，湿度
	           LDBR R5,0,3
	           LDWR R0,0104H
	           MOVXL 0,1
	           
	           LDBR R4,87,1    ;87号字库，气压
	           LDBR R5,0,3
	           LDWR R0,0105H
	           MOVXL 0,1
	           
	           
 MAIN:	           
;;;;;;;;;;;;存储温度、湿度、气压
	           LDWR	R0,0106H		
	           MOVXR R30,1,1
	           IJNE	R31,1,23
		       	     	    
	           LDBR R4,85,1    ;85号字库，温度
	           LDBR R5,0,3
	           LDWR R0,0103H
	           MOVXL 1,1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	           LDBR R4,86,1    ;86号字库，湿度
	           LDBR R5,0,3
	           LDWR R0,0104H
	           MOVXL 1,1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
	           LDBR R4,87,1    ;87号字库，气压
	           LDBR R5,0,3
	           LDWR R0,0105H
	           MOVXL 1,1
	           
	           LDWR	R0,0106H	;串口发送温度、湿度、气压设定值	
	           MOVXR R30,1,1
	           IJNE	R31,1,8
	           LDWR	R30,0
	           MOVXR R30,0,1
  		
	           LDWR	R101,5AA5H
	           LDWR	R0,0103H
	           MOVXR R50,1,3
	           MOV	R50,R103,6
	           COMTXI 0,R101,8
	           
	           GOTO	OVER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;温度、湿度、气压设定;;;;;;;;;;;;;;;;;;;;;;;;	
		LDWR	R0,0106H		
  		MOVXR	R30,1,1
  		IJNE	R31,1,8
  		LDWR	R30,0
  		MOVXR	R30,0,1
  		
  		LDWR	R101,5AA5H
		LDWR	R0,0103H
	    MOVXR	R50,1,3
	    MOV		R50,R103,6
		COMTXI	0,R101,8
    	GOTO	OVER 
     
	 OVER:	END          