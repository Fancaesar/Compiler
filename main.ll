declare void @putint(i32 ) #1
declare void @putch(i32 ) #1
define dso_local i32 @main() #0 {
    %1 = add i32 5 , 3
    call void @putint(i32  %1)
    call void @putch(i32  10)
    %2 = sub i32 %1 , 1
    call void @putint(i32  %2)
    call void @putch(i32  10)
    %3 = mul i32 %1 , 10
    call void @putint(i32  %3)
    call void @putch(i32  10)
    ret i32 0;
 }
