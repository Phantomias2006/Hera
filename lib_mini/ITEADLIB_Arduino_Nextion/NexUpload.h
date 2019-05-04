/**
 * @file NexUpload.h
 *
 * The definition of class NexUpload. 
 *
 * @author Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date 2016/3/29
 *
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
#ifndef __NEXUPLOAD_H__
#define __NEXUPLOAD_H__
#include <Arduino.h>
#include "NexHardware.h"

/**
 * @addtogroup CoreAPI 
 * @{ 
 */

typedef enum{
    TFT_DOWNLOAD_INIT = 0,
    TFT_DOWNLOAD_IN_PROGRESS,
    TFT_DOWNLOAD_ERROR,
} t_tft_download_state;

/**
 *
 * Provides the API for nextion to download the ftf file.
 */
class NexUpload
{
public: /* methods */

    /**
     * Constructor. 
     * 
     * @param download_baudrate - set download baudrate.
     */
    NexUpload(uint32_t download_baudrate);
    
    /**
     * destructor. 
     * 
     */
    ~NexUpload(){}
    
    /*
     * prepare download.
     *
     * @return none.
     */
    void prepareUpload();
    
    /*
     * set file size.
     *
     * @return none.
     */
    void SetFileSize(size_t size);

    /*
     * start download.
     *
     * @return none.
     */
    void upload(uint8_t *data, size_t size);
    
    /*
     * is uploading.
     *
     * @return true if upload is in progress.
     */
    bool isUploading();

private: /* methods */

    /*
     * get communicate baudrate. 
     * 
     * @return communicate baudrate.
     *
     */
    uint16_t _getBaudrate(void);

    /*
     * search communicate baudrate.
     *
     * @param baudrate - communicate baudrate.
     *   
     * @return true if success, false for failure. 
     */
    bool _searchBaudrate(uint32_t baudrate);

    /*
     * set download baudrate.
     *
     * @param baudrate - set download baudrate.
     *   
     * @return true if success, false for failure. 
     */
    bool _setDownloadBaudrate(uint32_t baudrate);
    
    /*
     * Send command to Nextion.
     *
     * @param cmd - the string of command.
     *
     * @return none.
     */
    void sendCommand(const char* cmd);

    /*
     * Receive string data. 
     * 
     * @param buffer - save string data.  
     * @param timeout - set timeout time. 
     * @param recv_flag - if recv_flag is true,will braak when receive 0x05.
     *
     * @return the length of string buffer.
     *
     */   
    uint16_t recvRetString(String &string, uint32_t timeout = 100,bool recv_flag = false);
    
private: /* data */ 
    uint32_t _baudrate; /*nextion serial baudrate*/
    uint32_t _undownloadByte; /*undownload byte of tft file*/
    uint32_t _download_baudrate; /*download baudrate*/
    t_tft_download_state _download_state; /*download state*/
    uint16_t _bytes_counter; /*keep track of 4k download mechanism*/
};
/**
 * @}
 */

#endif /* #ifndef __NEXDOWNLOAD_H__ */
