@echo off
setlocal enabledelayedexpansion

:: Set input and output file names
set "input_image=GUI.png"
set "temp_image=temp.png"
set "final_output=GUI_Final.png"

:: Set coordinates file
set "coordinates_file=coordinates.txt"

:: Copy input image to temp file (to modify in-place)
copy /Y "%input_image%" "%temp_image%" >nul

:: Initialize counter
set "count=1"

:: Read each line from the coordinates file
for /F "tokens=1 delims=	" %%A in (%coordinates_file%) do (
    set "coordinates=%%A"
    
    :: Stop processing on the first empty line
    if "!coordinates!"=="exit" goto :done
    
    :: Draw the number onto the temp image
    magick "%temp_image%" -fill red -pointsize 50 -weight 700 -draw "text !coordinates! '!count!'" "%temp_image%"
    
    set /A count+=1
)

:done
:: Rename final output
move /Y "%temp_image%" "%final_output%" >nul
echo Processing complete! Final output: %final_output%
exit /b