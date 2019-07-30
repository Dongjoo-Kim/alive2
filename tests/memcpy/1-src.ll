declare void @memcpy(i8*, i8*, i32)

define i8 @f() {
  %p1 = alloca i8
  %p2 = alloca i8
  store i8 1, i8* %p1
  call void @memcpy(i8* %p2, i8* %p1, i32 3)
  %v = load i8, i8* %p2
  ret i8 %v
}
