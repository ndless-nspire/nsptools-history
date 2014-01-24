@echo off
set DIR=%~dp0
cd %DIR%
java -jar logstash-1.3.3-flatjar.jar agent -f shipper.conf
