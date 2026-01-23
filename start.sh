#!/usr/bin/expect

if { [lindex $argv 0] == "" } {
    puts "参数1(3BWT/3DPA/3A0V)缺失"
    exit 1
}
if { [lindex $argv 1] == "" } {
    puts "参数2(JP/EU)缺失"
    exit 1
}
if { [lindex $argv 2] == "" } {
    puts "参数3(EEU/EROW)缺失"
    exit 1
}
# 设置超时为无限等待
set timeout -1

spawn sudo /home/cx/auto_test_test/build/auto_test_test [lindex $argv 0] [lindex $argv 1] [lindex $argv 2]

expect {
    # 匹配包含"password"或用户名"cx"的提示
    "*assword*"  { send "1\r"; exp_continue }
    "*cx:"       { send "1\r"; exp_continue }
    eof
}
