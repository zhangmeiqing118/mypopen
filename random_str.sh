#!/bin/bash
#########################################################################
# @File Name: random_str.sh
# @Author: zhangmeiqing
# @mail: zhangmeiqing_2009@126.com
# @Created Time: Thu 07 Sep 2017 07:55:13 AM CST
#########################################################################
str=`cat /dev/urandom | base64 | head -n 3|awk 'BEGIN{RS="\n";ORS="AAA";}{print $0}'`
echo ${str:0:128}
