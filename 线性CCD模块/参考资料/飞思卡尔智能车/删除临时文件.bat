::@echo off 

@for /d /r %%c in (Debug) do @if exist %%c ( rd /s /q "%%c" & echo     É¾³ýÄ¿Â¼%%c) 
@for /d /r %%c in (settings) do @if exist %%c ( rd /s /q "%%c" & echo     É¾³ýÄ¿Â¼%%c) 
@for /d /r %%c in (Release) do @if exist %%c ( rd /s /q "%%c" & echo     É¾³ýÄ¿Â¼%%c)  
@for /d /r %%c in (.svn) do @if exist %%c ( rd /s /q "%%c" & echo     É¾³ýÄ¿Â¼%%c)  


::@for /r %%c in (*.tmp) do  ( del   "%%c" & echo     É¾³ýÎÄ¼þ%%c)  
@for /r  %%c in (*.tmp ) do del "%%c"
pause