#!/bin/bash
IN=$1
OUT=$2

tidy --quote-nbsp no -asxml -utf8 --new-blocklevel-tags vne $IN 2>tmp.log |sed '1s/<html.*/<html>/'|xsltproc z.xslt - > $OUT


