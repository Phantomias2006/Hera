/**
 * @file NexUpload.cpp
 *
 * The implementation of download tft file for nextion. 
 *
 * @author  Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date    2016/3/29
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * Modifications Copyright (C) 2019 Martin Koerner
 * 2019/05/04: Download tft file without sd card
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include "NexUpload.h"

#ifdef DEBUG_SERIAL_ENABLE
#define dbSerialPrint(a)    dbSerial.print(a)
#define dbSerialPrintln(a)  dbSerial.println(a)
#define dbSerialBegin(a)    dbSerial.begin(a)
#else
#define dbSerialPrint(a)    do{}while(0)
#define dbSerialPrintln(a)  do{}while(0)
#define dbSerialBegin(a)    do{}while(0)
#endif

NexUpload::NexUpload(uint32_t download_baudrate)
{
    _download_baudrate = download_baudrate;
    _download_state = TFT_DOWNLOAD_INIT;
    _undownloadByte = 0u;
}

void NexUpload::prepareUpload()
{
    if(!_undownloadByte)
    {
        dbSerialPrintln("file size not valid");
        _download_state = TFT_DOWNLOAD_ERROR;
    }

    _download_state = TFT_DOWNLOAD_IN_PROGRESS;
    _bytes_counter = 4096u;

    if(_getBaudrate() == 0)
    {
        dbSerialPrintln("get baudrate error");
        _download_state = TFT_DOWNLOAD_ERROR;
        return;
    }
    if(!_setDownloadBaudrate(_download_baudrate))
    {
        dbSerialPrintln("modify baudrate error");
        _download_state = TFT_DOWNLOAD_ERROR;
        return;
    }

    dbSerialPrintln("prepare upload ok");
}

void NexUpload::SetFileSize(size_t size)
{
    _undownloadByte = size;
}

void NexUpload::upload(uint8_t *data, size_t size)
{
    size_t i = 0;
    String string = String("");
    
    if(TFT_DOWNLOAD_IN_PROGRESS == _download_state)
    {
        for(i = 0; i < size; i++)
        {
            nexSerial.write(data[i]);
            _bytes_counter--;
            _undownloadByte--;
            
            if(!_bytes_counter)
            {
                dbSerialPrint("uploading... ");
                dbSerialPrint(_undownloadByte);
                dbSerialPrintln(" bytes left");

                _bytes_counter = 4096u;
                
                this->recvRetString(string, 500, true);  
                
                if(string.indexOf(0x05) != -1)
                { 
                    string = "";
                } 
                else
                {
                    _download_state = TFT_DOWNLOAD_ERROR;
                    _undownloadByte = 0u;
                    dbSerialPrintln("error during upload");
                    return;
                }
            }

            if(0u == _undownloadByte)
            {
                _download_state == TFT_DOWNLOAD_INIT;
                dbSerialPrintln("upload finished");
            }

        }
    }
    else if(TFT_DOWNLOAD_INIT == _download_state)
    {
        dbSerialPrintln("prepare upload missing");
    }
}

bool NexUpload::isUploading()
{
    if(TFT_DOWNLOAD_IN_PROGRESS == _download_state)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t NexUpload::_getBaudrate(void)
{
    uint32_t baudrate_array[7] = {115200,19200,9600,57600,38400,4800,2400};
    for(uint8_t i = 0; i < 7; i++)
    {
        if(_searchBaudrate(baudrate_array[i]))
        {
            _baudrate = baudrate_array[i];
            dbSerialPrintln("get baudrate");
            break;
        }
    }
    return _baudrate;
}

bool NexUpload::_searchBaudrate(uint32_t baudrate)
{
    String string = String("");  
    nexSerial.begin(baudrate);
    this->sendCommand("");
    this->sendCommand("connect");
    this->recvRetString(string, 500);  
    if(string.indexOf("comok") != -1)
    {
        return 1;
    } 
    return 0;
}

void NexUpload::sendCommand(const char* cmd)
{

    while (nexSerial.available())
    {
        nexSerial.read();
    }

    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
}

uint16_t NexUpload::recvRetString(String &string, uint32_t timeout,bool recv_flag)
{
    uint16_t ret = 0;
    uint8_t c = 0;
    long start;
    bool exit_flag = false;
    start = millis();
    while (millis() - start <= timeout)
    {
        while (nexSerial.available())
        {
            c = nexSerial.read(); 
            if(c == 0)
            {
                continue;
            }
            string += (char)c;
            if(recv_flag)
            {
                if(string.indexOf(0x05) != -1)
                { 
                    exit_flag = true;
                } 
            }
        }
        if(exit_flag)
        {
            break;
        }
    }
    ret = string.length();
    return ret;
}

bool NexUpload::_setDownloadBaudrate(uint32_t baudrate)
{
    String string = String(""); 
    String cmd = String("");
    
    String filesize_str = String(_undownloadByte,10);
    String baudrate_str = String(baudrate,10);
    cmd = "whmi-wri " + filesize_str + "," + baudrate_str + ",0";
    
    dbSerialPrintln(cmd);
    this->sendCommand("");
    this->sendCommand(cmd.c_str());
    delay(50);
    nexSerial.begin(baudrate);
    this->recvRetString(string,500);  
    if(string.indexOf(0x05) != -1)
    { 
        return 1;
    } 
    return 0;
}

