#!/usr/bin/expect
set timeout -1
spawn svn co https://192.168.52.21/svn/Global5.0/11.测试/mcu_version_test/sdkag592eeu_lpm_test /home/xhh/auto_test/project/update_package/mcu_package
expect "haha" {send "123456\n"}
expect eof
