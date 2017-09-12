@echo off

if "%4" == ""  goto build
f:\msxsl\msxsl.exe f:\home\depot\xml\stylesheets\docbook-xsl-1.50.0\html\titlepage.templates.xml f:\home\depot\xml\stylesheets\docbook-1.50.0\template\titlepage.xsl | f:\msxsl\msxsl.exe - f:\msxsl\to_utf8.xsl -o f:\home\depot\xml\stylesheets\docbook-xsl-1.50.0\html\titlepage.templates.xsl

:build
sx -x lower -x empty %2 >%3.xml
xsltproc  -o %3 f:\home\depot\xml\stylesheets\docbook-xsl-1.50.0\html\my\%1 %3.xml
del %3.xml
f:\tidy\tidy.exe -config f:\tidy\config.txt -m %3
f:\tidy\tidy_attr.py %3
