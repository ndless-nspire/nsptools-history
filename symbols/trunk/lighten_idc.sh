#!/bin/sh
# Usage: lighten_idc.sh <in >out
# Keeps what should be shared from an IDA .idc file
egrep 'MakeName|MakeRptCmt' | egrep -v '"a[A-Z]' | egrep -v 'null|jumptable|default'
