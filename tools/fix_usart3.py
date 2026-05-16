#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re

with open(r'D:\stm32\后置f4  2.0\后置f4\STM32\usart3.c', 'r', encoding='utf-8') as f:
    content = f.read()

# Find and replace the three problematic sprintf lines
# The issue is: \" should be \\\" but the current code has double escaped versions

# Replace the temperature line
old1 = 'sprintf(cmd, "AT+MQTTPUB=0,\\"$sys/%s/%s/thing/property/post\\",\\"{\\\"id\\":\\"123\\",\\"params\\":{\\"temperature\\":{\\"value\\":%d\\}}}\"",0,0"'
new1 = 'sprintf(cmd, "AT+MQTTPUB=0,\\"$sys/%s/%s/thing/property/post\\",\\"{\\\\n"id\\\\n":\\\\n"123\\\\n",\\\\n"params\\\\n":{\\\\n"temperature\\\\n":{\\\\n"value\\\\":%d}}}",0,0"'
# Actually just remove the trailing ,0,0"  part at the end

# Let's be simpler - just fix by removing the extra " at the end of string before ,0,0
# Original: ...value\\":%d}}}\"",0,0"
# Should be: ...value\\":%d}}}\",0,0"

content = content.replace('}}}\"\",0,0"', '}}}}\",0,0"')

with open(r'D:\stm32\后置f4  2.0\后置f4\STM32\usart3.c', 'w', encoding='utf-8') as f:
    f.write(content)

print("Fixed!")