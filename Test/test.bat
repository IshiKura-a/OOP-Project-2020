@echo off

set /a N_REC = 30
set /a N_CASE = 6

echo "Testing..."
for /L %%i in (1,1,%N_REC%) do (
    :: echo Test%%i
    for /L %%j in (1,1,%N_CASE%) do (
        :: echo Test%%i.%%j
        %%j.exe >> test%%j.log
    )
)

echo "Extracting..."
for /L %%i in (1,1,%N_CASE%) do (
    echo Test%%i >> output.log
    cat test%%i.log | sed "s/[^0-9]//g" >> output.log
    echo # >> output.log
)

pause